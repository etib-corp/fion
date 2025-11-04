# Architecture

This document splits the architecture into focused views so each part can be explained independently, then brings them back together with a compact overview.

## Routing layer

The routing layer wires incoming requests to business logic:

- `Application` owns a `Router` and starts the server.
- `Router` stores `Route` entries and resolves a handler for a path + method.
- `Route` binds an HTTP method and path to a `Handler`.
- `Handler` is an interface: handle a `Request` and produce a `Response`.

```mermaid
classDiagram
    %% Routing Layer
    class Application {
        -Router router
        +add_route(path: string, method: string, handler: shared_ptr~Handler~)
        +run(host: string, port: int)
        +get_router() Router
    }

    class Router {
        +vector~Route~ routes
        +add_route(route: Route)
        +find_route(path: string, method: string) shared_ptr~Handler~
    }

    class Route {
        +string path
        +string method
        +shared_ptr~Handler~ handler
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

## HTTP primitives

Core HTTP types used across the system:

- `Method` and `StatusCode` are enums.
- `Version` captures the HTTP major/minor version.
- `URL` stores parsed components of a request target.

```mermaid
classDiagram
    %% HTTP Primitives
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
        OK
        CREATED
        BAD_REQUEST
        NOT_FOUND
        INTERNAL_SERVER_ERROR
    }

    class Version {
        -int _major
        -int _minor
        +Version(major: int, minor: int)
        +getMajor() int
        +getMinor() int
        +toString() string
    }

    class URL {
        -string _scheme
        -string _host
        -int _port
        -string _path
        -string _query
        -string _fragment
        +getScheme() string
        +getHost() string
        +getPort() int
        +getPath() string
        +getQuery() string
        +getFragment() string
    }
```

## HTTP messages

`Request` and `Response` represent the HTTP message model:

- `Request` holds method, URL, version, headers, and body.
- `Response` holds version, status, headers, and body.
- Both reference the primitives above.

```mermaid
classDiagram
    %% HTTP Messages
    class Request {
        -Method _method
        -URL _URL
        -Version _version
        -map~string, string~ _headers
        -string _body
        +Request(rawStartLine: string, rawHeaders: string, rawBody: string)
        +getMethod() Method
        +getURL() URL
        +getVersion() Version
        +getHeader(key: string) string
        +getBody() string
    }

    class Response {
        -Version _version
        -StatusCode _statusCode
        -map~string, string~ _headers
        -string _body
        +setVersion(version: Version)
        +setStatusCode(statusCode: StatusCode)
        +setHeader(key: string, value: string)
        +setBody(body: string)
        +toRawResponse() string
    }

    class Method
    class URL
    class Version
    class StatusCode

    Request --> Method : uses
    Request --> Version : uses
    Request --> URL : uses
    Response --> StatusCode : uses
    Response --> Version : uses
```

## End‑to‑end overview

This compact view shows how everything connects during a typical request lifecycle.

```mermaid
classDiagram
    Application --> Router : contains
    Router --> Route : contains
    Route --> Handler : uses
    Handler --> Response : returns
    Handler --> Request : accepts
    Request --> Method : uses
    Request --> Version : uses
    Request --> URL : uses
    Response --> StatusCode : uses
    Response --> Version : uses
```
