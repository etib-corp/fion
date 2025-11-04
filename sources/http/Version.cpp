#include "http/Version.hpp"

const fion::http::Version fion::http::stringToVersion(const std::string &rawVersion)
{
    if (rawVersion == "HTTP/0.9")
        return fion::http::Version::HTTP_0_9;
    else if (rawVersion == "HTTP/1.0")
        return fion::http::Version::HTTP_1_0;
    else if (rawVersion == "HTTP/1.1")
        return fion::http::Version::HTTP_1_1;
    else if (rawVersion == "HTTP/2.0")
        return fion::http::Version::HTTP_2_0;
    else if (rawVersion == "HTTP/3.0")
        return fion::http::Version::HTTP_3_0;
    else
        throw std::invalid_argument("Invalid HTTP Version");
}

const std::string fion::http::VersionToString(const fion::http::Version Version)
{
    switch (Version)
    {
    case fion::http::Version::HTTP_0_9:
        return "HTTP/0.9";
    case fion::http::Version::HTTP_1_0:
        return "HTTP/1.0";
    case fion::http::Version::HTTP_1_1:
        return "HTTP/1.1";
    case fion::http::Version::HTTP_2_0:
        return "HTTP/2.0";
    case fion::http::Version::HTTP_3_0:
        return "HTTP/3.0";
    default:
        throw std::invalid_argument("Unknown HTTP Version");
    }
}
