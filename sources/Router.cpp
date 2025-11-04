#include "Router.hpp"
#include "logging/Logger.hpp"

namespace fion {

void Router::addRoute(const Route &route) {
  routes.push_back(route);
  logging::Logger::debug("Router: added route path=" + route.path +
                         " method=" + route.method);
}

std::shared_ptr<Handler> Router::findRoute(const std::string &path,
                                           const std::string &method) {
  logging::Logger::debug("Router: searching for route path=" + path +
                         " method=" + method);
  for (const auto &route : routes) {
    if (route.path == path && route.method == method) {
      logging::Logger::debug("Router: found matching route");
      return route.handler;
    }
  }
  logging::Logger::debug("Router: no matching route");
  return nullptr;
}

} // namespace fion
