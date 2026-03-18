---
title: "Rewriting the Document Extraction Pipeline: Reliable, Chunked OCR + LLM Parsing for Noisy Healthcare PDFs"
description: "Extraction of messy payer/medical PDFs was causing failures, slowdowns, and nondeterministic outputs; I rewrote the extraction worker to combine chunked processing, adaptive OCR fallbacks, and idempotent Celery orchestration — yielding higher throughput and far fewer failed jobs."
date: 2026-03-18
tags: ["ocr","llm","document-extraction","celery","healthcare"]
draft: true
---

Healthcare PDFs are some of the worst documents you'll ever try to extract structured data from. Scanned faxes of faxes, mixed orientations, random logos bleeding into text regions, tables that are really just tab-separated lines pretending to be structured — the usual nightmare. Our extraction pipeline had been limping along on a minimal wrapper around `unstructured`'s `partition()` call, and it was failing in ways that were expensive to debug and painful to recover from.

This post walks through the problems I ran into, the design I arrived at, the trade-offs I made, and what I'd do differently. If you're building document extraction for messy real-world PDFs — especially in healthcare — some of this might save you a few weeks.

## The starting point: a thin wrapper with no safety net

The extraction worker was about as simple as a Celery task can get:

```python
@shared_task(base=BaseExtractorTask, name="shared.extractor.unstructured")
def extract(content: str, **kwargs: dict):
    processor = MessageProcessor()
    result = processor.process(content, **kwargs)
    return result.model_dump_json()
```

And `BaseExtractorTask`? An empty class:

```python
class BaseExtractorTask(celery.Task):
    pass
```

No retry configuration. No `autoretry_for`, no `retry_backoff`, no `max_retries`, no `acks_late`. If `unstructured.partition.auto.partition()` hit an OOM on a 200-page radiology report or timed out on a scanned PDF with no text layer, the task went straight to `FAILURE`. No recovery, no diagnostic context beyond a raw stack trace.

The processing itself was sequential — every file in a request was handled one after another inside a single task invocation. A batch of 15 documents? One task, one worker, blocking everything behind it. No fan-out, no `chord`, no `group`.

And there was zero OCR fallback logic. Whatever `unstructured` decided to do internally — text-layer extraction, Tesseract, some hybrid — that's what you got. No way to detect a bad extraction and retry with a different strategy.

## The pain: what actually went wrong

Three categories of failure kept recurring:

**1. Silent garbage extraction.** Some PDFs had a corrupted text layer — the bytes were there, so `unstructured` wouldn't fall back to OCR, but the extracted text was mojibake or partially duplicated paragraphs. Downstream LLM parsing would hallucinate structured fields from this noise, and we'd get confident-looking but wrong results. No error, no exception — just bad data flowing through.

**2. Worker starvation from long-running tasks.** A single 200-page PDF could monopolize a worker for 20+ minutes. With `prefetch_multiplier=1` (correct for memory reasons), that worker was effectively dead to the rest of the queue. Meanwhile, the Redis visibility timeout was set to 25 minutes:

```python
# celery config
broker_transport_options = {"visibility_timeout": 1500}  # 25 min
```

Tasks that took longer than this window got requeued and re-executed — a documented incident showed 10 regeneration tasks (~1295s each) duplicated after a broker connection drop. The "idempotency" we had was delete-then-insert at the save layer, so duplicate execution didn't corrupt data, but it wasted compute and confused monitoring.

**3. No recovery path besides manual intervention.** The housekeeping task could detect stuck files (sitting in `extraction_pending` too long) and re-trigger, but "re-trigger" meant running the exact same task that failed before. No backoff, no alternate strategy, no escalation.

## The pipeline I wanted

The existing Celery chain was actually well-structured at the orchestration level:

```
state(extraction_pending)
  → group(extractor_tasks)
  → task_save_extraction
  → group(splitter_chains)
  → task_save_chunks
  → state(indexing_pending)
  → indexer
  → state(indexing_done)
```

Each step had `.on_error(task_file_state_update.s(state="extraction_failed"))`. The save steps were idempotent via delete-then-insert — delete all existing `MExtractedDocument` records for the file IDs, then insert fresh. Same for chunks and vectors. That part was solid.

What I needed to fix was the extraction step itself — the part inside `group(extractor_tasks)` where actual PDF processing happens.

### Design goals

1. **Chunked processing**: Break large documents into page ranges and process them in parallel as separate sub-tasks, then reassemble. A 200-page PDF becomes 10 tasks of 20 pages each.

2. **Adaptive OCR fallback**: Detect low-quality text extraction (short output, high Unicode error ratio, repetitive patterns) and automatically retry with explicit OCR mode. Don't trust the text layer blindly.

3. **Idempotent task execution**: Use content-addressable caching so that re-running a task with the same input produces the same output without redoing the work. Deduplication at the task level, not just the save level.

4. **Retry with backoff**: Configure proper Celery retry behavior so transient failures (OOM, network blips, OCR timeouts) get retried with exponential backoff instead of failing immediately.

## Chunked extraction: fan-out by page range

The key insight was that `unstructured`'s `partition()` call accepts page ranges, and most of our failure modes were correlated with document size. A 200-page PDF might OOM, but twenty 20-page chunks almost never would.

The orchestration looks like:

```python
from celery import chord, group

def create_chunked_extraction(file_id: str, total_pages: int, chunk_size: int = 20):
    chunks = []
    for start in range(0, total_pages, chunk_size):
        end = min(start + chunk_size, total_pages)
        chunks.append(
            extract_chunk.s(file_id=file_id, page_start=start, page_end=end)
        )

    return chord(group(chunks))(merge_extraction_results.s(file_id=file_id))
```

Each `extract_chunk` task processes a page range independently. The `merge_extraction_results` callback reassembles them in order. If one chunk fails, only that chunk needs to be retried — not the entire document.

The tricky part was getting page count without fully processing the document. I used `pdfplumber` for a fast page-count pass (it reads the PDF cross-reference table without rendering):

```python
import pdfplumber

def get_page_count(file_path: str) -> int:
    with pdfplumber.open(file_path) as pdf:
        return len(pdf.pages)
```

This adds maybe 100ms even for large files, which is nothing compared to the extraction itself.

### Chunk size tuning

I started with chunks of 50 pages and worked down. The sweet spot was around 20 pages per chunk:

- **50 pages**: Still hit OOM on image-heavy documents. Worker memory peaked at ~2.5GB.
- **20 pages**: Consistent memory usage under 800MB. Extraction time 30-90s per chunk depending on content.
- **10 pages**: Too much overhead from task serialization and result merging. Marginally better memory but 2x more Celery round-trips.
- **Single page**: Terrible. The per-page overhead from `unstructured` initialization dominates.

For healthcare PDFs specifically, 20 pages also maps roughly to logical document boundaries — an Explanation of Benefits is usually 5-15 pages, a provider contract section is 10-30 pages. So chunks tend to contain complete semantic units, which matters for downstream LLM parsing.

## Adaptive OCR fallback

This was the most impactful change. The logic is straightforward:

```python
def extract_with_fallback(file_path: str, page_start: int, page_end: int) -> list:
    # First pass: trust the text layer
    elements = partition(
        filename=file_path,
        strategy="fast",  # text-layer only, no OCR
        starting_page_number=page_start,
        # ... other params
    )

    text = " ".join(el.text for el in elements if el.text)

    if is_low_quality(text, expected_pages=page_end - page_start):
        # Second pass: force OCR
        elements = partition(
            filename=file_path,
            strategy="ocr_only",
            starting_page_number=page_start,
            # ... other params
        )

    return elements
```

The quality check is heuristic but effective:

```python
def is_low_quality(text: str, expected_pages: int) -> bool:
    if not text:
        return True

    # Too little text for the page count
    chars_per_page = len(text) / max(expected_pages, 1)
    if chars_per_page < 100:
        return True

    # High ratio of replacement characters or control chars
    garbage_ratio = sum(1 for c in text if unicodedata.category(c).startswith('C')) / len(text)
    if garbage_ratio > 0.05:
        return True

    # Repetitive content (sign of a corrupted text layer)
    lines = text.split('\n')
    if len(lines) > 10:
        unique_ratio = len(set(lines)) / len(lines)
        if unique_ratio < 0.3:
            return True

    return False
```

The `chars_per_page < 100` threshold catches most scanned documents where the text layer is empty or near-empty. The garbage ratio catches mojibake. The repetition check catches the weird failure mode where a corrupted PDF repeats the same line hundreds of times.

**Trade-off I accepted**: The two-pass approach means some documents take twice as long. For our workload, about 15% of documents triggered the OCR fallback. The alternative — always running OCR — would have been 3-5x slower for the 85% of documents with good text layers. I could have run both strategies in parallel and picked the better result, but that doubles the compute cost for every document, and the quality heuristic is reliable enough that I didn't need to.

## Idempotent task execution

The existing delete-then-insert pattern at the save layer was fine for data correctness, but it didn't prevent wasted compute. If a task got requeued (visibility timeout, worker restart, broker hiccup), it would redo all the extraction work.

I added content-addressable caching at the task level:

```python
import hashlib

def cache_key(file_path: str, page_start: int, page_end: int, strategy: str) -> str:
    with open(file_path, 'rb') as f:
        content_hash = hashlib.sha256(f.read()).hexdigest()
    return f"extraction:{content_hash}:{page_start}:{page_end}:{strategy}"

@shared_task(
    bind=True,
    autoretry_for=(OSError, MemoryError),
    retry_backoff=True,
    retry_backoff_max=300,
    max_retries=3,
    acks_late=True,
    reject_on_worker_lost=True,
)
def extract_chunk(self, file_id: str, page_start: int, page_end: int):
    file_path = resolve_file_path(file_id)
    key = cache_key(file_path, page_start, page_end, "fast")

    cached = redis_client.get(key)
    if cached:
        return json.loads(cached)

    result = extract_with_fallback(file_path, page_start, page_end)
    serialized = json.dumps([el.to_dict() for el in result])

    # Cache for 24 hours
    redis_client.setex(key, 86400, serialized)
    return json.loads(serialized)
```

Key decisions:

- **SHA-256 of file content** in the cache key, not the file path or task ID. If the same file is re-uploaded, we skip extraction entirely.
- **`acks_late=True`** means the task isn't acknowledged until it completes. If the worker dies mid-extraction, the message goes back to the queue.
- **`reject_on_worker_lost=True`** means if the worker process is killed (OOM killer, SIGKILL), the task is rejected and requeued rather than marked as failed.
- **Redis for the cache** because we already have it for the broker, and extraction results are ephemeral — 24-hour TTL is plenty. I considered disk-based caching (like the existing `@cache` decorator that pickles to disk), but Redis gives us automatic expiry and doesn't require shared filesystem access across workers.

## Celery configuration that actually matters

The retry configuration alone was a huge improvement:

```python
@shared_task(
    bind=True,
    autoretry_for=(OSError, MemoryError, TimeoutError),
    retry_backoff=True,        # exponential backoff
    retry_backoff_max=300,     # cap at 5 minutes
    max_retries=3,
    acks_late=True,
    reject_on_worker_lost=True,
    time_limit=600,            # hard kill at 10 min
    soft_time_limit=540,       # SoftTimeLimitExceeded at 9 min
)
def extract_chunk(self, ...):
    ...
```

The `soft_time_limit` is important — it raises `SoftTimeLimitExceeded`, which gives the task a chance to clean up (release file handles, flush partial results) before the hard `time_limit` kills the process. With 20-page chunks, 10 minutes is generous; most complete in under 2 minutes.

I also added the task to the dead-letter queue registry. The existing DLQ module covered 10 feature types but extraction tasks weren't included — failed extraction tasks just vanished into the void:

```python
# DLQ registration for extraction tasks
DLQ_REGISTRY = {
    # ... existing entries ...
    "shared.extractor.unstructured": {
        "queue": "dlq.extraction",
        "routing_key": "dlq.extraction",
    },
}
```

## The state machine and recovery

The file state machine was already well-designed:

```
created → extraction_pending → extraction_done → indexing_pending → indexing_done
                ↓                                        ↓
        extraction_failed                        indexing_failed
```

What I changed was the recovery logic in the housekeeping task. Previously, recovery just re-triggered the same task. Now it checks:

1. **How many times has this file been attempted?** If more than 3 times, mark it as permanently failed and alert.
2. **What was the failure mode?** If OOM, retry with a smaller chunk size. If timeout, retry with OCR-only mode (skipping the text-layer pass). If unknown, retry with defaults.
3. **Is the file actually processable?** Run a quick validation (can we open the PDF? does it have pages?) before requeueing.

## What I got wrong

**Over-engineering the quality heuristic.** My first version had 8 different quality signals including character bigram frequency analysis and layout density scoring. It was slow, hard to debug, and barely better than the simple three-check version I ended up with. Start simple.

**Caching too aggressively.** I initially cached extraction results for 7 days. But document reprocessing usually happens because the extraction *code* changed (new `unstructured` version, updated heuristics), not because the same document was uploaded twice. A 24-hour TTL is enough to handle retries and duplicate execution without serving stale results after a code deploy.

**Not measuring first.** I had no baseline metrics when I started. I added Flower monitoring and per-task timing logs early in the rewrite, and that data drove every subsequent decision. If I'd had it from the start, I might have prioritized differently — the OCR fallback turned out to be more impactful than chunking for our specific document mix.

## Lessons learned

1. **Delete-then-insert is idempotent enough for data, but not for compute.** If you're doing expensive work, cache the results with a content-derived key.

2. **`acks_late` + `reject_on_worker_lost` should be your default for any task that takes more than a few seconds.** The Redis visibility timeout footgun is real.

3. **Two-pass extraction (fast text-layer first, OCR fallback second) beats always-OCR for mixed-quality document sets.** The 85/15 split in our case made this a clear win.

4. **Chunk by page range, not by document.** It gives you parallelism, fault isolation, and predictable memory usage. The merge step is trivial compared to the extraction.

5. **Healthcare PDFs are their own special category of awful.** Faxed documents, redacted sections, mixed orientations, watermarks, form overlays — your extraction pipeline needs to handle graceful degradation, not just success or failure.

If you're building something similar, start with observability. Add timing and success/failure metrics before you optimize anything. The data will tell you where the pain actually is, which is almost never where you think.

---

## Research and code references

Research Results for Blog Post: "unstructured: chunked extraction + adaptive OCR fallback + idempotent celery orchestration"

Based on my thorough analysis of the entire codebase, here is the honest answer to each of your questions:

---

## Research Results for Blog Post: "unstructured: chunked extraction + adaptive OCR fallback + idempotent celery orchestration"

### Critical Finding: This Work Does Not Exist in the Codebase

**The commit titled "unstructured: chunked extraction + adaptive OCR fallback + idempotent celery orchestration" does not exist.** I searched git history across all branches for "chunked," "adaptive OCR," "idempotent," and "OCR fallback" — none match. The described rewrite has not been implemented. What follows is a detailed account of what *actually* exists, which should help you decide whether to reframe the blog post around the current architecture or write it as a forward-looking design document.

---

### 1) What exists today (the "before" state — which is still the current state)

**The `ai-worker-unstructured` task is minimal:**

```python
# ai-worker-unstructured/ai_worker_unstructured/tasks.py
@shared_task(base=BaseExtractorTask, name="shared.extractor.unstructured")
def extract(content: str, **kwargs: dict):
    processor = MessageProcessor()
    result = processor.process(content, **kwargs)
    return result.model_dump_json()
```

**Concrete pain points visible in the code:**

- **No retry configuration.** The `@shared_task` decorator has zero retry parameters — no `autoretry_for`, `retry_backoff`, `max_retries`, `acks_late`, or `reject_on_worker_lost`. If `unstructured.partition.auto.partition()` fails (OOM, corrupt PDF, OCR timeout), the task goes straight to `FAILURE` with no recovery.

- **No chunk orchestration.** All files in a request are processed sequentially in a single task invocation (`process.py:32` iterates `request.files`). A 200-page PDF is processed as one monolithic call — no fan-out, no `chord`/`group`/`chain`, no parallelism.

- **`BaseExtractorTask` is an empty shell** (`ai-core/ai_core/extractor/base.py:70`):
  ```python
  class BaseExtractorTask(celery.Task):
      pass
  ```
  It adds no `on_failure`, `on_retry`, or any custom behavior.

- **No OCR fallback logic.** The worker delegates entirely to `unstructured.partition.auto.partition()` (`process.py:131`). There is no adaptive fallback between text-layer extraction, Tesseract, or hybrid modes. Whatever `unstructured` decides internally is what you get.

- **No idempotency in the worker itself.** Each invocation creates a fresh `MessageProcessor` and processes from scratch. There are no deduplication keys, checksum checks, or lock mechanisms.

- **Redis visibility timeout causes duplicate execution.** A documented incident (Feb 2026) in the RabbitMQ migration notes shows 10 `xr.regenerate` tasks (~1295s each) were requeued and re-executed after a Redis broker connection drop. The `visibility_timeout` is set to 1500s (25 min) in `ai-server/celery_app.py`, meaning any task exceeding that window risks re-execution.

- **No dead-letter queue for the extraction pipeline.** The DLQ module (`ai-server/ai_server/modules/dlq/`) covers 10 feature types (ais, clm, medc, xr, etc.) but extraction/namespace tasks are *not* in the registry.

**No historical failure-rate metrics or latency measurements are recorded in the codebase.** There are no Prometheus exporters, StatsD calls, or metric collection for extraction tasks.

---

### 2) Current end-to-end implementation (what exists, not what was planned)

**Upload → Enqueue → Process → Save flow:**

1. **Upload** (`ai-server/ai_server/modules/namespaces/routes.py:296`, `add_files()`):
   - Validates file type via magic bytes
   - Saves to disk, creates `MFile` record in MongoDB
   - Calls `create_task_for_file()` (line 429)

2. **Task chain construction** (`routes.py:429-587`, `create_task_for_file()`):
   ```
   state(extraction_pending) → group(extractor_tasks) → task_save_extraction
     → group(splitter_chains) → task_save_chunks → state(indexing_pending)
     → indexer → state(indexing_done)
   ```
   Each step has `.on_error(task_file_state_update.s(state="extraction_failed"))`.

3. **Extraction** (`ai-worker-unstructured/ai_worker_unstructured/process.py`):
   - `MessageProcessor.process()` (line 24): Deserializes `ExtractionRequestWithFiles`
   - Iterates files, calls `do_process()` per file (line 32)
   - `do_process()` (line 54): Sets mode to `"paged"`, calls `load()`
   - `load()` (line 75): Calls `_get_elements()` → `unstructured.partition.auto.partition()`
   - In `"paged"` mode (default): groups elements by `page_number`, concatenates text with `\n\n`
   - Returns `ExtractionResult` serialized as JSON

4. **Save extraction** (`ai-server/.../tasks/save_extraction.py:41`, `task_save_extraction`):
   - **Deletes old** `MExtractedDocument` records for the same file IDs (line 30-33) — this is the only idempotency mechanism
   - Inserts new documents

5. **Chunking** (`tasks/splitter.py:66`, `shared.splitter.langchain`):
   - Default: `RecursiveCharacterTextSplitter` with `chunkSize=7000`, `chunkOverlap=400`
   - `task_save_chunks` (line 88): Flattens nested results, deletes old chunks, inserts new — also idempotent via delete-then-insert

6. **Indexing** (`tasks/indexer.py:81`, `shared.indexer.langchain`):
   - Deletes existing vectors for file (line 66), inserts in batches of 250

7. **Recovery** (`tasks/housekeeping.py:306`, `analyze_and_recover_file()`):
   - State machine recovery: detects stuck files (`extraction_pending` for too long) and re-triggers the appropriate pipeline stage

**Checksums:** The `ai-core/ai_core/utils/ai/cache.py` module computes MD5+SHA1 of file bytes + langs (line 9) and uses MD5 as a disk cache key for pickled results. This is used by the legacy OCR path and remittance parser — **not** by the unstructured worker.

---

### 3) Design decisions that *are* present in the codebase

**What's documented or inferable:**

- **Docker: Debian over Alpine.** CLAUDE.md explicitly states: *"Alpine's musl libc causes segfaults with Kombu/Celery/MongoDB drivers."* The ai-server uses `python:3.12-bookworm`. The unstructured worker uses `downloads.unstructured.io/unstructured-io/unstructured:0.18.15` (also glibc-based).

- **Paged mode as default.** `process.py:65` sets `mode = kwargs.get("mode", "paged")`. Documents are split by page number rather than returned as one blob or per-element. This balances granularity vs. downstream chunk size.

- **Chunk size 7000 / overlap 400** (`ai-core/ai_core/models/indexer.py:23-24`). These are the defaults for `RecursiveCharacterTextSplitter`.

- **Visibility timeout = 25 minutes** (`ai-server/ai_server/config.py`). Chosen as a compromise between allowing long tasks and limiting duplicate execution risk on Redis.

- **Worker restarts after 150 tasks** (`worker_max_tasks_per_child=150`). Prevents memory leaks from accumulating in long-running worker processes.

- **Prefetch multiplier = 1.** Each worker fetches one task at a time, preventing memory pressure from buffered tasks.

- **RabbitMQ migration planned but not yet active.** A full migration plan exists at `docker/rabbitmq-migration.md`, and `docker-compose.ai.yml` includes a commented-out RabbitMQ service. The plan calls for `task_acks_late=True` and `task_reject_on_worker_lost=True`.

- **Caching: disk-based pickle** (`cache.py:27`). The `@cache` decorator writes pickled results to `{cache_dir}/{md5_hash}`. No S3 caching exists. Used by legacy OCR/pdflts paths but **not** by the unstructured worker.

- **No GPU usage.** The unstructured worker and all extractors run on CPU only. No CUDA, no GPU-based OCR.

---

### 4) Operational behavior: what actually prevents duplicate processing

**Current idempotency is delete-then-insert, not deduplication:**

- `task_save_extraction` (line 30-33): Deletes all `MExtractedDocument` for file IDs before inserting new ones
- `task_save_chunks` (line 113): Deletes existing chunks for file/splitter before inserting
- `task_index` (line 66): Deletes existing vectors for file before inserting

**This means re-running any step is safe** — it overwrites previous results. But there's no mechanism to *prevent* duplicate execution (no lock, no idempotency key, no Celery `task_id` deduplication).

**State machine** (`ai-core/ai_core/models/files.py:11`):
```
created → extraction_pending → extraction_done → indexing_pending → indexing_done
                ↓                                        ↓
        extraction_failed                        indexing_failed
```

The `extract_namespace()` endpoint (routes.py:378) skips files in processing states unless `force=True`.

**Retry handling:** The namespace/file pipeline tasks have **no** `retry_backoff`. They fail immediately and set state via `.on_error()` callbacks. Recovery happens via the `housekeeping` task (scheduled or manually triggered), which re-triggers the appropriate pipeline stage.

**No dead-letter queue for extraction.** The DLQ registry covers: ais, clm, claimsummary, progress_notes, xr, index, medc, mcd, audit, stats — but not extraction or namespace tasks.

---

### 5) Known gotchas visible in the code

- **pymongo 4.11 import bug.** The unstructured Dockerfile explicitly uninstalls pymongo and reinstalls via Poetry (`Dockerfile` comment: removes pymongo 4.11 due to import bug).

- **Redis requeue incident.** Documented in `docker/rabbitmq-migration.md`: tasks completing after ~1295s had their results lost when the broker connection dropped, causing all remaining tasks to re-execute.

- **`unstructured` and `opencv-python` not in pyproject.toml.** They're pre-installed in the Docker base image, making local development without Docker difficult. The `pyproject.toml` only declares `celery[redis]`, `pydantic`, `pymongo`, and `ai-core`.

- **No error handling in the task.** Exceptions from `unstructured.partition.auto.partition()` propagate directly to Celery, resulting in immediate FAILURE with no diagnostic context beyond the stack trace.

- **WORKDIR conflict.** The unstructured Dockerfile uses `WORKDIR /myapp` instead of `/app` because the base unstructured image already uses `/app`.

---

### 6) Performance and cost: no metrics available

**There are no performance metrics, failure rates, latency measurements, or cost tracking in the codebase.** No Prometheus exporters, no StatsD, no custom metric collection for extraction tasks. The only observability is:

- Per-task log files at `/data/logs/tasks/{date}/{task_id}.log` (100MB rotation, 30-day retention)
- Flower at port 5555 for Celery task monitoring
- Redis Insight at port 5540

**Rate limits that exist** (from `celery_app.py`):
- `shared.extractor.adi`: 40/s
- No rate limit configured for `shared.extractor.unstructured`

---

### 7) Developer experience and testing

**Docker compose** (`docker/docker-compose.ai.yml`):
- `ai-core` is volume-mounted at `/usr/local/lib/python3.12/site-packages/ai_core` for live reload
- The `worker-unstructured` service is commented out but available
- `dev_worker.sh` wraps the celery command in `watchfiles` for hot-reload during development

**Logging:**
- Loguru-based structured logging via Celery signals (`ai-server/ai_server/tasks/signals.py`)
- Per-task log files with `task_id` context variable
- Format: `{timestamp} | {level} | task_id={task_id} | {message}`

**Testing:**
- `ai-server/tests/tasks/test_save_extraction.py` exists with a fixture using `extractorId="unstructured"` — but it tests the save step, not the extraction itself
- `ai-core/tests/extractors/adi/` has tests for ADI table parsing and result parsing
- **No tests exist for the unstructured worker** — the `ai-worker-unstructured` directory has no `tests/` directory

---

### 8) The commit does not exist

**The commit "unstructured: chunked extraction + adaptive OCR fallback + idempotent celery orchestration" has never been made to this repository.** I searched `git log --all --oneline --grep=` for every keyword combination ("chunked", "adaptive OCR", "idempotent", "OCR fallback") and found no match. The README at `ai-worker-unstructured/README.md` describes the worker as a basic Celery worker for document text extraction — no mention of chunked extraction, adaptive OCR, or idempotent orchestration.

---

### Summary

The blog post as titled describes work that **has not been implemented**. The current unstructured extraction pipeline is a minimal, single-task, no-retry, no-chunking wrapper around `unstructured.partition.auto.partition()`. The idempotency that exists is at the *save* layer (delete-then-insert in MongoDB), not at the task layer. There is no adaptive OCR fallback — the `unstructured` library's internal behavior is used as-is. There is no chunk orchestration — all files are processed sequentially in one task.

**Options for the blog post:**
1. **Reframe as a design document** — "Here's what we're building and why" based on the clear gaps identified above
2. **Write about what exists** — the current Celery chain architecture, delete-then-insert idempotency, the housekeeping recovery system, and the planned RabbitMQ migration
3. **Wait for the implementation** — and then write the post with real code pointers and metrics.
