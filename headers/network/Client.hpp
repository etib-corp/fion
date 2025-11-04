#pragma once

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "network/Buffer.hpp"
#include <memory>
#include <mutex>
#include <vector>

namespace fion::network {
/**
 * @brief Client connection state
 */
enum class ClientState {
  READING_REQUEST,  ///< Currently reading the request
  PROCESSING,       ///< Processing the request
  WRITING_RESPONSE, ///< Writing the response
  CLOSED            ///< Connection closed
};

/**
 * @brief Represents a single client connection
 *
 * This class manages a client connection including its socket,
 * request/response buffers, and connection state.
 */
class Client {
private:
  int _fd;                   ///< File descriptor for the client socket
  Buffer _requestBuffer;     ///< Buffer for incoming request data
  Buffer _responseBuffer;    ///< Buffer for outgoing response data
  ClientState _state;        ///< Current connection state
  mutable std::mutex _mutex; ///< Mutex for thread-safe operations

public:
  /**
   * @brief Construct a new Client object
   *
   * @param fd The file descriptor for the client socket
   */
  explicit Client(int fd);

  /**
   * @brief Destroy the Client object
   */
  ~Client();

  // Prevent copying
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;

  // Allow moving
  Client(Client &&) noexcept;
  Client &operator=(Client &&) noexcept;

  /**
   * @brief Read data from the client socket into the request buffer
   *
   * @return ssize_t Number of bytes read, 0 if connection closed, -1 on error
   */
  ssize_t readRequest();

  /**
   * @brief Write response data from the response buffer to the client socket
   *
   * @return ssize_t Number of bytes written, -1 on error
   */
  ssize_t writeResponse();

  /**
   * @brief Prepare response data from a Response object
   *
   * @param response The HTTP response to send
   */
  void prepare_response(const http::Response &response);

  /**
   * @brief Check if the request is complete and ready for processing
   *
   * @return true if the request is complete
   * @return false otherwise
   */
  bool is_request_ready() const;

  /**
   * @brief Get the file descriptor
   *
   * @return int The client socket file descriptor
   */
  int get_fd() const { return _fd; }

  /**
   * @brief Get the current client state
   *
   * @return ClientState The current state
   */
  ClientState get_state() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _state;
  }

  /**
   * @brief Set the client state
   *
   * @param state The new state
   */
  void set_state(ClientState state) {
    std::lock_guard<std::mutex> lock(_mutex);
    _state = state;
  }

  /**
   * @brief Get the request buffer data
   *
   * @return std::string_view View of the request buffer
   */
  std::string_view get_request_data() const { return _requestBuffer.getData(); }

  /**
   * @brief Clear the request buffer
   */
  void clear_request_buffer() { _requestBuffer.clear(); }

  /**
   * @brief Clear the response buffer
   */
  void clear_response_buffer() { _responseBuffer.clear(); }
};

} // namespace fion::network
