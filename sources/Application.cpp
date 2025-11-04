#include <iostream>

#include "Application.hpp"

namespace fion
{

    void Application::add_route(const std::string &path, const std::string &method,
                                std::shared_ptr<Handler> handler)
    {
        Route route(path, method, handler);
        router.add_route(route);
    }

    void Application::run(const std::string &host, int port)
    {
        std::cout << "Server running on " << host << ":" << port << std::endl;
        // TODO: Implement server logic (socket binding, listening, request handling)
    }

} // namespace fion
