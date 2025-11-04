#pragma once

#include "network/Client.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>

namespace fion::network {
/**
 * @brief Thread-safe pool for managing active client connections
 *
 * This class maintains a collection of active client connections
 * and provides thread-safe access to them.
 */
class ConnectionPool {
private:
  std::unordered_map<int, std::unique_ptr<Client>> _clients;
  mutable std::mutex _mutex;

public:
  /**
   * @brief Construct a new Connection Pool object
   */
  ConnectionPool() = default;

  /**
   * @brief Destroy the Connection Pool object
   */
  ~ConnectionPool() = default;

  // Prevent copying
  ConnectionPool(const ConnectionPool &) = delete;
  ConnectionPool &operator=(const ConnectionPool &) = delete;

  // Prevent moving (mutex cannot be moved)
  ConnectionPool(ConnectionPool &&) = delete;
  ConnectionPool &operator=(ConnectionPool &&) = delete;

  /**
   * @brief Add a new client to the pool
   *
   * @param fd The file descriptor for the client socket
   */
  void addClient(int fd);

  /**
   * @brief Remove a client from the pool
   *
   * @param fd The file descriptor of the client to remove
   */
  void removeClient(int fd);

  /**
   * @brief Get a client by file descriptor
   *
   * @param fd The file descriptor of the client
   * @return Client* Pointer to the client, or nullptr if not found
   */
  Client *getClient(int fd);

  /**
   * @brief Get the number of active clients
   *
   * @return size_t The number of clients in the pool
   */
  size_t size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _clients.size();
  }

  /**
   * @brief Check if the pool is empty
   *
   * @return true if there are no clients
   * @return false otherwise
   */
  bool empty() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _clients.empty();
  }
};

} // namespace fion::network
