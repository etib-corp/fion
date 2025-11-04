#include "Route.hpp"

namespace fion
{

    Route::Route(const std::string &path, const std::string &method,
                 std::shared_ptr<Handler> handler)
        : path(path), method(method), handler(handler) {}

} // namespace fion
