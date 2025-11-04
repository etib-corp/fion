#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "http/Headers.hpp"
#include "http/Version.hpp"

namespace fion::http {
/**
 * @brief HTTP status codes
 *
 * Enumeration of standard HTTP status codes as defined in RFC 9110
 * and other relevant specifications
 */
enum class StatusCode : ::std::int16_t {
  // Informational responses
  CONTINUE = 100, ///< The server has received the request headers and the
                  ///< client should proceed to send the request body
  SWITCHING_PROTOCOLS = 101, ///< The server is switching protocols according to
                             ///< the Upgrade header
  PROCESSING = 102,  ///< The server has received and is processing the request,
                     ///< but no response is available yet
  EARLY_HINTS = 103, ///< The server is sending preliminary headers before the
                     ///< final response

  // Successful responses
  OK = 200,       ///< The request has succeeded
  CREATED = 201,  ///< The request has been fulfilled and resulted in a new
                  ///< resource being created
  ACCEPTED = 202, ///< The request has been accepted for processing, but the
                  ///< processing has not been completed
  NON_AUTHORITATIVE_INFORMATION =
      203, ///< The server is successfully returning modified information
  NO_CONTENT = 204,    ///< The server successfully processed the request and is
                       ///< not returning any content
  RESET_CONTENT = 205, ///< The server successfully processed the request, but
                       ///< is not returning any content
  PARTIAL_CONTENT = 206, ///< The server is delivering only part of the resource
                         ///< due to a range header sent by the client
  MULTI_STATUS = 207, ///< The message body that follows is an XML message and
                      ///< can contain multiple response codes
  ALREADY_REPORTED = 208, ///< The members of a DAV binding have already been
                          ///< enumerated in a previous reply to this request
  IM_USED =
      226, ///< The server has fulfilled a request for the resource and the
           ///< response is a representation of the status of the resource

  // Redirection messages
  MULTIPLE_CHOICES = 300, ///< The request has more than one possible response
  MOVED_PERMANENTLY =
      301, ///< The requested resource has been assigned a new permanent URI
  FOUND =
      302, ///< The requested resource resides temporarily under a different URI
  SEE_OTHER =
      303, ///< The response to the request can be found under another URI
  NOT_MODIFIED =
      304, ///< The resource has not been modified since the last request
  USE_PROXY = 305, ///< The requested resource must be accessed through the
                   ///< proxy given by the Location field
  UNUSED = 306,    ///< This unused status code is reserved
  TEMPORARY_REDIRECT =
      307, ///< The requested resource resides temporarily under a different URI
  PERMANENT_REDIRECT =
      308, ///< The requested resource has been assigned a new permanent URI

  // Client error responses
  BAD_REQUEST = 400,  ///< The server cannot or will not process the request due
                      ///< to a client error
  UNAUTHORIZED = 401, ///< Authentication is required and has failed or has not
                      ///< yet been provided
  PAYMENT_REQUIRED = 402, ///< This code is reserved for future use
  FORBIDDEN = 403, ///< The request was valid, but the server is refusing action
  NOT_FOUND = 404, ///< The requested resource could not be found
  METHOD_NOT_ALLOWED =
      405, ///< A request method is not supported for the requested resource
  NOT_ACCEPTABLE =
      406, ///< The requested resource is capable of generating only content not
           ///< acceptable according to the Accept headers sent in the request
  PROXY_AUTHENTICATION_REQUIRED = 407, ///< Proxy authentication is required
  REQUEST_TIMEOUT = 408, ///< The server timed out waiting for the request
  CONFLICT = 409, ///< The request could not be completed due to a conflict with
                  ///< the current state of the resource
  GONE = 410, ///< The requested resource is no longer available and will not be
              ///< available again
  LENGTH_REQUIRED =
      411, ///< The request did not specify the length of its content, which is
           ///< required by the requested resource
  PRECONDITION_FAILED =
      412, ///< The server does not meet one of the preconditions that the
           ///< requester put on the request
  CONTENT_TOO_LARGE = 413, ///< The request is larger than the server is willing
                           ///< or able to process
  URI_TOO_LONG =
      414, ///< The URI provided was too long for the server to process
  UNSUPPORTED_MEDIA_TYPE = 415, ///< The request entity has a media type which
                                ///< the server or resource does not support
  RANGE_NOT_SATISFIABLE =
      416, ///< The client has asked for a portion of the file, but the server
           ///< cannot supply that portion
  EXPECTATION_FAILED = 417, ///< The server cannot meet the requirements of the
                            ///< Expect request-header field
  IM_A_TEAPOT = 418, ///< Any attempt to instruct an HTTP/1.1 server to "teapot"
                     ///< should fail with the error "418 I'm a teapot"
  MISDIRECTED_REQUEST = 421,   ///< The request was directed at a server that is
                               ///< not able to produce a response
  UNPROCESSABLE_CONTENT = 422, ///< The request was well-formed but was unable
                               ///< to be followed due to semantic errors
  LOCKED = 423, ///< The resource that is being accessed is locked
  FAILED_DEPENDENCY =
      424,         ///< The request failed due to failure of a previous request
  TOO_EARLY = 425, ///< The server is unwilling to risk processing a request
                   ///< that might be replayed
  UPGRADE_REQUIRED = 426, ///< The client should switch to a different protocol
  PRECONDITION_REQUIRED =
      428, ///< The server requires the request to be conditional
  TOO_MANY_REQUESTS =
      429, ///< The user has sent too many requests in a given amount of time
  REQUEST_HEADER_FIELDS_TOO_LARGE =
      431, ///< The server is unwilling to process the request because its
           ///< header fields are too large
  UNAVAILABLE_FOR_LEGAL_REASONS =
      451, ///< The requested resource is not available due to legal reasons

  // Server error responses
  INTERNAL_SERVER_ERROR = 500, ///< A generic error message, given when an
                               ///< unexpected condition was encountered
  NOT_IMPLEMENTED =
      501, ///< The server either does not recognize the request method, or it
           ///< lacks the ability to fulfill the request
  BAD_GATEWAY = 502, ///< The server was acting as a gateway or proxy and
                     ///< received an invalid response from the upstream server
  SERVICE_UNAVAILABLE =
      503, ///< The server is currently unable to handle the request due to
           ///< temporary overloading or maintenance of the server
  GATEWAY_TIMEOUT =
      504, ///< The server was acting as a gateway or proxy and did not receive
           ///< a timely response from the upstream server
  HTTP_VERSION_NOT_SUPPORTED =
      505, ///< The server does not support the HTTP protocol version that was
           ///< used in the request
  VARIANT_ALSO_NEGOTIATES =
      506, ///< The server has an internal configuration error: transparent
           ///< content negotiation for this resource is not supported
  INSUFFICIENT_STORAGE = 507, ///< The server is unable to store the
                              ///< representation needed to complete the request
  LOOP_DETECTED =
      508, ///< The server detected an infinite loop while processing a request
  NOT_EXTENDED = 510, ///< Further extensions to the request are required for
                      ///< the server to fulfill it
  NETWORK_AUTHENTICATION_REQUIRED =
      511 ///< The client must authenticate itself to gain network access
};

/**
 * @brief Convert a StatusCode to its string representation
 *
 * @param statusCode The status code to convert
 * @return The string representation of the status code (e.g., "OK", "Not
 * Found")
 * @throws std::invalid_argument if the status code is not recognized
 */
const std::string statusCodeToString(const StatusCode statusCode);

/**
 * @brief Represents an HTTP response
 *
 * This class encapsulates an HTTP response with its version, status code,
 * headers, and body. It provides methods to build and serialize HTTP responses.
 */
class Response {
private:
  Version _version;
  StatusCode _statusCode;
  Headers _headers;
  std::string _body;

public:
  /**
   * @brief Construct a new Response object
   */
  Response(void);

  /**
   * @brief Destroy the Response object
   */
  ~Response(void);

  /**
   * @brief Set the HTTP version for the response
   *
   * @param Version The HTTP version to use
   */
  void setVersion(const Version Version) { _version = Version; }

  /**
   * @brief Set the status code for the response
   *
   * @param statusCode The HTTP status code
   */
  void setStatusCode(const StatusCode statusCode) { _statusCode = statusCode; }

  /**
   * @brief Set a header key-value pair
   *
   * @param key The header name
   * @param value The header value
   */
  void setHeader(const std::string &key, const std::string &value) {
    _headers.set(key, value);
  }

  /**
   * @brief Set the response body
   *
   * @param body The response body content
   */
  void setBody(const std::string &body) { _body = body; }

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
   * @brief Convert the response to raw HTTP response format
   *
   * @return The raw HTTP response string with status line, headers, and body
   */
  const std::string toRawResponse(void) const;
};

} // namespace fion::http
