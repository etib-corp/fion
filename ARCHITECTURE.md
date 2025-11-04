# Architecture

This document splits the architecture into focused views so each part can be explained independently, then brings them back together with a compact overview.

---

## Application (Top-Level)

The `Application` class is the entry point and orchestrates the entire framework.

```mermaid
classDiagram
    class Application {
        -_server: Server
        -_router: Router
        +addRoute(path: string, method: string, handler: shared_ptr~Handler~)
        +run(host: string, port: int, numThreads: int)
        +stop()
        +getRouter() Router
    }

    class Server {
        -_listener: Listener
        -_poolManager: PoolManager
        +start(numThreads: int)
        +stop()
    }

    Application --> Server : contains
    Application --> Router : contains
```

**Purpose:**

- **Application**: The top-level class that owns the `Server` and `Router`. It initializes the server, manages routes, and starts/stops the framework.
- **Server**: Manages the listener, pool manager, and I/O threads.

---

## Server and Listener

The server and listener handle connection acceptance and I/O orchestration.

```mermaid
classDiagram
    class Server {
        -_listener: Listener
        -_poolManager: PoolManager
        +start(numThreads: int)
        +stop()
    }

    class Listener {
        -_listenFD: int
        -_address: sockaddr_in
        +acceptClient() int
    }

    Server --> Listener : contains
    Server --> PoolManager : contains
```

**Purpose:**

- **Server**: Orchestrates the listener and pool manager.
- **Listener**: Binds to a port and accepts new client connections.

---

## Pool Management

Distributes and manages client connections across multiple I/O threads.

```mermaid
classDiagram
    class PoolManager {
        -_pools: vector~Pool~
        -_mutex: mutex
        -_nextPoolIndex: int
        +add_pool(pool: Pool)
        +get_pool() Pool*
        +distribute_client(fd: int)
    }

    class Pool {
        -_loop: EventLoop
        -_connectionPool: ConnectionPool
        -_thread: thread
        +run()
        +stop()
        +addClient(fd: int)
    }

    PoolManager --> Pool : contains (1..N)
```

**Purpose:**

- **PoolManager**: Coordinates multiple `Pool` instances, distributing new client connections (e.g., round-robin).
- **Pool**: Encapsulates an `EventLoop`, `ConnectionPool`, and a thread. Each pool runs independently, handling I/O for its assigned clients.

---

## I/O and Event Loop

Monitors and processes I/O events for clients.

```mermaid
classDiagram
    class EventLoop {
        -_poller: Poller
        -_running: bool
        +run()
        +stop()
    }

    class Poller {
        -_epoll_fd: int
        +addFD(fd: int, events: uint32_t)
        +removeFD(fd: int)
        +poll(timeout_ms: int) vector~epoll_event~
    }

    Pool --> EventLoop : contains
    EventLoop --> Poller : contains
```

**Purpose:**

- **EventLoop**: Runs in each pool’s thread, processing I/O events.
- **Poller**: Uses `poll` to monitor sockets for read/write events.

---

## Connection Management

Manages active client connections and their data buffers.

```mermaid
classDiagram
    class ConnectionPool {
        -_clients: unordered_map~int, unique_ptr~Client~~
        -_mutex: mutex
        +addClient(fd: int)
        +removeClient(fd: int)
        +getClient(fd: int) Client*
    }

    class Client {
        -_fd: int
        -_requestBuffer: vector~char~
        -_responseBuffer: vector~char~
        -_state: State
        -_mutex: mutex
        +readRequest()
        +writeResponse(res: Response)
        +isReady() bool
    }

    class Buffer {
        -_data: vector~char~
        -_mutex: mutex
        +append(data: char*, len: size_t)
        +clear()
        +getData() string_view
    }

    Pool --> ConnectionPool : contains
    ConnectionPool --> Client : manages
    Client --> Buffer : uses
```

**Purpose:**

- **ConnectionPool**: Thread-safe container for active clients.
- **Client**: Represents a single client connection, with buffers for request/response data.
- **Buffer**: Thread-safe storage for raw request/response bytes.

---

## Routing Layer

Wires incoming requests to business logic.

```mermaid
classDiagram
    class Router {
        -_routes: vector~Route~
        -_mutex: mutex
        +addRoute(route: Route)
        +findRoute(path: string, method: string) shared_ptr~Handler~
    }

    class Route {
        -_path: string
        -_method: string
        -_handler: shared_ptr~Handler~
        +Route(path: string, method: string, handler: shared_ptr~Handler~)
    }

    class Handler {
        <<interface>>
        +virtual unique_ptr~Response~ handle(request: unique_ptr~Request~)* = 0
    }

    Application --> Router : contains
    Router --> Route : contains
    Route --> Handler : uses
```

**Purpose:**

- **Router**: Stores routes and resolves handlers for incoming requests.
- **Route**: Binds a path, method, and handler.
- **Handler**: Interface for request processing.

---

## HTTP Primitives

Core HTTP types used across the system.

```mermaid
classDiagram
    class Method {
        <<enumeration>>
        GET
        HEAD
        POST
        PUT
        DELETE
        CONNECT
        OPTIONS
        TRACE
        PATCH
    }
    class StatusCode {
        <<enumeration>>
        CONTINUE = 100
        OK = 200
        CREATED = 201
        MOVED_PERMANENTLY = 301
        BAD_REQUEST = 400
        UNAUTHORIZED = 401
        FORBIDDEN = 403
        NOT_FOUND = 404
        INTERNAL_SERVER_ERROR = 500
        BAD_GATEWAY = 502
        SERVICE_UNAVAILABLE = 503
        ... and more
    }
    class Version {
        <<enumeration>>
        HTTP_0_9
        HTTP_1_0
        HTTP_1_1
        HTTP_2_0
        HTTP_3_0
    }
    class URL {
        -_scheme: string
        -_domainName: string
        -_port: uint16_t
        -_pathToResource: string
        -_queryParameters: map~string, string~
        -_anchor: string
        +URL(rawURL: string)
        +getScheme() string
        +getDomainName() string
        +getPort() uint16_t
        +getPathToResource() string
        +getQueryParameters() map~string, string~
        +getAnchor() string
        +toString() string
    }
    class Headers {
        -_headers: map~string, string~
        +set(key: string, value: string)
        +get(key: string) string
        +get(key: string, defaultValue: string) string
        +getOptional(key: string) optional~string~
        +has(key: string) bool
        +remove(key: string) bool
        +clear()
        +getKeys() vector~string~
        +size() size_t
        +empty() bool
        +getAll() map~string, string~
        +parseFromRaw(rawHeaders: string)
        +toRawString() string
    }
```

**Purpose:**

- **Method/StatusCode/Version/URL/Headers**: Core HTTP types and utilities.

---

## HTTP Messages

Models HTTP requests and responses.

```mermaid
classDiagram
    class Request {
        -_method: Method
        -_URL: URL
        -_version: Version
        -_headers: Headers
        -_body: string
        -parseStartLine(rawStartLine: string)
        -parseHeaders(rawHeaders: string)
        -parseBody(rawBody: string)
        +Request(rawStartLine: string, rawHeaders: string, rawBody: string)
        +getMethod() Method
        +getURL() URL
        +getVersion() Version
        +getHeaders() Headers
        +getHeader(key: string) string
        +getBody() string
    }
    class Response {
        -_version: Version
        -_statusCode: StatusCode
        -_headers: Headers
        -_body: string
        +setVersion(version: Version)
        +setStatusCode(statusCode: StatusCode)
        +setHeader(key: string, value: string)
        +setBody(body: string)
        +getHeaders() Headers
        +toRawResponse() string
    }
    Request --> Method : uses
    Request --> Version : uses
    Request --> URL : uses
    Request --> Headers : uses
    Response --> StatusCode : uses
    Response --> Version : uses
    Response --> Headers : uses
```

**Purpose:**

- **Request/Response**: Encapsulate HTTP messages with headers, body, and metadata.

---

## Thread Pool (Optional)

Offloads CPU-bound handler execution.

```mermaid
classDiagram
    class ThreadPool {
        -_threads: vector~thread~
        -_tasks: queue~function~void()~
        -_mutex: mutex
        -_cv: condition_variable
        +enqueue(task: function~void()~)
        +worker_loop()
    }

    PoolManager --> ThreadPool : contains (optional)
    ThreadPool --> Handler : executes
```

**Purpose:**

- **ThreadPool**: Executes handlers in worker threads to avoid blocking I/O threads.

---

## End-to-End Overview

This compact view shows how everything connects during a typical request lifecycle.

```mermaid
classDiagram
    Application --> Server : contains
    Application --> Router : contains
    Server --> Listener : contains
    Server --> PoolManager : contains
    PoolManager --> Pool : contains (1..N)
    Pool --> EventLoop : contains
    Pool --> ConnectionPool : contains
    PoolManager --> ThreadPool : contains (optional)
    EventLoop --> Poller : contains
    ConnectionPool --> Client : manages
    Client --> Buffer : uses
    Client --> Request : creates
    Client --> Response : sends
    Router --> Route : contains
    Route --> Handler : uses
    Handler --> Response : returns
    Handler --> Request : accepts
    Request --> Method : uses
    Request --> Version : uses
    Request --> URL : uses
    Request --> Headers : uses
    Response --> StatusCode : uses
    Response --> Version : uses
    Response --> Headers : uses
    ThreadPool --> Handler : executes
```

**Workflow:**

1. **Application** starts the **Server** and **Router**.
2. **Listener** accepts a connection → **PoolManager** distributes it to a **Pool**.
3. **Pool**’s **EventLoop** monitors the client socket.
4. **Client** reads the request → **Router** resolves the **Handler**.
5. **Handler** processes the request (optionally in **ThreadPool**) → **Client** writes the response.
