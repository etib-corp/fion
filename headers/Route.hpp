#pragma once

#include <string>
#include <memory>
#include "Handler.hpp"

namespace fion
{

    class Route
    {
    public:
        std::string path;
        std::string method;
        std::shared_ptr<Handler> handler;

        Route() = default;
        Route(const std::string &path, const std::string &method,
              std::shared_ptr<Handler> handler);
    };

} // namespace fion
