#include "network/Poller.hpp"
#include "logging/Logger.hpp"
#include <cstring>
#include <stdexcept>
#include <unistd.h>

namespace fion::network {
#ifdef __APPLE__
// macOS/BSD implementation using kqueue

Poller::Poller() : _kqueue_fd(::kqueue()) {
  if (_kqueue_fd < 0)
    throw std::runtime_error("Failed to create kqueue instance: " +
                             std::string(std::strerror(errno)));
}

Poller::~Poller() {
  if (_kqueue_fd >= 0) {
    ::close(_kqueue_fd);
    _kqueue_fd = -1;
  }
}

Poller::Poller(Poller &&other) noexcept : _kqueue_fd(other._kqueue_fd) {
  other._kqueue_fd = -1;
}

Poller &Poller::operator=(Poller &&other) noexcept {
  if (this != &other) {
    if (_kqueue_fd >= 0)
      ::close(_kqueue_fd);

    _kqueue_fd = other._kqueue_fd;
    other._kqueue_fd = -1;
  }
  return *this;
}

void Poller::addFD(int fd, uint32_t events) {
  struct kevent kev[2];
  int n = 0;

  if (events & static_cast<uint32_t>(PollerEvent::READ)) {
    EV_SET(&kev[n++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
  }

  if (events & static_cast<uint32_t>(PollerEvent::WRITE)) {
    EV_SET(&kev[n++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
  }

  if (n > 0 && ::kevent(_kqueue_fd, kev, n, nullptr, 0, nullptr) < 0) {
    throw std::runtime_error("Failed to add fd to kqueue: " +
                             std::string(std::strerror(errno)));
  }
}

void Poller::modify_fd(int fd, uint32_t events) {
  // For kqueue, we need to delete old filters and add new ones
  struct kevent kev[4];
  int n = 0;

  // Delete existing filters
  EV_SET(&kev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
  EV_SET(&kev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

  // Add new filters based on events
  if (events & static_cast<uint32_t>(PollerEvent::READ)) {
    EV_SET(&kev[n++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
  }

  if (events & static_cast<uint32_t>(PollerEvent::WRITE)) {
    EV_SET(&kev[n++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, nullptr);
  }

  logging::Logger::debug("Poller: modify_fd fd=" + std::to_string(fd) +
                         " new_events=" + std::to_string(events));
  ::kevent(_kqueue_fd, kev, n, nullptr, 0, nullptr);
}

void Poller::removeFD(int fd) {
  struct kevent kev[2];

  EV_SET(&kev[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
  EV_SET(&kev[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

  ::kevent(_kqueue_fd, kev, 2, nullptr, 0, nullptr);
}

std::vector<PollerEventData> Poller::poll(int timeout_ms) {
  const int max_events = 64;
  struct kevent events[max_events];

  struct timespec timeout;
  struct timespec *timeout_ptr = nullptr;

  if (timeout_ms >= 0) {
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_nsec = (timeout_ms % 1000) * 1000000;
    timeout_ptr = &timeout;
  }

  int num_events =
      ::kevent(_kqueue_fd, nullptr, 0, events, max_events, timeout_ptr);

  if (num_events < 0) {
    if (errno == EINTR)
      return {}; // Interrupted, return empty
    throw std::runtime_error("kevent failed: " +
                             std::string(std::strerror(errno)));
  }

  std::vector<PollerEventData> result;
  result.reserve(num_events);

  for (int i = 0; i < num_events; ++i) {
    PollerEventData event_data;
    event_data.fd = static_cast<int>(events[i].ident);
    event_data.events = 0;

    if (events[i].filter == EVFILT_READ)
      event_data.events |= static_cast<uint32_t>(PollerEvent::READ);

    if (events[i].filter == EVFILT_WRITE)
      event_data.events |= static_cast<uint32_t>(PollerEvent::WRITE);

    if (events[i].flags & EV_ERROR)
      event_data.events |= static_cast<uint32_t>(PollerEvent::ERROR);

    if (events[i].flags & EV_EOF)
      event_data.events |= static_cast<uint32_t>(PollerEvent::HANGUP);

    result.push_back(event_data);
  }

  return result;
}

#else
// Linux implementation using epoll
Poller::Poller() : _epoll_fd(::epoll_create1(0)) {
  if (_epoll_fd < 0)
    throw std::runtime_error("Failed to create epoll instance: " +
                             std::string(std::strerror(errno)));
}

Poller::~Poller() {
  if (_epoll_fd >= 0) {
    ::close(_epoll_fd);
    _epoll_fd = -1;
  }
}

Poller::Poller(Poller &&other) noexcept : _epoll_fd(other._epoll_fd) {
  other._epoll_fd = -1;
}

Poller &Poller::operator=(Poller &&other) noexcept {
  if (this != &other) {
    if (_epoll_fd >= 0)
      ::close(_epoll_fd);

    _epoll_fd = other._epoll_fd;
    other._epoll_fd = -1;
  }
  return *this;
}

void Poller::addFD(int fd, uint32_t events) {
  epoll_event ev{};
  ev.events = events;
  ev.data.fd = fd;

  if (::epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
    throw std::runtime_error("Failed to add fd to epoll: " +
                             std::string(std::strerror(errno)));
}

void Poller::modify_fd(int fd, uint32_t events) {
  epoll_event ev{};
  ev.events = events;
  ev.data.fd = fd;

  if (::epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
    throw std::runtime_error("Failed to modify fd in epoll: " +
                             std::string(std::strerror(errno)));
}

void Poller::removeFD(int fd) {
  if (::epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) < 0)
    throw std::runtime_error("Failed to remove fd from epoll: " +
                             std::string(std::strerror(errno)));
}

std::vector<PollerEventData> Poller::poll(int timeout_ms) {
  const int max_events = 64;
  epoll_event events[max_events];

  int num_events = ::epoll_wait(_epoll_fd, events, max_events, timeout_ms);

  if (num_events < 0) {
    if (errno == EINTR)
      return {}; // Interrupted, return empty
    throw std::runtime_error("epoll_wait failed: " +
                             std::string(std::strerror(errno)));
  }

  std::vector<PollerEventData> result;
  result.reserve(num_events);

  for (int i = 0; i < num_events; ++i) {
    PollerEventData event_data;
    event_data.fd = events[i].data.fd;
    event_data.events = events[i].events;
    result.push_back(event_data);
  }

  return result;
}
#endif

} // namespace fion::network
