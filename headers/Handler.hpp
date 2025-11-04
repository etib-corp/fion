#pragma once

#include "http/Request.hpp"
#include "http/Response.hpp"
#include <memory>

namespace fion {

class Handler {
public:
  virtual ~Handler(void) = default;
  virtual std::unique_ptr<http::Response>
  handle(std::unique_ptr<http::Request> request) = 0;
};

} // namespace fion
