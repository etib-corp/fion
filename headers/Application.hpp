#pragma once

#include <string>
#include <memory>
#include "Router.hpp"
#include "Handler.hpp"

namespace fion
{

    class Application
    {
    private:
        Router router;

    public:
        Application() = default;

        void add_route(const std::string &path, const std::string &method,
                       std::shared_ptr<Handler> handler);
        void run(const std::string &host, int port);

        Router &get_router() { return router; }
    };

} // namespace fion
