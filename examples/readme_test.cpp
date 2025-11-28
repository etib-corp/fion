#include "Application.hpp"
#include "Handler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include <iostream>
#include <memory>

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

int main() {
    fion::Application app;
    auto hello_handler = std::make_shared<HelloHandler>();
    auto log_middleware = [](std::unique_ptr<fion::http::Request>& req) {
        std::cout << "[Middleware] " << req->getURL().getPathToResource() << std::endl;
    };

    // Static route
    app.addRoute("/", "GET", hello_handler, {log_middleware});
    // Parameterized route
    app.addRoute("/users/:id", "GET", hello_handler, {log_middleware});
    // Regex route
    app.addRoute("/search/(.*)", "GET", hello_handler, {log_middleware}, true, {"query"});
    // Grouped routes
    std::vector<fion::Route> apiRoutes = {
        fion::Route("/status", "GET", hello_handler),
        fion::Route("/info", "GET", hello_handler)
    };
    app.addGroup("/api", apiRoutes, {log_middleware});
    // RESTful resource
    app.addResource("items", hello_handler, {log_middleware});
    app.run("0.0.0.0", 8080);
    return 0;
}
