---
title: "Building a Scalable Chat Backend with LangGraph, FastAPI, Celery, and Redis"
date: 2026-01-28
tags: ["LangGraph", "FastAPI", "Celery", "Redis", "LLM", "streaming", "scalability", "RAG"]
description: "Learn how to evolve a simple LangGraph chat agent into a production-ready, scalable architecture using FastAPI, Celery, and Redis for reliable token streaming."
---

# Introduction

Building a chat backend powered by LLMs seems straightforward at first. You create an API endpoint, invoke your LangGraph agent, and stream the response back to the client. It works beautifully in development.

Then reality hits. Users lose connection mid-response. Load balancers time out long-running requests. Your server restarts, and all in-flight conversations are lost. Scaling horizontally becomes a nightmare because each request is tightly coupled to the process handling it.

In this post, I'll walk you through the evolution from a simple, synchronous approach to a production-ready architecture that:

- **Decouples** the HTTP request lifecycle from the LLM processing
- **Survives** connection drops and allows users to reconnect
- **Scales** horizontally with multiple workers
- **Provides** control mechanisms to stop ongoing generations

We'll build a basic RAG (Retrieval-Augmented Generation) chat agent using LangGraph and progressively enhance the architecture using FastAPI, Celery, and Redis.

---

# Part 1: The Simple Approach

Let's start with what most tutorials show you - a straightforward implementation where the API request directly triggers the LangGraph agent.

## Architecture

```
Client → FastAPI → LangGraph Agent → Stream Response → Client
```

The flow is simple:
1. Client sends a POST request with the user's message
2. FastAPI endpoint invokes the LangGraph agent
3. Agent streams tokens back through the HTTP response
4. Client receives Server-Sent Events (SSE)

## The Code

### LangGraph RAG Agent

First, let's create a simple RAG agent that retrieves context and generates responses:

```python
# agent.py
from typing import TypedDict, Annotated, Sequence
from langchain_openai import ChatOpenAI, OpenAIEmbeddings
from langchain_community.vectorstores import FAISS
from langchain_core.messages import BaseMessage, HumanMessage, AIMessage
from langchain_core.prompts import ChatPromptTemplate
from langgraph.graph import StateGraph, END
import operator


class AgentState(TypedDict):
    messages: Annotated[Sequence[BaseMessage], operator.add]
    context: str


# Initialize components
embeddings = OpenAIEmbeddings()
vectorstore = FAISS.load_local("./vectorstore", embeddings)
retriever = vectorstore.as_retriever(search_kwargs={"k": 3})
llm = ChatOpenAI(model="gpt-4o-mini", streaming=True)

prompt = ChatPromptTemplate.from_messages([
    ("system", "You are a helpful assistant. Use the following context to answer questions:\n\n{context}"),
    ("placeholder", "{messages}"),
])


def retrieve(state: AgentState) -> AgentState:
    """Retrieve relevant documents based on the last message."""
    last_message = state["messages"][-1]
    docs = retriever.invoke(last_message.content)
    context = "\n\n".join(doc.page_content for doc in docs)
    return {"context": context}


def generate(state: AgentState) -> AgentState:
    """Generate response using the LLM."""
    chain = prompt | llm
    response = chain.invoke({
        "context": state["context"],
        "messages": state["messages"]
    })
    return {"messages": [response]}


# Build the graph
workflow = StateGraph(AgentState)
workflow.add_node("retrieve", retrieve)
workflow.add_node("generate", generate)
workflow.set_entry_point("retrieve")
workflow.add_edge("retrieve", "generate")
workflow.add_edge("generate", END)

agent = workflow.compile()
```

### FastAPI with SSE Streaming

Now the FastAPI endpoint that streams responses:

```python
# main.py
from fastapi import FastAPI
from fastapi.responses import StreamingResponse
from pydantic import BaseModel
from agent import agent, AgentState
from langchain_core.messages import HumanMessage
import json

app = FastAPI()


class ChatRequest(BaseModel):
    message: str
    conversation_id: str | None = None


async def stream_agent_response(message: str):
    """Stream tokens from the agent."""
    initial_state: AgentState = {
        "messages": [HumanMessage(content=message)],
        "context": ""
    }
    
    async for event in agent.astream_events(initial_state, version="v2"):
        if event["event"] == "on_chat_model_stream":
            chunk = event["data"]["chunk"]
            if chunk.content:
                yield f"data: {json.dumps({'token': chunk.content})}\n\n"
    
    yield "data: [DONE]\n\n"


@app.post("/chat")
async def chat(request: ChatRequest):
    return StreamingResponse(
        stream_agent_response(request.message),
        media_type="text/event-stream"
    )
```

## The Problem

This approach works fine for simple cases, but falls apart at scale:

| Issue | Impact |
|-------|--------|
| **HTTP Timeouts** | Load balancers (nginx, AWS ALB) typically timeout after 30-60 seconds. Long LLM responses get cut off. |
| **Connection Drops** | Mobile users, flaky networks - when the connection drops, the entire response is lost. No way to resume. |
| **No Reconnection** | If the client disconnects and reconnects, they can't resume the stream. They have to start over. |
| **Process Coupling** | The request is tied to the specific process/thread. If that process crashes, the request is gone. |
| **Scaling Limits** | Each streaming connection holds a worker. With sync workers, you're limited by worker count. |

We need to decouple the LLM processing from the HTTP request lifecycle.

---

# Part 2: The Scalable Architecture

The key insight is simple: **separate the work from the delivery**.

Instead of running the LangGraph agent inside the HTTP request, we:
1. Dispatch the work to a background task (Celery)
2. Stream results through a message broker (Redis)
3. Let the API server relay those results to the client

## Architecture Overview

```
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌───────────┐
│  Client │────▶│ FastAPI │────▶│  Redis  │◀────│  Celery   │
│         │◀────│   API   │◀────│ Pub/Sub │     │  Worker   │
└─────────┘     └─────────┘     └─────────┘     └───────────┘
                                                      │
                                                      ▼
                                               ┌───────────┐
                                               │ LangGraph │
                                               │   Agent   │
                                               └───────────┘
```

## The Flow

1. **Start Task**: Client sends `POST /chat` → FastAPI creates a Celery task → Returns `task_id` immediately
2. **Process**: Celery worker runs LangGraph agent → Publishes each token to Redis channel
3. **Stream**: Client calls `GET /chat/{task_id}/stream` → FastAPI subscribes to Redis → Relays tokens via SSE
4. **Stop**: Client can call `POST /chat/{task_id}/stop` → Sets a flag in Redis → Worker checks and stops gracefully

## Why This Works

| Benefit | Explanation |
|---------|-------------|
| **Decoupled Lifecycle** | HTTP connection can drop and reconnect. The Celery task continues unaffected. |
| **Resumable Streams** | Client reconnects, subscribes to the same Redis channel, continues from where they left off. |
| **Horizontal Scaling** | Add more Celery workers to handle more concurrent tasks. API servers are stateless. |
| **Graceful Shutdown** | Stop signal through Redis allows clean termination without killing processes. |
| **Fault Tolerance** | If a worker crashes, Celery can retry the task on another worker. |

---

# Part 3: Implementation

Let's build this architecture piece by piece.

## Project Structure

```
chat-backend/
├── app/
│   ├── __init__.py
│   ├── main.py          # FastAPI application
│   ├── agent.py         # LangGraph agent
│   ├── tasks.py         # Celery tasks
│   ├── redis_client.py  # Redis utilities
│   └── config.py        # Configuration
├── docker-compose.yml
├── requirements.txt
└── Dockerfile
```

## Configuration

```python
# app/config.py
from pydantic_settings import BaseSettings


class Settings(BaseSettings):
    redis_url: str = "redis://localhost:6379/0"
    celery_broker_url: str = "redis://localhost:6379/1"
    openai_api_key: str
    
    class Config:
        env_file = ".env"


settings = Settings()
```

## Redis Client

```python
# app/redis_client.py
import redis.asyncio as aioredis
import redis
from app.config import settings

# Async client for FastAPI
async_redis = aioredis.from_url(settings.redis_url)

# Sync client for Celery workers
sync_redis = redis.from_url(settings.redis_url)


def get_channel_name(task_id: str) -> str:
    """Get the Redis channel name for a task."""
    return f"chat:{task_id}"


def get_stop_key(task_id: str) -> str:
    """Get the Redis key for stop signal."""
    return f"chat:{task_id}:stop"


def get_state_key(task_id: str) -> str:
    """Get the Redis key for task state."""
    return f"chat:{task_id}:state"
```

## LangGraph Agent with Streaming Callback

```python
# app/agent.py
from typing import TypedDict, Annotated, Sequence, Callable
from langchain_openai import ChatOpenAI, OpenAIEmbeddings
from langchain_community.vectorstores import FAISS
from langchain_core.messages import BaseMessage, HumanMessage
from langchain_core.prompts import ChatPromptTemplate
from langchain_core.callbacks import BaseCallbackHandler
from langgraph.graph import StateGraph, END
import operator


class AgentState(TypedDict):
    messages: Annotated[Sequence[BaseMessage], operator.add]
    context: str


class StreamingCallbackHandler(BaseCallbackHandler):
    """Callback handler that publishes tokens to a callback function."""
    
    def __init__(self, on_token: Callable[[str], None], should_stop: Callable[[], bool]):
        self.on_token = on_token
        self.should_stop = should_stop
        self._stopped = False
    
    def on_llm_new_token(self, token: str, **kwargs) -> None:
        if self._stopped:
            raise StopIteration("Generation stopped by user")
        if self.should_stop():
            self._stopped = True
            raise StopIteration("Generation stopped by user")
        self.on_token(token)


def create_agent(on_token: Callable[[str], None], should_stop: Callable[[], bool]):
    """Create a LangGraph agent with streaming callbacks."""
    
    embeddings = OpenAIEmbeddings()
    # In production, load from persistent storage
    vectorstore = FAISS.load_local("./vectorstore", embeddings, allow_dangerous_deserialization=True)
    retriever = vectorstore.as_retriever(search_kwargs={"k": 3})
    
    callback_handler = StreamingCallbackHandler(on_token, should_stop)
    llm = ChatOpenAI(
        model="gpt-4o-mini",
        streaming=True,
        callbacks=[callback_handler]
    )
    
    prompt = ChatPromptTemplate.from_messages([
        ("system", "You are a helpful assistant. Use the following context to answer:\n\n{context}"),
        ("placeholder", "{messages}"),
    ])
    
    def retrieve(state: AgentState) -> AgentState:
        last_message = state["messages"][-1]
        docs = retriever.invoke(last_message.content)
        context = "\n\n".join(doc.page_content for doc in docs)
        return {"context": context}
    
    def generate(state: AgentState) -> AgentState:
        chain = prompt | llm
        response = chain.invoke({
            "context": state["context"],
            "messages": state["messages"]
        })
        return {"messages": [response]}
    
    workflow = StateGraph(AgentState)
    workflow.add_node("retrieve", retrieve)
    workflow.add_node("generate", generate)
    workflow.set_entry_point("retrieve")
    workflow.add_edge("retrieve", "generate")
    workflow.add_edge("generate", END)
    
    return workflow.compile()
```

## Celery Tasks

```python
# app/tasks.py
from celery import Celery
from app.config import settings
from app.redis_client import sync_redis, get_channel_name, get_stop_key, get_state_key
from app.agent import create_agent, AgentState
from langchain_core.messages import HumanMessage
import json

celery_app = Celery(
    "chat_tasks",
    broker=settings.celery_broker_url,
)

celery_app.conf.update(
    task_serializer="json",
    result_serializer="json",
    accept_content=["json"],
    task_track_started=True,
)


@celery_app.task(bind=True)
def run_chat_agent(self, message: str, conversation_history: list[dict] | None = None):
    """Run the LangGraph agent and stream tokens to Redis."""
    task_id = self.request.id
    channel = get_channel_name(task_id)
    stop_key = get_stop_key(task_id)
    state_key = get_state_key(task_id)
    
    # Set initial state
    sync_redis.set(state_key, "running")
    
    def publish_token(token: str):
        """Publish a token to Redis."""
        sync_redis.publish(channel, json.dumps({"type": "token", "data": token}))
    
    def should_stop() -> bool:
        """Check if we should stop generation."""
        return sync_redis.exists(stop_key)
    
    try:
        # Create agent with callbacks
        agent = create_agent(publish_token, should_stop)
        
        # Build messages from history
        messages = []
        if conversation_history:
            for msg in conversation_history:
                if msg["role"] == "user":
                    messages.append(HumanMessage(content=msg["content"]))
        messages.append(HumanMessage(content=message))
        
        # Run the agent
        initial_state: AgentState = {
            "messages": messages,
            "context": ""
        }
        
        result = agent.invoke(initial_state)
        
        # Publish completion
        final_message = result["messages"][-1].content
        sync_redis.publish(channel, json.dumps({
            "type": "done",
            "data": final_message
        }))
        sync_redis.set(state_key, "completed")
        
    except StopIteration:
        sync_redis.publish(channel, json.dumps({"type": "stopped"}))
        sync_redis.set(state_key, "stopped")
        
    except Exception as e:
        sync_redis.publish(channel, json.dumps({"type": "error", "data": str(e)}))
        sync_redis.set(state_key, "error")
        raise
    
    finally:
        # Set expiry on keys for cleanup
        sync_redis.expire(state_key, 3600)  # 1 hour
        sync_redis.delete(stop_key)
    
    return {"status": "completed", "task_id": task_id}
```

## FastAPI Application

```python
# app/main.py
from fastapi import FastAPI, HTTPException
from fastapi.responses import StreamingResponse
from pydantic import BaseModel
from app.tasks import run_chat_agent, celery_app
from app.redis_client import async_redis, get_channel_name, get_stop_key, get_state_key
import json
import asyncio

app = FastAPI(title="Scalable Chat Backend")


class ChatRequest(BaseModel):
    message: str
    conversation_history: list[dict] | None = None


class ChatResponse(BaseModel):
    task_id: str
    status: str


class TaskStatus(BaseModel):
    task_id: str
    status: str


@app.post("/chat", response_model=ChatResponse)
async def start_chat(request: ChatRequest):
    """Start a chat task and return the task ID."""
    task = run_chat_agent.delay(
        message=request.message,
        conversation_history=request.conversation_history
    )
    return ChatResponse(task_id=task.id, status="started")


@app.get("/chat/{task_id}/stream")
async def stream_chat(task_id: str):
    """Stream chat responses via SSE."""
    
    # Check if task exists
    state_key = get_state_key(task_id)
    state = await async_redis.get(state_key)
    
    if state is None:
        # Task might not have started yet, wait a bit
        await asyncio.sleep(0.5)
        state = await async_redis.get(state_key)
        if state is None:
            raise HTTPException(status_code=404, detail="Task not found")
    
    async def event_generator():
        channel = get_channel_name(task_id)
        pubsub = async_redis.pubsub()
        await pubsub.subscribe(channel)
        
        try:
            async for message in pubsub.listen():
                if message["type"] == "message":
                    data = json.loads(message["data"])
                    yield f"data: {json.dumps(data)}\n\n"
                    
                    # Stop streaming on terminal events
                    if data.get("type") in ("done", "stopped", "error"):
                        break
        finally:
            await pubsub.unsubscribe(channel)
            await pubsub.close()
    
    return StreamingResponse(
        event_generator(),
        media_type="text/event-stream",
        headers={
            "Cache-Control": "no-cache",
            "Connection": "keep-alive",
        }
    )


@app.post("/chat/{task_id}/stop", response_model=TaskStatus)
async def stop_chat(task_id: str):
    """Stop an ongoing chat task."""
    state_key = get_state_key(task_id)
    state = await async_redis.get(state_key)
    
    if state is None:
        raise HTTPException(status_code=404, detail="Task not found")
    
    if state.decode() != "running":
        return TaskStatus(task_id=task_id, status=state.decode())
    
    # Set stop signal
    stop_key = get_stop_key(task_id)
    await async_redis.set(stop_key, "1", ex=300)  # 5 min expiry
    
    return TaskStatus(task_id=task_id, status="stopping")


@app.get("/chat/{task_id}/status", response_model=TaskStatus)
async def get_chat_status(task_id: str):
    """Get the status of a chat task."""
    state_key = get_state_key(task_id)
    state = await async_redis.get(state_key)
    
    if state is None:
        # Check Celery task state
        result = celery_app.AsyncResult(task_id)
        return TaskStatus(task_id=task_id, status=result.status.lower())
    
    return TaskStatus(task_id=task_id, status=state.decode())
```

## Docker Compose

```yaml
# docker-compose.yml
version: '3.8'

services:
  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"
    volumes:
      - redis_data:/data
    command: redis-server --appendonly yes

  api:
    build: .
    ports:
      - "8000:8000"
    environment:
      - REDIS_URL=redis://redis:6379/0
      - CELERY_BROKER_URL=redis://redis:6379/1
      - OPENAI_API_KEY=${OPENAI_API_KEY}
    depends_on:
      - redis
    command: uvicorn app.main:app --host 0.0.0.0 --port 8000

  worker:
    build: .
    environment:
      - REDIS_URL=redis://redis:6379/0
      - CELERY_BROKER_URL=redis://redis:6379/1
      - OPENAI_API_KEY=${OPENAI_API_KEY}
    depends_on:
      - redis
    command: celery -A app.tasks worker --loglevel=info --concurrency=4
    deploy:
      replicas: 2  # Run multiple workers for scaling

volumes:
  redis_data:
```

## Requirements

```text
# requirements.txt
fastapi>=0.109.0
uvicorn[standard]>=0.27.0
celery>=5.3.0
redis>=5.0.0
langchain>=0.1.0
langchain-openai>=0.0.5
langchain-community>=0.0.20
langgraph>=0.0.20
faiss-cpu>=1.7.4
pydantic-settings>=2.1.0
```

## Client Usage Example

```python
# client_example.py
import requests
import sseclient

BASE_URL = "http://localhost:8000"

# Start a chat
response = requests.post(f"{BASE_URL}/chat", json={
    "message": "What is the capital of France?",
    "conversation_history": []
})
task_id = response.json()["task_id"]
print(f"Started task: {task_id}")

# Stream the response
response = requests.get(
    f"{BASE_URL}/chat/{task_id}/stream",
    stream=True
)
client = sseclient.SSEClient(response)

full_response = ""
for event in client.events():
    data = json.loads(event.data)
    if data["type"] == "token":
        print(data["data"], end="", flush=True)
        full_response += data["data"]
    elif data["type"] == "done":
        print("\n--- Done ---")
        break
    elif data["type"] == "stopped":
        print("\n--- Stopped by user ---")
        break
    elif data["type"] == "error":
        print(f"\n--- Error: {data['data']} ---")
        break
```

---

# Part 4: Benefits and Trade-offs

## Benefits

1. **Resilience**: HTTP connections can drop and reconnect without losing progress. The background task continues independently.

2. **Scalability**: Add more Celery workers to handle increased load. The API servers remain stateless and lightweight.

3. **User Control**: Users can stop ongoing generations without waiting for completion.

4. **Observability**: Task states are stored in Redis, making it easy to monitor and debug.

5. **Timeout Immunity**: Load balancer timeouts don't affect the LLM processing. Only the streaming connection needs to stay alive.

## Trade-offs

1. **Complexity**: More moving parts (Redis, Celery) mean more infrastructure to manage.

2. **Latency**: There's a small overhead for task dispatch and pub/sub. For very short responses, the simple approach might be faster.

3. **Cost**: Running Redis and Celery workers adds infrastructure cost.

4. **Message Ordering**: In rare cases with network issues, Redis pub/sub messages could arrive out of order. Consider using Redis Streams for guaranteed ordering.

## When to Use Which Approach

| Scenario | Recommended Approach |
|----------|---------------------|
| Prototype / MVP | Simple (direct streaming) |
| Low traffic, reliable network | Simple |
| Production with scale requirements | Scalable (Redis + Celery) |
| Mobile clients, unreliable networks | Scalable |
| Long-running generations (> 30s) | Scalable |

## Next Steps

To further enhance this architecture, consider:

- **Redis Streams**: Replace pub/sub with Streams for message persistence and replay capability
- **Task Queues**: Priority queues for premium users
- **Rate Limiting**: Implement per-user rate limits at the API layer
- **Caching**: Cache frequent queries to reduce LLM costs
- **Monitoring**: Add Prometheus metrics for task durations, queue depths, and error rates

---

# Conclusion

Building a scalable chat backend requires separating the LLM processing from the HTTP request lifecycle. By introducing Redis as a message broker and Celery for background task processing, we create an architecture that:

- Survives connection drops
- Scales horizontally
- Gives users control over their requests
- Handles long-running generations gracefully

The added complexity is worth it for any production system that needs to handle real users with real network conditions. Start with the simple approach for prototyping, but plan to evolve to the scalable architecture as you grow.
