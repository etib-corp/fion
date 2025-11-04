#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <string>

namespace fion::network {
/**
 * @brief Listens for and accepts incoming client connections
 *
 * This class manages a listening socket that binds to a host and port
 * and accepts new client connections.
 */
class Listener {
private:
  int _listenFD;        ///< File descriptor for the listening socket
  sockaddr_in _address; ///< Server address structure
  bool _is_listening;   ///< Whether the listener is active

public:
  /**
   * @brief Construct a new Listener object
   */
  Listener();

  /**
   * @brief Destroy the Listener object
   */
  ~Listener();

  // Prevent copying
  Listener(const Listener &) = delete;
  Listener &operator=(const Listener &) = delete;

  // Allow moving
  Listener(Listener &&other) noexcept;
  Listener &operator=(Listener &&other) noexcept;

  /**
   * @brief Bind the listener to a host and port
   *
   * @param host The hostname or IP address to bind to
   * @param port The port number to bind to
   * @throws std::runtime_error if binding fails
   */
  void bind(const std::string &host, std::uint16_t port);

  /**
   * @brief Start listening for connections
   *
   * @param backlog The maximum length of the queue of pending connections
   * @throws std::runtime_error if listen fails
   */
  void listen(int backlog = 128);

  /**
   * @brief Accept a new client connection
   *
   * @return int File descriptor for the new client, or -1 on error
   */
  int acceptClient();

  /**
   * @brief Get the listening file descriptor
   *
   * @return int The listening socket file descriptor
   */
  int get_fd() const { return _listenFD; }

  /**
   * @brief Check if the listener is active
   *
   * @return true if listening
   * @return false otherwise
   */
  bool is_listening() const { return _is_listening; }

  /**
   * @brief Close the listening socket
   */
  void close();
};

} // namespace fion::network
