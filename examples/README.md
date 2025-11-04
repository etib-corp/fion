# Fion Examples

This directory contains example applications demonstrating how to use the Fion framework. Each example is organized in its own subdirectory with its own CMakeLists.txt and README.

## Available Examples

### Simple Application

A minimal HTTP server example that demonstrates:

- Creating a Fion application
- Implementing custom request handlers
- Registering routes with different HTTP methods
- Running a multi-threaded server
- Graceful shutdown handling

See [simple_application/README.md](simple_application/README.md) for more details.

## Building Examples

From the Fion project root directory:

```bash
mkdir build && cd build
cmake ..
make
```

All examples will be built automatically unless you disable them with:

```bash
cmake -DBUILD_EXAMPLES=OFF ..
```

## Running Examples

After building, you can run any example from the build directory:

```bash
# Run the simple application example
./examples/simple_application/simple_application
```

## Creating Your Own Example

To create a new example:

1. Create a new directory under `examples/`
2. Add a `CMakeLists.txt` file (see `simple_application/CMakeLists.txt` as reference)
3. Create a `sources/` directory with your main.cpp
4. Add a README.md explaining what your example demonstrates

The build system will automatically detect and build new examples.
