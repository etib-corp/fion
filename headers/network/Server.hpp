#pragma once

#include "Router.hpp"
#include "network/Listener.hpp"
#include "network/PoolManager.hpp"
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

namespace fion::network {
/**
 * @brief Main server that orchestrates the listener and pool manager
 *
 * This class manages the server lifecycle, accepting connections
 * and distributing them to I/O pools.
 */
class Server {
private:
  Listener _listener;
  PoolManager _poolManager;
  Router *_router;
  std::atomic<bool> _running;
  std::thread _accept_thread;

  /**
   * @brief Accept loop that runs in a separate thread
   */
  void accept_loop();

public:
  /**
   * @brief Construct a new Server object
   *
   * @param router Pointer to the application's router
   */
  explicit Server(Router *router);

  /**
   * @brief Destroy the Server object
   */
  ~Server();

  // Prevent copying
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  // Prevent moving (contains non-movable members)
  Server(Server &&) = delete;
  Server &operator=(Server &&) = delete;

  /**
   * @brief Start the server
   *
   * @param host The hostname or IP address to bind to
   * @param port The port number to listen on
   * @param numThreads The number of I/O threads (pools) to create
   * @throws std::runtime_error if server startup fails
   */
  void start(const std::string &host, std::uint16_t port,
             std::size_t numThreads = 4);

  /**
   * @brief Stop the server
   *
   * Stops accepting new connections and shuts down all I/O pools.
   */
  void stop();

  /**
   * @brief Check if the server is running
   *
   * @return true if the server is running
   * @return false otherwise
   */
  bool is_running() const { return _running.load(); }
};

} // namespace fion::network
