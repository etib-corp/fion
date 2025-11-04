#include "network/Server.hpp"
#include "logging/Logger.hpp"
#include <chrono>
#include <thread>
#include <unistd.h>

namespace fion::network {
Server::Server(Router *router) : _router(router), _running(false) {}

Server::~Server() { stop(); }

void Server::start(const std::string &host, std::uint16_t port,
                   std::size_t numThreads) {
  if (_running.exchange(true))
    return; // Already running

  // Bind and listen
  _listener.bind(host, port);
  _listener.listen();

  logging::Logger::info("Server listening on " + host + ":" +
                        std::to_string(port));

  // Create I/O pools
  for (std::size_t i = 0; i < numThreads; ++i) {
    auto pool = std::make_unique<Pool>(_router);
    _poolManager.add_pool(std::move(pool));
  }

  // Start all pools
  _poolManager.start_all();

  // Start accept thread
  _accept_thread = std::thread([this]() { accept_loop(); });
}

void Server::stop() {
  if (!_running.exchange(false))
    return; // Not running

  logging::Logger::info("Stopping server...");

  // Stop accepting new connections
  _listener.close();

  // Wait for accept thread
  if (_accept_thread.joinable())
    _accept_thread.join();

  // Stop all pools
  _poolManager.stop_all();

  logging::Logger::info("Server stopped");
}

void Server::accept_loop() {
  while (_running.load()) {
    int client_fd = _listener.acceptClient();

    if (client_fd >= 0) {
      try {
        _poolManager.distribute_client(client_fd);
      } catch (const std::exception &e) {
        logging::Logger::error(std::string("Failed to distribute client: ") +
                               e.what());
        ::close(client_fd);
      }
    } else {
      // No connection available, sleep briefly
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

} // namespace fion::network
