#pragma once

#include <cstdint>
#include <vector>

// Platform-specific includes
#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#else
#include <sys/epoll.h>
#endif

namespace fion::network {
/**
 * @brief Event types for the poller
 */
enum class PollerEvent : uint32_t {
#ifdef __APPLE__
  READ = 0x0001,          ///< Socket is ready for reading
  WRITE = 0x0002,         ///< Socket is ready for writing
  ERROR = 0x0004,         ///< Error on socket
  HANGUP = 0x0008,        ///< Hangup on socket
  EDGE_TRIGGERED = 0x0010 ///< Edge-triggered mode
#else
  READ = EPOLLIN,          ///< Socket is ready for reading
  WRITE = EPOLLOUT,        ///< Socket is ready for writing
  ERROR = EPOLLERR,        ///< Error on socket
  HANGUP = EPOLLHUP,       ///< Hangup on socket
  EDGE_TRIGGERED = EPOLLET ///< Edge-triggered mode
#endif
};

/**
 * @brief Platform-agnostic event structure
 */
struct PollerEventData {
  int fd;          ///< File descriptor
  uint32_t events; ///< Event flags
};

/**
 * @brief Wrapper for platform-specific I/O multiplexing
 *
 * This class provides a cross-platform abstraction over epoll (Linux)
 * and kqueue (macOS/BSD) for monitoring multiple file descriptors for I/O
 * events.
 */
class Poller {
private:
#ifdef __APPLE__
  int _kqueue_fd; ///< File descriptor for kqueue instance
#else
  int _epoll_fd; ///< File descriptor for epoll instance
#endif

public:
  /**
   * @brief Construct a new Poller object
   *
   * @throws std::runtime_error if epoll creation fails
   */
  Poller();

  /**
   * @brief Destroy the Poller object
   */
  ~Poller();

  // Prevent copying
  Poller(const Poller &) = delete;
  Poller &operator=(const Poller &) = delete;

  // Allow moving
  Poller(Poller &&other) noexcept;
  Poller &operator=(Poller &&other) noexcept;

  /**
   * @brief Add a file descriptor to the poller
   *
   * @param fd The file descriptor to monitor
   * @param events Bitmask of events to monitor (use PollerEvent flags)
   * @throws std::runtime_error if adding the fd fails
   */
  void addFD(int fd, uint32_t events);

  /**
   * @brief Modify events for a file descriptor
   *
   * @param fd The file descriptor to modify
   * @param events New bitmask of events to monitor
   * @throws std::runtime_error if modifying fails
   */
  void modify_fd(int fd, uint32_t events);

  /**
   * @brief Remove a file descriptor from the poller
   *
   * @param fd The file descriptor to remove
   * @throws std::runtime_error if removing the fd fails
   */
  void removeFD(int fd);

  /**
   * @brief Wait for events on monitored file descriptors
   *
   * @param timeout_ms Timeout in milliseconds (-1 for infinite)
   * @return std::vector<PollerEventData> Vector of events that occurred
   */
  std::vector<PollerEventData> poll(int timeout_ms = -1);

  /**
   * @brief Get the poller file descriptor
   *
   * @return int The poller file descriptor
   */
  int get_fd() const {
#ifdef __APPLE__
    return _kqueue_fd;
#else
    return _epoll_fd;
#endif
  }
};

} // namespace fion::network
