#pragma once

#include <vector>
#include <memory>
#include "Route.hpp"
#include "Handler.hpp"

namespace fion
{

    class Router
    {
    public:
        std::vector<Route> routes;

        Router() = default;

        void add_route(const Route &route);
        std::shared_ptr<Handler> find_route(const std::string &path, const std::string &method);
    };

} // namespace fion
