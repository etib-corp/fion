#pragma once

#include <string>
#include <map>

#include "URL.hpp"
#include "HTTPVersion.hpp"

namespace fion
{
    enum class Method
    {
        GET,     // The GET method requests a representation of the specified resource. Requests using GET should only retrieve data and should not contain a request content.
        HEAD,    // The HEAD method asks for a response identical to a GET request, but without a response body.
        POST,    // The POST method submits an entity to the specified resource, often causing a change in state or side effects on the server.
        PUT,     // The PUT method replaces all current representations of the target resource with the request content.
        DELETE,  // The DELETE method deletes the specified resource.
        CONNECT, // The CONNECT method establishes a tunnel to the server identified by the target resource.
        OPTIONS, // The OPTIONS method describes the communication options for the target resource.
        TRACE,   // The TRACE method performs a message loop-back test along the path to the target resource.
        PATCH    // The PATCH method applies partial modifications to a resource.
    };

    const Method stringToMethod(std::string rawMethod);

    class Request
    {
    private:
        Method _method;
        URL _URL;
        HTTPVersion _HTTPVersion;
        std::map<std::string, std::string> _headers;
        std::string _body;

        void parseStartLine(const std::string &rawStartLine);
        void parseHeaders(const std::string &rawHeaders);
        void parseBody(const std::string &rawBody);

    public:
        Request(std::string rawStartLine, const std::string &rawHeaders, const std::string &rawBody);
        ~Request(void);

        Method getMethod(void) const { return _method; }

        URL getURL(void) const { return _URL; }

        HTTPVersion getHTTPVersion(void) const { return _HTTPVersion; }

        std::string getHeader(const std::string &key) const
        {
            if (_headers.find(key) != _headers.end())
                return _headers.at(key);
            throw std::invalid_argument("Header not found");
        }

        std::string getBody(void) const { return _body; }
    };

} // namespace fion
