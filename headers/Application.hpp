#pragma once

#include "Handler.hpp"
#include "Router.hpp"
#include "network/Server.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace fion {

class Application {
private:
  Router router;
  std::unique_ptr<network::Server> server;

public:
  Application();
  ~Application();

  void addRoute(const std::string &pattern, const std::string &method,
                std::shared_ptr<Handler> handler,
                const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware = {},
                bool isRegex = false,
                const std::vector<std::string> &paramKeys = {});

  void addGroup(const std::string &prefix, const std::vector<Route> &groupRoutes,
                const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &groupMiddleware = {});

  void addResource(const std::string &resource,
                   std::shared_ptr<Handler> handler,
                   const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware = {});

  void run(const std::string &host, std::uint16_t port,
           std::size_t numThreads = 4);
  void stop();

  Router &getRouter() { return router; }
};

} // namespace fion
