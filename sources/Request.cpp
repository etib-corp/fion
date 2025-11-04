#include <sstream>
#include <istream>

#include "Request.hpp"

namespace fion
{
    const Method stringToMethod(std::string rawMethod)
    {
        if (rawMethod == "GET")
            return Method::GET;
        else if (rawMethod == "HEAD")
            return Method::HEAD;
        else if (rawMethod == "POST")
            return Method::POST;
        else if (rawMethod == "PUT")
            return Method::PUT;
        else if (rawMethod == "DELETE")
            return Method::DELETE;
        else if (rawMethod == "CONNECT")
            return Method::CONNECT;
        else if (rawMethod == "OPTIONS")
            return Method::OPTIONS;
        else if (rawMethod == "TRACE")
            return Method::TRACE;
        else if (rawMethod == "PATCH")
            return Method::PATCH;
        else
            throw std::invalid_argument("Invalid HTTP Method");
    }

    void Request::parseStartLine(const std::string &rawStartLine)
    {
        std::istringstream stream(rawStartLine);
        std::string rawMethod;
        std::string rawURL;
        std::string rawHTTPVersion;

        stream >> rawMethod >> rawURL >> rawHTTPVersion;

        try
        {
            _method = stringToMethod(rawMethod);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid HTTP Method");
        }

        try
        {
            _URL = URL(rawURL);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid URL");
        }

        try
        {
            _HTTPVersion = stringToHTTPVersion(rawHTTPVersion);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid HTTP Version");
        }
    }

    void Request::parseHeaders(const std::string &rawHeaders)
    {
        std::istringstream stream(rawHeaders);
        std::string line;
        while (std::getline(stream, line) && line != "\r")
        {
            auto delimiterPos = line.find(": ");
            if (delimiterPos != std::string::npos)
            {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 2);
                _headers[key] = value;
            }
        }
    }

    void Request::parseBody(const std::string &rawBody)
    {
        _body = rawBody;
    }

    Request::Request(std::string rawStartLine, const std::string &rawHeaders, const std::string &rawBody)
    {
        try
        {
            parseStartLine(rawStartLine);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid HTTP Request");
        }

        try
        {
            parseHeaders(rawHeaders);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid HTTP Headers");
        }

        try
        {
            parseBody(rawBody);
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid HTTP Body");
        }
    }

    Request::~Request(void) {}

} // namespace fion
