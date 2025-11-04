#include "network/EventLoop.hpp"
#include "logging/Logger.hpp"
#include <stdexcept>

namespace fion::network {
EventLoop::EventLoop() : _running(false) {}

EventLoop::~EventLoop() { stop(); }

void EventLoop::run() {
  if (_running.exchange(true))
    return; // Already running

  logging::Logger::debug("EventLoop: started");
  while (_running.load()) {
    try {
      // Poll for events with a timeout to allow checking _running flag
      auto events = _poller.poll(100); // 100ms timeout
      if (!events.empty()) {
        logging::Logger::debug("EventLoop: polled events=" +
                               std::to_string(events.size()));
      }
      for (const auto &event : events) {
        if (_event_callback) {
          _event_callback(event.fd, event.events);
        }
      }
    } catch (const std::exception &e) {
      // Log error but continue running
      logging::Logger::error(std::string("EventLoop: exception: ") + e.what());
    }
  }
  logging::Logger::debug("EventLoop: stopped");
}

void EventLoop::stop() { _running.store(false); }

} // namespace fion::network
