---
title: "When Your Celery Workers Ran Tasks Twice: Migrating from Redis to RabbitMQ"
date: 2026-03-01
tags: ["celery","rabbitmq","redis","reliability","python","devops"]
---

## What happened (the incident)

We had a production incident where long-running AI pricing tasks were requeued and re-executed, doubling downstream LLM API costs and wasting CPU time. The short timeline:

- A batch of long-running tasks (each ~20–25 minutes) completed on workers.
- Immediately after completion the network layer silently dropped the broker TCP connection.
- Because tasks were running with late ACKs, Celery never acknowledged the finished tasks to Redis.
- Redis treated the tasks as unacknowledged and requeued them — workers picked them up again and executed the tasks a second time.

The financial impact was unfortunate and immediate: LLM calls and compute doubled for affected runs. The technical impact exposed two important failure modes when using Redis as a Celery broker for long-running tasks: visibility timeout semantics and idle TCP connection drops.

## Why Redis can fail for long-running tasks

There are two interlocking problems when Redis is used as a Celery broker for long-running tasks:

1. Visibility timeout (Redis transport emulation)
   - Redis implements a visibility/ack emulation with a timer (`visibility_timeout`). If a worker doesn't ACK a task before that timeout the broker assumes the worker died and requeues the message.
   - For long-running tasks you must set `visibility_timeout` higher than the worst-case runtime. Guess wrong and you get duplicates.

2. Silent connection drops / lack of protocol heartbeats
   - Container networking layers or cloud infra (we observed idle TCP connections getting killed after a few minutes) can drop broker connections silently.
   - With Redis this can leave task acknowledgements in-flight or delayed, and the visibility timeout will requeue tasks even if the worker actually finished the work.

Combined: a completed task whose ACK never reached Redis (because the TCP connection was dropped) looks like a worker failure — Redis requeues and the task runs again.

## Short-term mitigations we tried (and why they fell short)

We applied several quick fixes to make Redis more robust:

- Added socket keepalive, connect timeouts, and retry-on-timeout options to the broker transport to keep the TCP connection alive and reduce silent drops.
- Added a periodic health check / ping (health_check_interval) so the transport would more aggressively test the connection.
- Set `broker_connection_retry_on_startup=True` so worker startup is resilient to an unavailable broker.
- As a further stopgap we changed `task_acks_late` to `False` (immediate ACK) to avoid double-execution.

Snippets from the interim Celery configuration (interim fix) look like this:

```python
# ai-server/celery_app.py  (interim hotfix)
celery.conf.update(
    ...
    task_acks_late=True,  # originally True
    broker_connection_retry_on_startup=True,  # NEW
)
celery.conf.broker_transport_options = {
    "visibility_timeout": config.visibility_timeout,
    "socket_keepalive": True,
    "socket_connect_timeout": 30,
    "retry_on_timeout": True,
    "health_check_interval": 30,
    "max_retries": 3,
}
```

Why these were insufficient:

- Keepalives and retries mitigate connection drops but they don't change the visibility timer semantics. If a connection is killed mid-ACK or ACK never reaches Redis, the task can still be requeued.
- Turning off `task_acks_late` (immediate ACK) avoids duplicates but reintroduces another risk: if a worker ACKs and then dies before completing the work, the task is lost. That's acceptable for some idempotent/cheap tasks, but not for tasks where we must ensure work completion or where partial failure is expensive.

We needed a broker that provides protocol-level heartbeats and robust consumer lifecycle semantics — RabbitMQ.

## Why RabbitMQ (and what it gives you)

AMQP (RabbitMQ) gives us three useful properties for long-running tasks:

- Protocol heartbeats: the broker and client keep TCP connections alive at the protocol level. Silent TCP drops are detected quickly.
- Consumer lifecycle: when a consumer connection dies, RabbitMQ immediately cancels consumers and returns unacknowledged messages to the queue — no visibility timer guesswork.
- Explicit acknowledge model: no emulation-driven visibility timeout; the broker knows whether a message was acknowledged or the consumer has disconnected.

In short: RabbitMQ makes duplicate execution due to long-running tasks and idle network timeouts much less likely without having to turn off late ACK semantics.

## The migration we implemented — step by step

We followed a staged, low-risk migration path: bring RabbitMQ into the environment alongside Redis, test with workers pointing to RabbitMQ, then flip traffic once confident.

Key steps (executed in order):

1. Add RabbitMQ to Docker Compose (dev/test) with a basic healthcheck.

```yaml
# docker/docker-compose.ai.yml
rabbitmq:
  image: rabbitmq:4-management
  ports:
    - "5672:5672"
    - "15672:15672"
  environment:
    - RABBITMQ_DEFAULT_USER=admin
    - RABBITMQ_DEFAULT_PASS=admin
  healthcheck:
    test: ["CMD", "rabbitmq-diagnostics", "-q", "ping"]
    interval: 10s
    timeout: 5s
    retries: 5
    start_period: 30s
```

Notes: we kept Redis in place for the result backend and caching — only the Celery *broker* moved.

2. Make minimal Celery config changes. The worker broker URL is read from `WORKER_BROKER_URL` (via a Pydantic settings class), so switching brokers is an env change:

```python
# ai-server/ai_server/config.py
class WorkerConfig(BaseSettings):
    broker_url: Optional[str] = "redis://redis:6379/0"
    result_backend: Optional[str] = "redis://redis:6379/1"
    visibility_timeout: Optional[int] = 25 * 60
    task_acks_late: Optional[bool] = False

# value can be overridden by WORKER_BROKER_URL env var
```

To switch to RabbitMQ you set the broker URL to an AMQP URL, e.g. `amqp://admin:admin@rabbitmq:5672//`.

3. Update worker launch options to reduce broker noise and rely on AMQP heartbeats. We changed the supervisord worker command:

```ini
# ai-server/supervisord.conf
[program:celery]
command=celery -A worker.app worker --loglevel=info -Q %(ENV_CELERY_QUEUE_NAMES)s --autoscale=%(ENV_CELERY_WORKER_AUTOSCALE)s --without-gossip
```

Two small changes: we removed `-E` (events) and added `--without-gossip` — both reduce unnecessary worker-to-worker chatter which is unnecessary with RabbitMQ.

4. Add a simple HTTP health endpoint for the worker container to let Docker and orchestrators detect liveness (used by supervisord's healthcheck). The health check uses Celery's control API to ping the local worker and fetch stats:

```python
# ai-server/celery_health_check.py
@app.get("/health")
async def health_check():
    control: Control = celery_app.control
    inspector: Inspect = control.inspect(timeout=1)
    ping_response = inspector.ping(destination=[f"celery@{hostname}"])
    if not ping_response:
        raise HTTPException(status_code=500, detail={"status": "unhealthy", "details": ...})
    stats = celery_app.control.inspect().stats()
    ...
    return {"status": "healthy", "details": health_data}
```

5. Add RabbitMQ AMQP dependency to the Celery requirements (the `amqp` extras), and ensure containers can resolve `rabbitmq` hostname.

6. Deploy RabbitMQ alongside Redis, point a small fleet of canary workers at RabbitMQ, and run long-running synthetic tasks to exercise the worst-case path.

7. Verify via monitoring — Flower and RabbitMQ Management UI (15672) — task lifecycle, message redeliveries, heartbeats, and worker disconnects.

8. When confident, flip the `WORKER_BROKER_URL` in the environment for remaining workers, and retire Redis as the broker (keeping Redis for results/cache). Rolling restart workers as you flip keeps availability.

Rollback is straightforward: change `WORKER_BROKER_URL` back to the Redis URL and restart workers; result backend and caches remained unchanged.

## Testing and deployment strategy

- Run RabbitMQ in the same network as workers locally (docker compose) and in staging.
- Start with a single worker (or small autoscale window) pointed at RabbitMQ. Run long-running jobs and observe the management UI for requeues/redeliveries.
- Inject network faults (kill the container or block traffic) to ensure RabbitMQ cancels consumers and re-delivers as expected.
- Keep Redis in place for result backend — that reduces the number of moving parts during the cutover.
- Use the health endpoint and Docker healthchecks to ensure orchestrator restarts crashed workers.

A concise checklist for the rollout:

- [ ] Add RabbitMQ service with healthcheck
- [ ] Add `amqp` extras to the Python deps
- [ ] Set WORKER_BROKER_URL to `amqp://...` for canary workers
- [ ] Validate with long-running test tasks
- [ ] Monitor Flower + RabbitMQ management UI for redeliveries and heartbeats
- [ ] Flip remaining workers
- [ ] Observe LLM API and compute billing metrics for anomalies

## Monitoring and operational changes

We added and tracked these signals after the migration:

- Celery worker heartbeats and `inspect().ping()` results (exposed by the health endpoint).
- Broker connection errors and reconnection rates.
- Task redelivery counts and ETA for message requeues in RabbitMQ.
- LLM API call counts and cost (a direct downstream metric for duplicate task execution).
- Flower UI for task events and RabbitMQ Management UI for queue depth and consumer state.

Operationally we also:

- Reduced noisy worker options (`--without-gossip`, no `-E`) to lower broker load.
- Kept a short-runbook for flipping brokers and rolling back.

## Code and configuration examples (before/after)

Interim Redis transport additions (hotfix) to keep connections alive:

```python
# ai-server/celery_app.py (hotfix)
celery.conf.broker_transport_options = {
    "visibility_timeout": config.visibility_timeout,
    "socket_keepalive": True,
    "socket_connect_timeout": 30,
    "retry_on_timeout": True,
    "health_check_interval": 30,
    "max_retries": 3,
}
```

Switching the broker URL is an environment change. The worker settings read from Pydantic `WorkerConfig` so you can flip brokers by changing `WORKER_BROKER_URL` in your environment/compose file:

```python
# ai-server/ai_server/config.py
class WorkerConfig(BaseSettings):
    broker_url: Optional[str] = "redis://redis:6379/0"
    result_backend: Optional[str] = "redis://redis:6379/1"
    # can be overridden by WORKER_BROKER_URL
```

Worker launch changes (supervisord):

```ini
# ai-server/supervisord.conf
[program:celery]
command=celery -A worker.app worker --loglevel=info -Q %(ENV_CELERY_QUEUE_NAMES)s --autoscale=%(ENV_CELERY_WORKER_AUTOSCALE)s --without-gossip
```

Health endpoint used for container healthchecks:

```python
# ai-server/celery_health_check.py
# (simplified)
@app.get("/health")
async def health_check():
    inspector = celery_app.control.inspect(timeout=1)
    ping = inspector.ping(destination=[f"celery@{hostname}"])
    if not ping:
        raise HTTPException(500, "no workers responding")
    stats = inspector.stats()
    ...
    return {"status": "healthy", "details": stats}
```

Docker Compose RabbitMQ service snippet (dev):

```yaml
# docker/docker-compose.ai.yml
rabbitmq:
  image: rabbitmq:4-management
  ports:
    - "5672:5672"
    - "15672:15672"
  environment:
    - RABBITMQ_DEFAULT_USER=admin
    - RABBITMQ_DEFAULT_PASS=admin
  healthcheck:
    test: ["CMD", "rabbitmq-diagnostics", "-q", "ping"]
    interval: 10s
    timeout: 5s
    retries: 5
    start_period: 30s
```

## Trade-offs and things I would call out

- Immediate ACK vs. late ACK. We temporarily set `task_acks_late=False` to stop double runs, but that risks losing work if a worker crashes after ACKing and before finishing. The true long-term fix is a broker that detects dead consumers quickly (RabbitMQ) so you can safely use late ACK semantics where appropriate.

- Persistence and durability. Our dev RabbitMQ used default credentials and no data volume for ease of testing; in production you should enable durable queues, persistent messages (when you need them), and a data volume for RabbitMQ.

- Operational surface area. Introducing RabbitMQ adds another operational component. We kept Redis for result backend and caching to reduce risk during the cutover.

- Downtime and rollout. Rolling restarts and canary workers are a safe way to migrate. There's no need for data migration in our setup because results stayed in Redis.

## What went wrong and surprises

- The biggest surprise was the combination of long-running tasks and cloud/container network behavior. It’s easy to assume that TCP connections are stable — but idle connections are often culled by infra after a few minutes.

- Redis's visibility timeout model works fine for short jobs. It’s brittle for jobs that run longer than your idle-connection thresholds unless you tune timeouts aggressively (which is guesswork).

- Running with late ACKs and not having heartbeat-level detection allowed a race where tasks completed but their ACKs were dropped; Redis then requeued them and we paid for two runs.

## Final recommendations

- For long-running, non-idempotent tasks that call expensive external APIs (e.g., LLMs): prefer RabbitMQ (or another AMQP broker) as Celery broker.
- If you must use Redis, either set visibility timeouts conservatively relative to your maximum task run time, or avoid late ACKs for tasks that call expensive external services (understand the risk of lost tasks).
- Add health endpoints and orchestrator healthchecks that directly exercise Celery control APIs (inspect.ping, stats) so the platform restarts unhealthy worker containers quickly.
- Monitor task redelivery counts, broker connection errors, and downstream API billing as part of reliability SLIs.

## Where this fits with previous work

This post is an update to our earlier architecture writeup about scaling a chat backend with FastAPI, Celery and Redis. The previous post focused on streaming and scaling patterns; this follow-up covers an operational failure mode we hit while operating long-running AI tasks and the concrete migration steps and changes we made to reduce duplicate executions and costs.

## A short checklist for your migration

- Add RabbitMQ service with healthchecks
- Add `amqp` extras to Celery dependencies
- Point a canary worker at RabbitMQ via WORKER_BROKER_URL
- Run long-running test jobs and inject network faults
- Monitor Flower and RabbitMQ Management UI for redeliveries
- Roll remaining workers to RabbitMQ once confident

## Lessons learned (TL;DR)

- Visibility timeouts + silent TCP drops = duplicated work for long tasks on Redis.
- Protocol-level heartbeats and explicit consumer lifecycle (RabbitMQ) eliminate much of the guesswork.
- Incremental migration (bring up RabbitMQ alongside Redis, canary workers, synthetic tests) is low-risk and effective.

—

Author: Zeeshan Khan

If you ran into this and want the diffs/commands I ran, ping me — happy to share the exact compose/env changes I used during the cutover and the minimal scripts for canary testing.
