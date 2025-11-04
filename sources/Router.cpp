#include "Router.hpp"

namespace fion
{

    void Router::add_route(const Route &route)
    {
        routes.push_back(route);
    }

    std::shared_ptr<Handler> Router::find_route(const std::string &path, const std::string &method)
    {
        for (const auto &route : routes)
        {
            if (route.path == path && route.method == method)
            {
                return route.handler;
            }
        }
        return nullptr;
    }

} // namespace fion
