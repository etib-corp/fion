#pragma once

#include "Router.hpp"
#include "network/ConnectionPool.hpp"
#include "network/EventLoop.hpp"
#include <memory>
#include <thread>

namespace fion::network {
/**
 * @brief I/O pool that runs an event loop in its own thread
 *
 * This class encapsulates an EventLoop, ConnectionPool, and a worker thread.
 * Each pool handles I/O for its assigned clients independently.
 */
class Pool {
private:
  EventLoop _loop;
  ConnectionPool _connectionPool;
  std::thread _thread;
  Router *_router; ///< Pointer to the application's router

  /**
   * @brief Handle I/O events for a client
   *
   * @param fd The file descriptor with events
   * @param events The event flags
   */
  void handle_client_event(int fd, uint32_t events);

  /**
   * @brief Process a complete HTTP request
   *
   * @param client The client with a complete request
   */
  void process_request(Client *client);

public:
  /**
   * @brief Construct a new Pool object
   *
   * @param router Pointer to the application's router
   */
  explicit Pool(Router *router);

  /**
   * @brief Destroy the Pool object
   */
  ~Pool();

  // Prevent copying
  Pool(const Pool &) = delete;
  Pool &operator=(const Pool &) = delete;

  // Prevent moving (contains non-movable members)
  Pool(Pool &&) = delete;
  Pool &operator=(Pool &&) = delete;

  /**
   * @brief Start the pool's event loop in a new thread
   */
  void run();

  /**
   * @brief Stop the pool's event loop and join the thread
   */
  void stop();

  /**
   * @brief Add a new client to this pool
   *
   * @param fd The file descriptor for the client socket
   */
  void addClient(int fd);

  /**
   * @brief Get the number of clients in this pool
   *
   * @return size_t The number of active clients
   */
  size_t get_client_count() const { return _connectionPool.size(); }
};

} // namespace fion::network
