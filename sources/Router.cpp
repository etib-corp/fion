
#include "Router.hpp"
#include "logging/Logger.hpp"
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <functional>

namespace fion {


void Router::addRoute(const Route &route) {
  routes.push_back(route);
  logging::Logger::debug("Router: added route pattern=" + route.pathPattern +
                         " method=" + route.method);
}


// Helper: split path into segments
static std::vector<std::string> splitPath(const std::string &path) {
  std::vector<std::string> segments;
  size_t start = 0, end = 0;
  while ((end = path.find('/', start)) != std::string::npos) {
    if (end > start) segments.push_back(path.substr(start, end - start));
    start = end + 1;
  }
  if (start < path.size()) segments.push_back(path.substr(start));
  return segments;
}

// Helper: match path with pattern and extract params
static bool matchPattern(const std::string &pattern, const std::string &path,
                        std::vector<std::string> &paramKeys,
                        std::map<std::string, std::string> &params) {
  auto patSegs = splitPath(pattern);
  auto pathSegs = splitPath(path);
  if (patSegs.size() != pathSegs.size()) return false;
  for (size_t i = 0; i < patSegs.size(); ++i) {
    if (!patSegs[i].empty() && patSegs[i][0] == ':') {
      std::string key = patSegs[i].substr(1);
      paramKeys.push_back(key);
      params[key] = pathSegs[i];
    } else if (patSegs[i] != pathSegs[i]) {
      return false;
    }
  }
  return true;
}

#include <regex>

std::shared_ptr<Handler> Router::findRoute(const std::string &path,
                                           const std::string &method,
                                           std::map<std::string, std::string> &outParams,
                                           std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &outMiddleware) {
  logging::Logger::debug("Router: searching for route path=" + path +
                         " method=" + method);
  for (const auto &route : routes) {
    if (route.method != method) continue;
    if (route.isRegex) {
      std::smatch match;
      std::regex re(route.pathPattern);
      if (std::regex_match(path, match, re)) {
        // Extract named groups if any
        for (size_t i = 1; i < match.size(); ++i) {
          if (i-1 < route.paramKeys.size())
            outParams[route.paramKeys[i-1]] = match[i];
        }
        outMiddleware = route.middleware;
        logging::Logger::debug("Router: regex route matched");
        return route.handler;
      }
    } else {
      std::vector<std::string> paramKeys;
      std::map<std::string, std::string> params;
      if (matchPattern(route.pathPattern, path, paramKeys, params)) {
        outParams = params;
        outMiddleware = route.middleware;
        logging::Logger::debug("Router: param route matched");
        return route.handler;
      }
    }
  }
  logging::Logger::debug("Router: no matching route");
  return nullptr;
}

// Grouping and RESTful helpers (stubs)
void Router::addGroup(const std::string &prefix, const std::vector<Route> &groupRoutes,
                      const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &groupMiddleware) {
  for (auto route : groupRoutes) {
    route.pathPattern = prefix + route.pathPattern;
    for (const auto &mw : groupMiddleware) route.middleware.push_back(mw);
    addRoute(route);
  }
}

void Router::addResource(const std::string &resource,
                         std::shared_ptr<Handler> handler,
                         const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware) {
  // RESTful: GET /resource, POST /resource, GET /resource/:id, PUT /resource/:id, DELETE /resource/:id
  addRoute(Route("/" + resource, "GET", handler, middleware));
  addRoute(Route("/" + resource, "POST", handler, middleware));
  addRoute(Route("/" + resource + "/:id", "GET", handler, middleware));
  addRoute(Route("/" + resource + "/:id", "PUT", handler, middleware));
  addRoute(Route("/" + resource + "/:id", "DELETE", handler, middleware));
}

} // namespace fion
