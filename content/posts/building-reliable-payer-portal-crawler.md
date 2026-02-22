# Building a Reliable Payer Portal Crawler: handling 2FA, email automation, and adapter patterns

Payer/provider portals are among the nastiest web scraping targets I’ve worked with: flaky JS, inconsistent HTML across providers, PDFs masquerading as primary content, email-based 2FA, and aggressive rate limits. Over time I built a crawler that copes with these realities in production: adapter-based payer integrations, a hybrid browser/http fetch model, disk caching for reproducible runs, and an async pipeline that favors idempotent re-runs over brittle request-level retries.

In this post I’ll walk through why these choices mattered, the concrete implementation details I used (with real code excerpts), and the operational playbook I settled on for CI and seed-data generation.

## The problem we were solving (short version)

- Many payer portals are JS-heavy and require a real browser for some flows (login, PDF downloads behind auth). Simple http requests fail.
- Different payers expose data very differently — some serve HTML pages, others PDFs. A single parser quickly becomes a tangle of special cases.
- Two-factor auth (2FA) is common and often delivered via email. Manual intervention breaks CI and scheduled runs.
- Downloads are flaky and rate-limited. Retries at the HTTP level are helpful, but so is a design that tolerates partial failures and can resume.
- For CI and downstream systems we needed reproducible seed data — once a scrape succeeded we should be able to re-run parsing without hitting the live site.

These problems drove a few clear design goals: adapter-based payer integrations, hybrid fetch modes (httpx vs Playwright), a disk cache for downloaded raw artifacts (PDFs), an async pipeline with discovery and idempotency, and automated email-based 2FA where possible.

## High level design

- Adapter pattern: each payer implements a PayerAdapter with: get_seeds(), establish_session(), fetch(), parse(), and postprocess(). This keeps payer-specific quirks isolated.
- Hybrid fetch: use httpx for public pages/PDFs, and Playwright for login and authenticated downloads. Configure per-adapter which mode to use.
- Disk cache: store raw artifacts on disk (~/ .cache/ppr/<payer>/) so downloads are single-shot and parsing is repeatable.
- Async pipeline: a work queue feeds concurrent fetch tasks and a parse queue feeds a pool of parse workers (threads or processes depending on CPU-bound parsing).
- Email automation: for portals that send 2FA by email we poll a mailbox (MS Graph via client credentials) for the code and inject it into the browser.

Below I’ll walk through the important code surfaces.

## Adapter example: Blue Shield CA (hybrid fetch + disk cache)

I used an adapter that knows whether a given PDF requires authentication and routes downloads accordingly. Here’s the core fetch method (trimmed):

```python
# src/payer_policy_repo/payers/blueshieldca/adapter.py (excerpt)
async def fetch(self, page: Page, item: WorkItem) -> RawContent:
    # prepare file name and auth flag
    filename = f"{item.policy_id or 'unknown'}.pdf"
    requires_auth = item.metadata.get("requires_auth", False)

    # check disk cache first
    if self._cache.exists(filename):
        path = self._cache.get_path(filename)
        return RawContent(..., raw_value=str(path), raw_type="file_pdf", size=path.stat().st_size)

    # download via authenticated browser or httpx depending on the flag
    if requires_auth:
        pdf_bytes = await download_pdf_via_browser(page, item.url)
    else:
        pdf_bytes = await download_pdf(item.url)

    path = self._cache.write(filename, pdf_bytes)
    return RawContent(..., raw_value=str(path), raw_type="file_pdf", size=len(pdf_bytes))
```

Why this matters:
- We only use the heavy browser path for the subset of files that actually need it. That saves resources and improves throughput.
- The DiskCache prevents re-downloading large PDFs when parsing needs to be re-run.

## Disk cache (simple, reliable)

The disk cache is intentionally small and predictable. It’s just a filesystem store per payer.

```python
# src/payer_policy_repo/cache.py
class DiskCache:
    def __init__(self, payer_name: str, base_dir: Optional[Path] = None):
        self.cache_dir = (base_dir or Path.home() / ".cache" / "ppr") / payer_name
        self.cache_dir.mkdir(parents=True, exist_ok=True)

    def get_path(self, filename: str) -> Path:
        return self.cache_dir / filename

    def exists(self, filename: str) -> bool:
        return self.get_path(filename).exists()

    def write(self, filename: str, data: bytes) -> Path:
        path = self.get_path(filename)
        path.write_bytes(data)
        return path

    def read(self, filename: str) -> Optional[bytes]:
        path = self.get_path(filename)
        return path.read_bytes() if path.exists() else None
```

This enables two important operational patterns:
- Seed runs: download everything once and commit the raw artifacts to a seed database so CI doesn’t need to scrape live sites.
- Re-parse from disk: if parsing logic changes, we re-run parse jobs against cached files instead of hammering the payer portal again.

## Login and 2FA automation (Playwright + email polling)

Blue Shield CA’s login flow required an interactive browser session and occasionally a 6-digit code sent by email. I automated this by combining Playwright for browser interactions and an Outlook/MS Graph client for email polling.

Here’s the important snippet where the login flow detects 2FA and delegates to the email client:

```python
# src/payer_policy_repo/payers/blueshieldca/login.py (excerpt)
# After submitting username/password and detecting 2FA on the page:
from ...email import OutlookClient

client = OutlookClient()
email = await client.wait_for_email(sender_filter=BLUESHIELD_2FA_SENDER, timeout_seconds=settings.EMAIL_2FA_TIMEOUT, poll_interval=settings.EMAIL_2FA_POLL_INTERVAL, since=login_submit_time)
code = client.extract_code(email, pattern=BLUESHIELD_2FA_CODE_PATTERN)
await _enter_2fa_code(page, code, cache_dir)
```

A few implementation notes:
- I used MSAL client-credentials (app-only) against Microsoft Graph so the email polling is automated and doesn’t depend on a user signing in interactively.
- Polling is a simple loop that queries messages received since the time we submitted the login, with a configurable poll interval and timeout. Once the email arrives we run a regex against the message body to extract the 6-digit code.

The polling client is intentionally simple: it uses Graph’s messages endpoint with a filter by receivedDateTime and performs in-Python filtering by sender/subject where needed. This produces reliable results for the 2FA emails we care about.

Security note: because we’re using client credentials, the service principal needs Mail.Read application permission in Azure AD. Treat those secrets like other production credentials.

## Async pipeline and parsing strategy

The pipeline is a queue-driven orchestrator with three main stages:
- Seed discovery: adapter.get_seeds() returns initial WorkItems.
- Fetch stage: concurrent download tasks (Playwright page pool or httpx) produce RawContent objects.
- Parse stage: a pool of parse workers executes adapter.parse(raw) either in threads (I/O-bound) or processes (CPU-bound PDF parsing).

Key bits from the pipeline orchestration:

```python
# src/payer_policy_repo/pipeline.py (excerpt)
# _fetch() runs many fetch tasks concurrently and pushes RawContent into parse_queue
async def _fetch(self, session):
    async def fetch_one(item, idx):
        if self.adapter.browser_fetch:
            async with pool.page() as page:
                raw_content = await self.adapter.fetch(page, item)
                await self.parse_queue.put(raw_content)
        else:
            async with semaphore:
                raw_content = await self.adapter.fetch(cookie_page, item)
                await self.parse_queue.put(raw_content)

# _parse_worker() runs parsing in an executor (thread or process)
async def _parse_worker(self, executor, worker_id, loop):
    raw_content = await self.parse_queue.get()
    if self.adapter.parse_executor == 'process':
        result = await loop.run_in_executor(executor, _run_parse, type(self.adapter), raw_content)
    else:
        result = await loop.run_in_executor(executor, self.adapter.parse, raw_content)
    # store results, enqueue discovered items
```

Parsing PDFs is CPU-bound and often relies on native libraries (PyMuPDF). For that reason I run parse() in a process pool (so it escapes the GIL and avoids interpreter-level contention). HTML parsing stays in-thread.

### Failure model: idempotent runs over per-request retries

An important design choice was to tolerate per-request failures (log and skip) and make the pipeline idempotent at the run level. The pipeline keeps track of existing URLs in storage and will not re-fetch them unless a force flag is used. That means: if a few download attempts fail due to transient network issues, the operator can re-run the pipeline and it picks up what was missed — without re-downloading everything.

Concretely:
- The fetch and parse loops catch broad exceptions, increment error counters, and continue.
- The DiskCache and storage track what succeeded.

This avoids complex retry/backoff logic embedded in every downloader. In practice this model proved robust when scraping flaky third-party portals: one robust seed run, and later re-parses from disk.

## Retry and backoff (what we did — and consciously didn’t do)

I intentionally kept the codebase light on per-request retry libraries. There are no tenacity/decorator retries in the downloader code. Instead there are these measures:
- Randomized polite delays between requests (jitter) to avoid rate limiting.
- Per-adapter fetch_concurrency tuning (some payers tolerate more parallel httpx connections than others).
- Browser anti-detection in the Playwright session and human-like typing delays when required.

Where errors happen, the pipeline logs them and continues; the operator can re-run the pipeline. This pattern trades off immediate robustness for simplicity and easier reasoning about when to re-run. If you need aggressive in-request retries you can add a retry decorator around download_pdf() and download_pdf_via_browser().

## Seeder and CI (how to make scraping reproducible)

Because scraping a payer portal is brittle and occasionally requires secrets or manual verification, I built a separable seed pipeline that:
1. Runs the crawler manually or via a controlled CI pipeline (one-off or scheduled) with secrets in a protected variable group.
2. Dumps the resulting seed database (raw artifacts + parsed policies) into a mongodump artifact.
3. Uses that artifact as the canonical input for downstream jobs/tests.

The seed runner is a tiny orchestrator script that invokes the async pipeline for configured payers:

```python
# tools/payer-policy-repo/gen-seed.py
async def create_seed_data():
    for payer in ["unitedhealth", "aetna", "blueshieldca"]:
        stats = await run_pipeline_async(payer=payer, max_depth=2, headless=True, mongo_uri="mongodb://localhost:27017", mongo_db="seed")
        print(f"Completed {payer}: {stats}")
```

Operational notes:
- The seed pipeline runs under a separate CI job with long timeouts and protected credentials (Azure AD client secret, mailbox address, payer credentials).
- The CI job dumps the seed DB and publishes it as a build artifact. That artifact becomes the reproducible input for tests and downstream parsing tasks.

## Gotchas and lessons learned

1. Don’t over-automate request-level retries early. On these sites a single retry never fixed layout-related failures; re-runs with fresh cookies/headers or manual intervention often did. Idempotent runs + disk cache are more practical.
2. Prefer process pools for PDF parsing. Native libraries and CPU-bound extraction behave poorly under GIL-limited threads.
3. Treat 2FA automation as optional. If email credentials aren’t present, fall back to a manual-wait path and capture screenshots and HTML for debugging.
4. Keep payer-specific logic out of generic parsing. The adapter pattern prevented a tangled single parser.
5. Log aggressively and capture artifacts (screenshots, HTML, pdfs) on failures — they’re invaluable for debugging brittle flows.

## What I’d do differently now

- Add optional per-request retry/backoff (configurable) for HTTP downloads with exponential backoff and circuit breakers. The current run-level retry works, but retries can make individual runs more successful and reduce operator cycles.
- Add a small suite of integration tests that mock Playwright and the email Graph client to exercise the login+2FA flow deterministically.
- Improve observability: emit structured events for fetch/parse failures (so we can alert on specific payers falling below thresholds) and track per-payer success rates.

## Operational checklist (how we run this reliably)

- Use a dedicated service principal with Mail.Read application permission and a mailbox for 2FA retrieval. Protect its secrets in a variable group.
- Run seed pipeline in a controlled CI job (long timeout). After success, mongodump and publish artifact.
- For recurring updates, run the pipeline on a cadence appropriate for the payer (monthly, weekly depending on churn).
- If a payout fails repeatedly, collect the cache dir (screenshots + html + PDFs) and inspect — those artifacts tell the story.

## Closing notes

The combination of adapter patterns, a hybrid fetch model, disk caching, and a pipeline that tolerates partial failures makes scraping real-world payer portals practical and maintainable. It’s not glamorous: a lot of the work is in engineering predictable failure modes and the operational playbook for re-running and seeding data. But once in place, it scales: new payers are added as small adapters, and the pipeline stays stable.

If you want the key files to inspect, start with these paths in the codebase:
- src/payer_policy_repo/pipeline.py (orchestrator)
- src/payer_policy_repo/cache.py (disk cache)
- src/payer_policy_repo/payers/blueshieldca/login.py (Playwright login + 2FA)
- src/payer_policy_repo/payers/blueshieldca/adapter.py (fetch + cache logic)
- src/payer_policy_repo/payers/blueshieldca/scraper.py (seed discovery)
- tools/payer-policy-repo/gen-seed.py (seed runner)

Author: Zeeshan (zkhan1093@gmail.com)

If you try this approach and hit a tricky payer, ping me — I’m happy to share debugging heuristics and the snags I hit while stabilizing 2FA and browser-driven downloads.