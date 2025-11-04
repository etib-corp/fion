# Simple Application

A minimal Fion example demonstrating how to create a basic HTTP server with multiple routes and handlers.

## What it demonstrates

- Creating a Fion application instance
- Implementing custom HTTP request handlers
- Registering routes with different HTTP methods
- Running a multi-threaded server
- Graceful shutdown with signal handling
- Working with HTTP requests and responses

## Features

This example includes:
- **HelloHandler**: Returns a simple "Hello, World" message at `/`
- **UserHandler**: Returns a JSON list of users at `/users`
- **EchoHandler**: Echoes back request information at `/echo` (POST)

## Building

From the Fion project root directory:

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./examples/simple_application/simple_application
```

The server will start on `http://0.0.0.0:8080` with 4 I/O threads by default.

## Testing

Once the server is running, you can test it with curl:

```bash
# Test the hello endpoint
curl http://localhost:8080/

# Test the users endpoint
curl http://localhost:8080/users

# Test the echo endpoint
curl -X POST -d "Hello from client" http://localhost:8080/echo
```

## Customization

You can customize the number of I/O threads by modifying the `run()` call:

```cpp
// Run with 8 I/O threads instead of the default 4
app.run("0.0.0.0", 8080, 8);
```

## Stopping the Server

Press `Ctrl+C` to gracefully stop the server.
