#include "network/Client.hpp"
#include "logging/Logger.hpp"
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace fion::network {
Client::Client(int fd) : _fd(fd), _state(ClientState::READING_REQUEST) {
  if (fd < 0)
    throw std::invalid_argument("Invalid file descriptor");
  logging::Logger::debug("Client: created for fd=" + std::to_string(fd));
}

Client::~Client() {
  if (_fd >= 0) {
    logging::Logger::debug("Client: closing fd=" + std::to_string(_fd));
    ::close(_fd);
    _fd = -1;
  }
}

Client::Client(Client &&other) noexcept : _fd(other._fd), _state(other._state) {
  other._fd = -1;
  // Note: buffers cannot be moved due to mutex, they will be empty in the new
  // object
}

Client &Client::operator=(Client &&other) noexcept {
  if (this != &other) {
    if (_fd >= 0)
      ::close(_fd);

    _fd = other._fd;
    _state = other._state;

    other._fd = -1;
    // Note: buffers cannot be moved due to mutex
  }
  return *this;
}

ssize_t Client::readRequest() {
  char buffer[4096] = {0};
  ssize_t bytes_read = ::recv(_fd, buffer, sizeof(buffer), 0);

  if (bytes_read > 0) {
    _requestBuffer.append(buffer, bytes_read);
    logging::Logger::debug("Client fd=" + std::to_string(_fd) +
                           " read=" + std::to_string(bytes_read));
  } else {
    logging::Logger::debug("Client fd=" + std::to_string(_fd) + " read=" +
                           std::to_string(bytes_read) + " (closed/error)");
  }

  return bytes_read;
}

ssize_t Client::writeResponse() {
  auto data = _responseBuffer.getData();
  if (data.empty())
    return 0;

  ssize_t bytes_sent = ::send(_fd, data.data(), data.size(), 0);

  if (bytes_sent > 0) {
    // For simplicity, we assume all data is sent at once
    // In a production system, you'd need to handle partial sends
    _responseBuffer.clear();
    logging::Logger::debug("Client fd=" + std::to_string(_fd) +
                           " sent=" + std::to_string(bytes_sent));
  } else {
    logging::Logger::error("Client fd=" + std::to_string(_fd) +
                           " send error=" + std::to_string(bytes_sent));
  }

  return bytes_sent;
}

void Client::prepare_response(const http::Response &response) {
  std::string raw_response = response.toRawResponse();
  _responseBuffer.clear();
  _responseBuffer.append(raw_response.data(), raw_response.size());
  logging::Logger::debug(
      "Client fd=" + std::to_string(_fd) +
      " response prepared, size=" + std::to_string(raw_response.size()));
}

bool Client::is_request_ready() const {
  auto data = _requestBuffer.getData();
  if (data.empty())
    return false;

  std::string str(data);

  // First, check if we have complete headers (double CRLF)
  size_t headers_end = str.find("\r\n\r\n");
  if (headers_end == std::string::npos)
    return false; // Headers not complete yet

  // Now check if we have the complete body based on Content-Length
  // Find the Content-Length header
  size_t content_length_pos = str.find("Content-Length:");
  if (content_length_pos == std::string::npos) {
    // No Content-Length header, request is ready after headers
    // (GET, DELETE, etc. typically have no body)
    return true;
  }

  // Parse Content-Length value
  size_t value_start = content_length_pos + 15; // strlen("Content-Length:")
  while (value_start < str.size() &&
         (str[value_start] == ' ' || str[value_start] == '\t'))
    value_start++;

  size_t value_end = str.find("\r\n", value_start);
  if (value_end == std::string::npos)
    return false;

  std::string length_str = str.substr(value_start, value_end - value_start);
  size_t content_length = 0;
  try {
    content_length = std::stoull(length_str);
  } catch (...) {
    // Invalid Content-Length, treat as ready to return error
    return true;
  }

  // Check if we have received the full body
  size_t body_start = headers_end + 4; // After "\r\n\r\n"
  size_t body_received = data.size() - body_start;

  logging::Logger::debug("Client fd=" + std::to_string(_fd) +
                         " content_length=" + std::to_string(content_length) +
                         " body_received=" + std::to_string(body_received));

  return body_received >= content_length;
}

} // namespace fion::network
