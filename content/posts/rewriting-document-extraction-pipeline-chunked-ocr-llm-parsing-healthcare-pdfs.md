---
title: "Rewriting the Document Extraction Pipeline: Reliable, Chunked OCR + LLM Parsing for Noisy Healthcare PDFs"
description: "Extraction of messy payer/medical PDFs was causing failures, slowdowns, and nondeterministic outputs; Zeeshan rewrote the extraction worker to combine chunked processing, adaptive OCR fallbacks, and idempotent Celery orchestration—yielding higher throughput and far fewer failed jobs."
date: 2026-03-18
tags: [extraction, ocr, llm, celery, architecture, healthcare]
draft: true
---

Healthcare PDFs are some of the worst documents you'll ever try to extract structured data from. Scanned images with no text layer, multi-column layouts, rotated pages, 200-page payer statements—the variety of ways these files break your pipeline is genuinely impressive. I spent a stretch of time rewriting our document extraction system from scratch, and this post walks through why the old approach was failing, how the new chunked pipeline works, and what I learned along the way.

## Why the old pipeline kept breaking

The original extraction worker took a straightforward approach: accept a PDF, send the whole thing to a remote OCR service, get text back, parse it. Simple in theory. In practice, it fell apart in several compounding ways.

**Timeouts and duplicate work.** Large documents—think ~200-page insurance statements—would exceed the remote OCR service's latency budget. Our Celery visibility timeout was set to 25 minutes, and a single-shot extraction of a big PDF could blow past that. When it did, Celery would redeliver the message to another worker, and now you've got two workers chewing on the same file. Files would get stuck in intermediate states, half-extracted, with no clean way to recover.

**Scanned documents with no text layer.** A huge fraction of healthcare PDFs are just scanned images. The pipeline relied on one primary OCR provider and had no automatic fallback. If that provider returned empty content for a scanned page, extraction simply failed.

**Fragile layout reconstruction.** Even when OCR succeeded, reconstructing readable text from polygon coordinates was brittle. Slight page rotation, multi-column layouts, or mixed orientations would garble the output, and that noise cascaded into every downstream consumer.

**Memory leaks and crashes.** The OCR processing library leaked memory steadily. Workers needed periodic restarts (we configured a max-tasks-per-child setting), and we hit actual segfaults when running on Alpine-based Docker images due to musl libc incompatibilities with C extensions.

The operational reality was: files stuck in limbo, workers restarting, duplicated work, and extraction quality that varied wildly depending on the document.

## The rewrite: chunked, per-page extraction

The core idea of the rewrite was simple: **break every PDF into individual pages and extract each one independently.** One page equals one Celery task. Pages process in parallel, failures are isolated, and no single task runs long enough to hit visibility timeouts.

### Upload and task creation

The flow starts at the API server. When files are uploaded, the handler validates them, saves each file to disk with a UUID-based name, creates a database record, and enqueues a Celery job:

```python
@api.post("/{namespace_name}/files", status_code=201)
async def add_files(namespace, files, settings, sub_namespace):
    for f in files:
        if not is_accepted_file_type(f):
            raise HTTPException(400, detail=f"Invalid file: {f.filename}")

    saved = []
    for f in files:
        unique_file_path, full_path = await save_file(f, settings.data_basedir)
        file_record = DBFileRecord(
            path=str(unique_file_path),
            original_name=f.filename,
            size=full_path.stat().st_size,
            namespace_id=namespace.id,
        )
        await file_record.save()
        saved.append(file_record)

    for file_to_extract in saved:
        task = await create_task_for_file(file_to_extract, namespace, sub_namespace, settings)
        if task:
            task.delay()
    return saved
```

Nothing fancy here—the key decision is deferring all extraction work to background tasks rather than doing anything synchronous in the upload path.

### Splitting into per-page requests

The task orchestration layer inspects each PDF, counts its pages, and creates one extraction request per page:

```python
def create_extraction_requests(extractor, files, data_basedir):
    requests = []
    for f in files:
        ext = Path(f.path).suffix.lower()
        if extractor.paged and extractor.page_counter_fn and ext == ".pdf":
            page_count = load_page_count(data_basedir / f.path)
            for page in range(page_count):
                requests.append(ExtractionRequest(
                    id=uuid4(), extractor_id=extractor.id,
                    files=[f], pages=[page],
                ))
        else:
            requests.append(ExtractionRequest(
                id=uuid4(), extractor_id=extractor.id, files=[f]
            ))
    return requests
```

This is the heart of the chunking strategy. A 200-page PDF becomes 200 independent tasks. Each task only needs to process a single page, so it finishes well within the visibility timeout. If one page fails, the other 199 still succeed—and we can retry just the failed page.

We considered batching pages (say, 10 at a time) instead of going fully per-page, but that approach broke layout reconstruction in practice and increased the blast radius when a batch failed. Per-page turned out to be the right granularity.

### Adaptive OCR fallback

Each per-page task runs a progressive extraction strategy:

1. **Try the embedded text layer first.** Many PDFs do have selectable text. If the text layer is present and has reasonable content, use it directly—no OCR cost incurred.
2. **Fall back to CPU-based OCR.** If the text layer is missing or low-quality, run Tesseract (via an "unstructured" processing library) locally on the worker.
3. **Fall back to a hybrid extraction method.** If local OCR yields poor layout results, try a hybrid approach that combines layout detection with character recognition.

For the remote OCR path, each page gets its own API call:

```python
for page_number, page_bytes in enumerate(split_pdf(filepath, pages)):
    poller = client.begin_analyze_document(model_id, page_bytes, output_format="markdown")
    result = poller.result()
    docs.extend(parse_page_result(result, filepath, page_number + 1))
```

We also added namespace-level extractor selection rules so that known-scanned document patterns can skip the primary remote OCR entirely, avoiding double cost for documents where we know the text layer won't exist.

### Parallel splitters and LLM summarization

After extraction, the per-page documents feed into splitters that run in parallel. There are two flavors:

- **Lightweight text splitters** for vector search indexing—fast, deterministic, running on the main worker queue.
- **LLM-based splitters** for richer structured output—slower, running on a separate queue so they can't starve extraction workers.

The LLM summarizer processes pages in batches, maintaining a running summary across the document:

```python
class LLMSummarySplitter(BaseSplitter):
    async def transform_documents(self, documents):
        pages = sorted(
            [d for d in documents if d.metadata.get("role") == "page"],
            key=lambda x: x.metadata.get("page_number", float('inf'))
        )
        running_summary = ""
        for page_range, batch in self.get_page_batch(pages, batch_size=10):
            messages = [
                {"role": "system", "content": system_prompt},
                {"role": "user", "content": format_pages(batch)},
            ]
            if running_summary:
                messages.insert(1, {"role": "user", "content": running_summary})
            response = await llm_client.chat.create(model=self.model, messages=messages)
            running_summary = response.choices[0].message.content
        return [Document(
            page_content=running_summary,
            metadata={"role": "summary-detailed"}
        )]
```

Keeping LLM work on a separate slow queue was a deliberate choice. These calls can take seconds per batch, and we didn't want them blocking page extraction for new uploads.

### Idempotent saves and self-healing

The save step uses a delete-then-insert pattern: before writing new chunks for a file, delete all existing records for that file, then bulk-insert the new ones. This makes the operation idempotent—if a save is retried (say, after a transient DB failure), you get the same result.

One subtlety: Celery group results arrive in completion order, not page order. The code has to rely on page metadata for correct ordering rather than assuming tasks complete sequentially:

```python
def flatten_chunks(chunks):
    parsed_chunks = []
    def dfs(items):
        for item in items:
            if isinstance(item, list):
                dfs(item)
            else:
                parsed_chunks.append(item)
    dfs(chunks)
    return parsed_chunks
```

A scheduled housekeeping task runs periodically, inspecting database state vs. stored artifacts. If it finds a file stuck in an intermediate state (extraction done but indexing failed, for instance), it re-drives the pipeline from the appropriate step. This replaced the old manual intervention workflow.

## Operational decisions that mattered

**Disk-mounted shared storage over blob storage.** We went with Docker volumes rather than a cloud blob store for file storage. The reason: native PDF libraries expect POSIX file semantics and streaming access. Wrapping them around blob storage APIs added complexity and latency for no real benefit at our scale.

**Debian over Alpine.** After debugging multiple segfaults, we switched all worker images from Alpine to Debian-based. The musl libc in Alpine was incompatible with several C extensions used by our OCR and document processing libraries. This was one of those "costs you a day to figure out, saves you weeks of random crashes" decisions.

**Celery hardening.** Beyond the per-page chunking, we tuned several Celery parameters: adjusted the visibility timeout, enabled socket keepalive to prevent broker disconnections, added rate limiting for remote OCR calls (to stay within provider quotas), and configured periodic worker recycling to mitigate the memory leaks in the processing library.

**Dependency pinning.** We hit a case where a specific version of a database driver caused startup failures. The fix was a temporary version pin, but it reinforced the habit of testing dependency upgrades in isolation before rolling them out.

## Surprises and things that bit us

- **Text layout reconstruction is still fragile.** Even with per-page extraction and adaptive fallbacks, small skews and multi-column layouts still cause garbled output. This is the area with the most remaining fragility—polygon-to-text reconstruction from OCR coordinates is fundamentally brittle.

- **Memory leaks are real.** The "unstructured" processing library leaked memory measurably. We couldn't fix it upstream, so periodic worker restarts became a permanent part of the architecture. Not elegant, but pragmatic.

- **Celery group ordering.** I initially assumed group task results would return in submission order. They don't—they come back in completion order. This caused a subtle bug where page sequences got scrambled. The fix was straightforward (sort by page metadata), but it was the kind of thing that only showed up with documents large enough to have meaningful timing variance between pages.

## What I learned / What's next

The biggest takeaway is that **chunking is the single most impactful change you can make to a document extraction pipeline.** It solves timeout problems, enables parallelism, isolates failures, and makes retry logic trivial. If you're processing documents of variable length with external services, break them into the smallest reasonable unit before sending them out.

The adaptive OCR fallback chain was the second-biggest win. Healthcare documents are unpredictable—some have text layers, some are scans, some are photographed printouts. Having a progressive strategy that tries the cheapest option first and falls back gracefully meant we stopped losing documents to "OCR returned nothing."

Looking ahead, there are clear improvements I want to make:

- **Replace polygon-based layout reconstruction** with a layout-aware model or native layout outputs from the OCR provider. The character-grid approach is too fragile for production.
- **Structured LLM outputs.** Using function-calling or JSON schemas to make LLM parsing more deterministic instead of hoping free-text responses parse cleanly.
- **Per-chunk checksums** to avoid rewriting unchanged chunks on re-extraction. Something like:

```python
import hashlib

def compute_chunk_checksum(content: str, metadata: dict) -> str:
    key_parts = f"{content}:{metadata.get('page_number')}:{metadata.get('role')}"
    return hashlib.sha256(key_parts.encode()).hexdigest()
```

- **End-to-end integration tests** with fixture PDFs covering the failure modes we've seen: scanned, multi-column, rotated, mixed-orientation. Right now we catch these issues in production rather than in CI.
- **Better monitoring and alerting** for stuck files and extraction error rates, so problems surface before users report them.

The pipeline isn't perfect, but it's dramatically more reliable than what it replaced. Healthcare documents will keep finding new ways to break things—the goal is to make the system resilient enough that those breaks are isolated, recoverable, and visible.
