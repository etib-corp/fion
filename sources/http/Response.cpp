#include <sstream>

#include "http/Response.hpp"

const std::string
fion::http::statusCodeToString(const fion::http::StatusCode statusCode) {
  switch (statusCode) {
  case fion::http::StatusCode::CONTINUE:
    return "Continue";
  case fion::http::StatusCode::SWITCHING_PROTOCOLS:
    return "Switching Protocols";
  case fion::http::StatusCode::PROCESSING:
    return "Processing";
  case fion::http::StatusCode::EARLY_HINTS:
    return "Early Hints";
  case fion::http::StatusCode::OK:
    return "OK";
  case fion::http::StatusCode::CREATED:
    return "Created";
  case fion::http::StatusCode::ACCEPTED:
    return "Accepted";
  case fion::http::StatusCode::NON_AUTHORITATIVE_INFORMATION:
    return "Non-Authoritative Information";
  case fion::http::StatusCode::NO_CONTENT:
    return "No Content";
  case fion::http::StatusCode::RESET_CONTENT:
    return "Reset Content";
  case fion::http::StatusCode::PARTIAL_CONTENT:
    return "Partial Content";
  case fion::http::StatusCode::MULTI_STATUS:
    return "Multi-Status";
  case fion::http::StatusCode::ALREADY_REPORTED:
    return "Already Reported";
  case fion::http::StatusCode::IM_USED:
    return "IM Used";
  case fion::http::StatusCode::MULTIPLE_CHOICES:
    return "Multiple Choices";
  case fion::http::StatusCode::MOVED_PERMANENTLY:
    return "Moved Permanently";
  case fion::http::StatusCode::FOUND:
    return "Found";
  case fion::http::StatusCode::SEE_OTHER:
    return "See Other";
  case fion::http::StatusCode::NOT_MODIFIED:
    return "Not Modified";
  case fion::http::StatusCode::USE_PROXY:
    return "Use Proxy";
  case fion::http::StatusCode::UNUSED:
    return "Unused";
  case fion::http::StatusCode::TEMPORARY_REDIRECT:
    return "Temporary Redirect";
  case fion::http::StatusCode::PERMANENT_REDIRECT:
    return "Permanent Redirect";
  case fion::http::StatusCode::BAD_REQUEST:
    return "Bad Request";
  case fion::http::StatusCode::UNAUTHORIZED:
    return "Unauthorized";
  case fion::http::StatusCode::PAYMENT_REQUIRED:
    return "Payment Required";
  case fion::http::StatusCode::FORBIDDEN:
    return "Forbidden";
  case fion::http::StatusCode::NOT_FOUND:
    return "Not Found";
  case fion::http::StatusCode::METHOD_NOT_ALLOWED:
    return "Method Not Allowed";
  case fion::http::StatusCode::NOT_ACCEPTABLE:
    return "Not Acceptable";
  case fion::http::StatusCode::PROXY_AUTHENTICATION_REQUIRED:
    return "Proxy Authentication Required";
  case fion::http::StatusCode::REQUEST_TIMEOUT:
    return "Request Timeout";
  case fion::http::StatusCode::CONFLICT:
    return "Conflict";
  case fion::http::StatusCode::GONE:
    return "Gone";
  case fion::http::StatusCode::LENGTH_REQUIRED:
    return "Length Required";
  case fion::http::StatusCode::PRECONDITION_FAILED:
    return "Precondition Failed";
  case fion::http::StatusCode::CONTENT_TOO_LARGE:
    return "Content Too Large";
  case fion::http::StatusCode::URI_TOO_LONG:
    return "URI Too Long";
  case fion::http::StatusCode::UNSUPPORTED_MEDIA_TYPE:
    return "Unsupported Media Type";
  case fion::http::StatusCode::RANGE_NOT_SATISFIABLE:
    return "Range Not Satisfiable";
  case fion::http::StatusCode::EXPECTATION_FAILED:
    return "Expectation Failed";
  case fion::http::StatusCode::IM_A_TEAPOT:
    return "I'm a teapot";
  case fion::http::StatusCode::MISDIRECTED_REQUEST:
    return "Misdirected Request";
  case fion::http::StatusCode::UNPROCESSABLE_CONTENT:
    return "Unprocessable Content";
  case fion::http::StatusCode::LOCKED:
    return "Locked";
  case fion::http::StatusCode::FAILED_DEPENDENCY:
    return "Failed Dependency";
  case fion::http::StatusCode::TOO_EARLY:
    return "Too Early";
  case fion::http::StatusCode::UPGRADE_REQUIRED:
    return "Upgrade Required";
  case fion::http::StatusCode::PRECONDITION_REQUIRED:
    return "Precondition Required";
  case fion::http::StatusCode::TOO_MANY_REQUESTS:
    return "Too Many Requests";
  case fion::http::StatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE:
    return "Request Header Fields Too Large";
  case fion::http::StatusCode::UNAVAILABLE_FOR_LEGAL_REASONS:
    return "Unavailable For Legal Reasons";
  case fion::http::StatusCode::INTERNAL_SERVER_ERROR:
    return "Internal Server Error";
  case fion::http::StatusCode::NOT_IMPLEMENTED:
    return "Not Implemented";
  case fion::http::StatusCode::BAD_GATEWAY:
    return "Bad Gateway";
  case fion::http::StatusCode::SERVICE_UNAVAILABLE:
    return "Service Unavailable";
  case fion::http::StatusCode::GATEWAY_TIMEOUT:
    return "Gateway Timeout";
  case fion::http::StatusCode::HTTP_VERSION_NOT_SUPPORTED:
    return "HTTP Version Not Supported";
  case fion::http::StatusCode::VARIANT_ALSO_NEGOTIATES:
    return "Variant Also Negotiates";
  case fion::http::StatusCode::INSUFFICIENT_STORAGE:
    return "Insufficient Storage";
  case fion::http::StatusCode::LOOP_DETECTED:
    return "Loop Detected";
  case fion::http::StatusCode::NOT_EXTENDED:
    return "Not Extended";
  case fion::http::StatusCode::NETWORK_AUTHENTICATION_REQUIRED:
    return "Network Authentication Required";
  default:
    throw std::invalid_argument("Unknown StatusCode");
  }
}

fion::http::Response::Response(void)
    : _version(Version::HTTP_1_1), _statusCode(StatusCode::OK) {}

fion::http::Response::~Response(void) {}

const std::string fion::http::Response::toRawResponse(void) const {
  std::ostringstream rawResponse;

  // Start line
  rawResponse << fion::http::VersionToString(_version) << " "
              << static_cast<unsigned short>(_statusCode) << " "
              << fion::http::statusCodeToString(_statusCode) << "\r\n";

  // Headers
  rawResponse << _headers.toRawString();

  // Blank line to separate headers from body
  rawResponse << "\r\n";

  // Body
  rawResponse << _body;

  return rawResponse.str();
}
