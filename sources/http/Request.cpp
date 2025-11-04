#include <sstream>
#include <istream>

#include "http/Request.hpp"

const fion::http::Method fion::http::stringToMethod(std::string rawMethod)
{
    if (rawMethod == "GET")
        return fion::http::Method::GET;
    else if (rawMethod == "HEAD")
        return fion::http::Method::HEAD;
    else if (rawMethod == "POST")
        return fion::http::Method::POST;
    else if (rawMethod == "PUT")
        return fion::http::Method::PUT;
    else if (rawMethod == "DELETE")
        return fion::http::Method::DELETE;
    else if (rawMethod == "CONNECT")
        return fion::http::Method::CONNECT;
    else if (rawMethod == "OPTIONS")
        return fion::http::Method::OPTIONS;
    else if (rawMethod == "TRACE")
        return fion::http::Method::TRACE;
    else if (rawMethod == "PATCH")
        return fion::http::Method::PATCH;
    else
        throw std::invalid_argument("Invalid HTTP Method");
}

void fion::http::Request::parseStartLine(const std::string &rawStartLine)
{
    std::istringstream stream(rawStartLine);
    std::string rawMethod;
    std::string rawURL;
    std::string rawVersion;

    stream >> rawMethod >> rawURL >> rawVersion;

    try
    {
        _method = fion::http::stringToMethod(rawMethod);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument("Invalid HTTP Method");
    }

    try
    {
        _URL = fion::http::URL(rawURL);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument("Invalid URL");
    }

    try
    {
        _version = fion::http::stringToVersion(rawVersion);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument("Invalid HTTP Version");
    }
}

void fion::http::Request::parseHeaders(const std::string &rawHeaders)
{
    _headers.parseFromRaw(rawHeaders);
}

void fion::http::Request::parseBody(const std::string &rawBody)
{
    _body = rawBody;
}

fion::http::Request::Request(std::string rawStartLine, const std::string &rawHeaders, const std::string &rawBody)
{
    try
    {
        parseStartLine(rawStartLine);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument("Invalid HTTP Request");
    }

    try
    {
        parseHeaders(rawHeaders);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument("Invalid HTTP Headers");
    }

    try
    {
        parseBody(rawBody);
    }
    catch (const std::invalid_argument &)
    {
        throw std::invalid_argument("Invalid HTTP Body");
    }
}

fion::http::Request::~Request(void) {}
