#include <chrono>
#include <csignal>
#include <thread>


#include "Application.hpp"
#include "logging/Logger.hpp"
#include <vector>
#include <string>
#include <functional>

namespace fion {

Application::Application()
    : server(std::make_unique<network::Server>(&router)) {}

Application::~Application() { stop(); }


void Application::addRoute(const std::string &pattern, const std::string &method,
                           std::shared_ptr<Handler> handler,
                           const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware,
                           bool isRegex,
                           const std::vector<std::string> &paramKeys) {
  Route route(pattern, method, handler, middleware, isRegex, paramKeys);
  router.addRoute(route);
}

void Application::addGroup(const std::string &prefix, const std::vector<Route> &groupRoutes,
                           const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &groupMiddleware) {
  router.addGroup(prefix, groupRoutes, groupMiddleware);
}

void Application::addResource(const std::string &resource,
                              std::shared_ptr<Handler> handler,
                              const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware) {
  router.addResource(resource, handler, middleware);
}

void Application::run(const std::string &host, std::uint16_t port,
                      std::size_t numThreads) {
  // Initialize logging (log to stderr as well for foreground mode)
  logging::Logger::init("fion", LOG_USER, true);
  logging::Logger::info("Starting Fion server on " + host + ":" +
                        std::to_string(port) + " with " +
                        std::to_string(numThreads) + " I/O threads");

  server->start(host, port, numThreads);

  // Block until server is stopped (e.g., via stop() or signal)
  while (server->is_running()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void Application::stop() {
  if (server) {
    server->stop();
  }
  // Shutdown logging
  logging::Logger::shutdown();
}

} // namespace fion
