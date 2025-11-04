#include "network/ConnectionPool.hpp"
#include "logging/Logger.hpp"
#include <stdexcept>

namespace fion::network {
void ConnectionPool::addClient(int fd) {
  std::lock_guard<std::mutex> lock(_mutex);
  _clients[fd] = std::make_unique<Client>(fd);
  logging::Logger::debug("ConnectionPool: added client fd=" +
                         std::to_string(fd));
}

void ConnectionPool::removeClient(int fd) {
  std::lock_guard<std::mutex> lock(_mutex);
  _clients.erase(fd);
  logging::Logger::debug("ConnectionPool: removed client fd=" +
                         std::to_string(fd));
}

Client *ConnectionPool::getClient(int fd) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto it = _clients.find(fd);
  if (it != _clients.end())
    return it->second.get();
  return nullptr;
}

} // namespace fion::network
