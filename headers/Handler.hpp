#pragma once

#include <memory>
#include "Request.hpp"
#include "Response.hpp"

namespace fion
{

    class Handler
    {
    public:
        virtual ~Handler(void) = default;
        virtual std::unique_ptr<Response> handle(std::unique_ptr<Request> request) = 0;
    };

} // namespace fion
