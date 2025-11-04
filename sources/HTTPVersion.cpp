#include "HTTPVersion.hpp"

namespace fion
{
    const HTTPVersion stringToHTTPVersion(const std::string &rawHTTPVersion)
    {
        if (rawHTTPVersion == "HTTP/0.9")
            return HTTPVersion::HTTP_0_9;
        else if (rawHTTPVersion == "HTTP/1.0")
            return HTTPVersion::HTTP_1_0;
        else if (rawHTTPVersion == "HTTP/1.1")
            return HTTPVersion::HTTP_1_1;
        else if (rawHTTPVersion == "HTTP/2.0")
            return HTTPVersion::HTTP_2_0;
        else if (rawHTTPVersion == "HTTP/3.0")
            return HTTPVersion::HTTP_3_0;
        else
            throw std::invalid_argument("Invalid HTTP Version");
    }

    const std::string HTTPVersionToString(const HTTPVersion HTTPVersion)
    {
        switch (HTTPVersion)
        {
        case HTTPVersion::HTTP_0_9:
            return "HTTP/0.9";
        case HTTPVersion::HTTP_1_0:
            return "HTTP/1.0";
        case HTTPVersion::HTTP_1_1:
            return "HTTP/1.1";
        case HTTPVersion::HTTP_2_0:
            return "HTTP/2.0";
        case HTTPVersion::HTTP_3_0:
            return "HTTP/3.0";
        default:
            throw std::invalid_argument("Unknown HTTP Version");
        }
    }
} // namespace fion