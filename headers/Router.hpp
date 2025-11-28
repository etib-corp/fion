#pragma once


#include "Handler.hpp"
#include "Route.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace fion {

class Router {
public:
  std::vector<Route> routes;

  Router() = default;

  void addRoute(const Route &route);

  // Find route and extract parameters
  std::shared_ptr<Handler> findRoute(const std::string &path,
                                     const std::string &method,
                                     std::map<std::string, std::string> &outParams,
                                     std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &outMiddleware);

  // Grouping and RESTful helpers
  void addGroup(const std::string &prefix, const std::vector<Route> &groupRoutes,
                const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &groupMiddleware = {});
  void addResource(const std::string &resource,
                   std::shared_ptr<Handler> handler,
                   const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware = {});
};

} // namespace fion
