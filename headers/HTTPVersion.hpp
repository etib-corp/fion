#pragma once

#include <string>

namespace fion
{
    enum class HTTPVersion
    {
        HTTP_0_9, //< Represents HTTP version 0.9
        HTTP_1_0, //< Represents HTTP version 1.0
        HTTP_1_1, //< Represents HTTP version 1.1
        HTTP_2_0, //< Represents HTTP version 2.0
        HTTP_3_0  //< Represents HTTP version 3.0
    };

    const HTTPVersion stringToHTTPVersion(const std::string &rawHTTPVersion);

    const std::string HTTPVersionToString(const HTTPVersion HTTPVersion);
} // namespace fion