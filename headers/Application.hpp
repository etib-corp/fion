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

  void addRoute(const std::string &path, const std::string &method,
                std::shared_ptr<Handler> handler);
  void run(const std::string &host, std::uint16_t port,
           std::size_t numThreads = 4);
  void stop();

  Router &getRouter() { return router; }
};

} // namespace fion
