#pragma once

#include "network/Poller.hpp"
#include <atomic>
#include <functional>
#include <memory>

namespace fion::network {
// Forward declaration
class ConnectionPool;

/**
 * @brief Event callback function type
 *
 * The callback receives the file descriptor and event flags
 */
using EventCallback = std::function<void(int fd, uint32_t events)>;

/**
 * @brief Event loop for processing I/O events
 *
 * This class runs an event loop that monitors file descriptors
 * using a Poller and dispatches events to registered callbacks.
 */
class EventLoop {
private:
  Poller _poller;
  std::atomic<bool> _running;
  EventCallback _event_callback;

public:
  /**
   * @brief Construct a new Event Loop object
   */
  EventLoop();

  /**
   * @brief Destroy the Event Loop object
   */
  ~EventLoop();

  // Prevent copying
  EventLoop(const EventLoop &) = delete;
  EventLoop &operator=(const EventLoop &) = delete;

  // Prevent moving (atomic cannot be moved)
  EventLoop(EventLoop &&) = delete;
  EventLoop &operator=(EventLoop &&) = delete;

  /**
   * @brief Set the event callback function
   *
   * @param callback The function to call when events occur
   */
  void set_event_callback(EventCallback callback) {
    _event_callback = std::move(callback);
  }

  /**
   * @brief Run the event loop
   *
   * This method blocks until stop() is called, processing I/O events
   * in a loop and invoking the registered callback for each event.
   */
  void run();

  /**
   * @brief Stop the event loop
   *
   * Signals the event loop to stop after the current iteration.
   */
  void stop();

  /**
   * @brief Check if the event loop is running
   *
   * @return true if the loop is running
   * @return false otherwise
   */
  bool is_running() const { return _running.load(); }

  /**
   * @brief Get the poller instance
   *
   * @return Poller& Reference to the poller
   */
  Poller &get_poller() { return _poller; }
};

} // namespace fion::network
