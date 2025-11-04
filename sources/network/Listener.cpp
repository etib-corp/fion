#include "network/Listener.hpp"
#include "logging/Logger.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

namespace fion::network {
Listener::Listener() : _listenFD(-1), _is_listening(false) {
  std::memset(&_address, 0, sizeof(_address));
}

Listener::~Listener() { close(); }

Listener::Listener(Listener &&other) noexcept
    : _listenFD(other._listenFD), _address(other._address),
      _is_listening(other._is_listening) {
  other._listenFD = -1;
  other._is_listening = false;
}

Listener &Listener::operator=(Listener &&other) noexcept {
  if (this != &other) {
    close();

    _listenFD = other._listenFD;
    _address = other._address;
    _is_listening = other._is_listening;

    other._listenFD = -1;
    other._is_listening = false;
  }
  return *this;
}

void Listener::bind(const std::string &host, std::uint16_t port) {
  logging::Logger::info("Binding listener to " + host + ":" +
                        std::to_string(port));
  // Create socket
  _listenFD = ::socket(AF_INET, SOCK_STREAM, 0);
  if (_listenFD < 0)
    throw std::runtime_error("Failed to create socket: " +
                             std::string(std::strerror(errno)));

  // Set socket options
  int opt = 1;
  if (::setsockopt(_listenFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    ::close(_listenFD);
    throw std::runtime_error("Failed to set SO_REUSEADDR: " +
                             std::string(std::strerror(errno)));
  }

  // Set non-blocking
  int flags = ::fcntl(_listenFD, F_GETFL, 0);
  if (flags < 0 || ::fcntl(_listenFD, F_SETFL, flags | O_NONBLOCK) < 0) {
    ::close(_listenFD);
    throw std::runtime_error("Failed to set non-blocking: " +
                             std::string(std::strerror(errno)));
  }

  // Prepare address
  _address.sin_family = AF_INET;
  _address.sin_port = htons(port);

  if (host == "0.0.0.0" || host.empty()) {
    _address.sin_addr.s_addr = INADDR_ANY;
  } else {
    if (::inet_pton(AF_INET, host.c_str(), &_address.sin_addr) <= 0) {
      ::close(_listenFD);
      throw std::runtime_error("Invalid address: " + host);
    }
  }

  // Bind
  if (::bind(_listenFD, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
    ::close(_listenFD);
    throw std::runtime_error("Failed to bind to " + host + ":" +
                             std::to_string(port) + ": " +
                             std::string(std::strerror(errno)));
  }
}

void Listener::listen(int backlog) {
  if (_listenFD < 0)
    throw std::runtime_error("Socket not bound");

  if (::listen(_listenFD, backlog) < 0)
    throw std::runtime_error("Failed to listen: " +
                             std::string(std::strerror(errno)));

  _is_listening = true;
  logging::Logger::info("Listener is now listening");
}

int Listener::acceptClient() {
  if (!_is_listening)
    return -1;

  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);

  int client_fd =
      ::accept(_listenFD, (struct sockaddr *)&client_addr, &client_len);

  if (client_fd >= 0) {
    // Set non-blocking
    int flags = ::fcntl(client_fd, F_GETFL, 0);
    if (flags >= 0)
      ::fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    logging::Logger::debug("Accepted client fd=" + std::to_string(client_fd));
  }

  return client_fd;
}

void Listener::close() {
  if (_listenFD >= 0) {
    ::close(_listenFD);
    _listenFD = -1;
    _is_listening = false;
  }
}

} // namespace fion::network
