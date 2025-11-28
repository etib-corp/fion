#pragma once

#include "Handler.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace fion {


class Route {
public:
  std::string pathPattern; // e.g. /users/:id or regex
  std::string method;
  std::shared_ptr<Handler> handler;
  std::vector<std::function<void(std::unique_ptr<http::Request>&)>> middleware;
  bool isRegex = false;
  std::vector<std::string> paramKeys; // e.g. ["id"]

  Route() = default;
  Route(const std::string &pattern, const std::string &method,
        std::shared_ptr<Handler> handler,
        const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware = {},
        bool isRegex = false,
        const std::vector<std::string> &paramKeys = {});
};

} // namespace fion
