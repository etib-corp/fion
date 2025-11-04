#pragma once

#include <map>
#include <string>

#include "http/Headers.hpp"
#include "http/URL.hpp"
#include "http/Version.hpp"

namespace fion::http {
/**
 * @brief HTTP request methods
 *
 * Enumeration of standard HTTP methods as defined in RFC 9110
 */
enum class Method {
  GET,  ///< The GET method requests a representation of the specified resource.
        ///< Requests using GET should only retrieve data and should not contain
        ///< a request content.
  HEAD, ///< The HEAD method asks for a response identical to a GET request, but
        ///< without a response body.
  POST, ///< The POST method submits an entity to the specified resource, often
        ///< causing a change in state or side effects on the server.
  PUT,  ///< The PUT method replaces all current representations of the target
        ///< resource with the request content.
  DELETE,  ///< The DELETE method deletes the specified resource.
  CONNECT, ///< The CONNECT method establishes a tunnel to the server identified
           ///< by the target resource.
  OPTIONS, ///< The OPTIONS method describes the communication options for the
           ///< target resource.
  TRACE, ///< The TRACE method performs a message loop-back test along the path
         ///< to the target resource.
  PATCH  ///< The PATCH method applies partial modifications to a resource.
};

/**
 * @brief Convert a string to an HTTP Method enum
 *
 * @param rawMethod The HTTP method as a string (e.g., "GET", "POST")
 * @return The corresponding Method enum value
 * @throws std::invalid_argument if the method string is not recognized
 */
const Method stringToMethod(std::string rawMethod);

/**
 * @brief Represents an HTTP request
 *
 * This class encapsulates an HTTP request with its method, URL, version,
 * headers, and body. It provides parsing capabilities for raw HTTP request
 * data.
 */
class Request {
private:
  Method _method;
  URL _URL;
  Version _version;
  Headers _headers;
  std::string _body;

  void parseStartLine(const std::string &rawStartLine);
  void parseHeaders(const std::string &rawHeaders);
  void parseBody(const std::string &rawBody);

public:
  /**
   * @brief Construct a new Request object from raw HTTP request data
   *
   * @param rawStartLine The HTTP request start line (e.g., "GET /path
   * HTTP/1.1")
   * @param rawHeaders The raw headers string
   * @param rawBody The request body
   * @throws std::invalid_argument if any part of the request is invalid
   */
  Request(std::string rawStartLine, const std::string &rawHeaders,
          const std::string &rawBody);

  /**
   * @brief Destroy the Request object
   */
  ~Request(void);

  /**
   * @brief Get the HTTP method of the request
   *
   * @return The HTTP method (GET, POST, etc.)
   */
  Method getMethod(void) const { return _method; }

  /**
   * @brief Get the URL of the request
   *
   * @return The URL object
   */
  URL getURL(void) const { return _URL; }

  /**
   * @brief Get the HTTP version of the request
   *
   * @return The HTTP version
   */
  Version getVersion(void) const { return _version; }

  /**
   * @brief Get the headers object (mutable)
   *
   * @return Reference to the Headers object
   */
  Headers &getHeaders(void) { return _headers; }

  /**
   * @brief Get the headers object (const)
   *
   * @return Const reference to the Headers object
   */
  const Headers &getHeaders(void) const { return _headers; }

  /**
   * @brief Get a specific header value by key
   *
   * @param key The header name
   * @return The header value
   * @throws std::invalid_argument if the header is not found
   */
  std::string getHeader(const std::string &key) const {
    return _headers.get(key);
  }

  /**
   * @brief Get the request body
   *
   * @return The request body as a string
   */
  std::string getBody(void) const { return _body; }
};

} // namespace fion::http
