#pragma once

#include "Handler.hpp"
#include "Route.hpp"
#include <memory>
#include <vector>

namespace fion {

class Router {
public:
  std::vector<Route> routes;

  Router() = default;

  void addRoute(const Route &route);
  std::shared_ptr<Handler> findRoute(const std::string &path,
                                     const std::string &method);
};

} // namespace fion
