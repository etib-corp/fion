#include <cstdlib>
#include <iostream>
#include <memory>
#include "Application.hpp"
#include "Handler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"

// Example handler implementation
class HelloHandler : public fion::Handler
{
public:
  std::unique_ptr<fion::http::Response> handle(std::unique_ptr<fion::http::Request> request) override
  {
    auto response = std::make_unique<fion::http::Response>();
    response->setStatusCode(fion::http::StatusCode::OK);
    response->setBody("Hello, World!");
    response->setHeader("Content-Type", "text/plain");
    return response;
  }
};

class UserHandler : public fion::Handler
{
public:
  std::unique_ptr<fion::http::Response> handle(std::unique_ptr<fion::http::Request> request) override
  {
    auto response = std::make_unique<fion::http::Response>();
    response->setStatusCode(fion::http::StatusCode::OK);
    response->setBody("User endpoint");
    response->setHeader("Content-Type", "application/json");
    return response;
  }
};

int main(const int argc, const char *const argv[], const char *const envp[])
{
  // Create application instance
  fion::Application app;

  // Create handlers
  auto hello_handler = std::make_shared<HelloHandler>();
  auto user_handler = std::make_shared<UserHandler>();

  // Add routes
  app.add_route("/", "GET", hello_handler);
  app.add_route("/users", "GET", user_handler);

  std::cout << "Routes registered:" << std::endl;
  std::cout << "  GET / -> HelloHandler" << std::endl;
  std::cout << "  GET /users -> UserHandler" << std::endl;

  // Run the application
  app.run("localhost", 8080);

  return EXIT_SUCCESS;
}
