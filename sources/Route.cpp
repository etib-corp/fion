
#include "Route.hpp"
#include <vector>
#include <string>
#include <functional>

namespace fion {


Route::Route(const std::string &pattern, const std::string &method,
             std::shared_ptr<Handler> handler,
             const std::vector<std::function<void(std::unique_ptr<http::Request>&)>> &middleware,
             bool isRegex,
             const std::vector<std::string> &paramKeys)
    : pathPattern(pattern), method(method), handler(handler), middleware(middleware), isRegex(isRegex), paramKeys(paramKeys) {}

} // namespace fion
