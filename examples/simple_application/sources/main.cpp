#include "Application.hpp"
#include "Handler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>

// Global application pointer for signal handling
fion::Application *g_app = nullptr;

// Signal handler for graceful shutdown
void signal_handler(int signal) {
  if (signal == SIGINT || signal == SIGTERM) {
    std::cout << "\nShutting down server..." << std::endl;
    if (g_app) {
      g_app->stop();
    }
  }
}

// Example handler implementation
class HelloHandler : public fion::Handler {
public:
  std::unique_ptr<fion::http::Response>
  handle(std::unique_ptr<fion::http::Request> request) override {
    auto response = std::make_unique<fion::http::Response>();
    response->setStatusCode(fion::http::StatusCode::OK);
    response->setHeader("Content-Type", "text/plain");
    std::string body = "Hello, World from Fion!";
    response->setHeader("Content-Length", std::to_string(body.size()));
    response->setBody(body);
    return response;
  }
};

class UserHandler : public fion::Handler {
public:
  std::unique_ptr<fion::http::Response>
  handle(std::unique_ptr<fion::http::Request> request) override {
    auto response = std::make_unique<fion::http::Response>();
    response->setStatusCode(fion::http::StatusCode::OK);
    response->setHeader("Content-Type", "application/json");
    std::string body = "{\"users\": [\"Alice\", \"Bob\", \"Charlie\"]}";
    response->setHeader("Content-Length", std::to_string(body.size()));
    response->setBody(body);
    return response;
  }
};

class EchoHandler : public fion::Handler {
public:
  std::unique_ptr<fion::http::Response>
  handle(std::unique_ptr<fion::http::Request> request) override {
    auto response = std::make_unique<fion::http::Response>();
    response->setStatusCode(fion::http::StatusCode::OK);
    response->setHeader("Content-Type", "text/plain");

    // Echo back request information
    std::string body = "Method: ";
    switch (request->getMethod()) {
    case fion::http::Method::GET:
      body += "GET";
      break;
    case fion::http::Method::POST:
      body += "POST";
      break;
    case fion::http::Method::PUT:
      body += "PUT";
      break;
    case fion::http::Method::DELETE:
      body += "DELETE";
      break;
    default:
      body += "UNKNOWN";
      break;
    }
    body += "\nPath: " + request->getURL().getPathToResource();
    body += "\nBody: " + request->getBody();

    response->setHeader("Content-Length", std::to_string(body.size()));
    response->setBody(body);
    return response;
  }
};

int main(const int argc, const char *const argv[], const char *const envp[]) {
  fion::Application app;
  g_app = &app;

  // Setup signal handlers for graceful shutdown
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Create handlers
  auto hello_handler = std::make_shared<HelloHandler>();
  auto user_handler = std::make_shared<UserHandler>();
  auto echo_handler = std::make_shared<EchoHandler>();

  // Example middleware
  auto log_middleware = [](std::unique_ptr<fion::http::Request>& req) {
    std::cout << "[Middleware] Incoming: " << req->getURL().getPathToResource() << std::endl;
  };

  // Add routes with parameters
  app.addRoute("/", "GET", hello_handler, {log_middleware});
  app.addRoute("/users", "GET", user_handler, {log_middleware});
  app.addRoute("/echo", "POST", echo_handler, {log_middleware});

  // Route with parameter: /users/:id
  app.addRoute("/users/:id", "GET", user_handler, {log_middleware});

  // Regex route: /search/(.*)
  app.addRoute("/search/(.*)", "GET", echo_handler, {log_middleware}, true, {"query"});

  // Grouped routes
  std::vector<fion::Route> apiRoutes = {
    fion::Route("/status", "GET", hello_handler),
    fion::Route("/info", "GET", user_handler)
  };
  app.addGroup("/api", apiRoutes, {log_middleware});

  // RESTful resource routing for /items
  app.addResource("items", echo_handler, {log_middleware});

  // Run the application with 4 I/O threads (default)
  app.run("0.0.0.0", 8080);

  return EXIT_SUCCESS;
}
