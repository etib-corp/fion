#pragma once

#include "Handler.hpp"
#include <memory>
#include <string>

namespace fion {

class Route {
public:
  std::string path;
  std::string method;
  std::shared_ptr<Handler> handler;

  Route() = default;
  Route(const std::string &path, const std::string &method,
        std::shared_ptr<Handler> handler);
};

} // namespace fion
