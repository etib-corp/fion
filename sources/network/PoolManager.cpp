#include "network/PoolManager.hpp"
#include "logging/Logger.hpp"
#include <stdexcept>

namespace fion::network {
PoolManager::PoolManager() : _nextPoolIndex(0) {}

PoolManager::~PoolManager() { stop_all(); }

void PoolManager::add_pool(std::unique_ptr<Pool> pool) {
  std::lock_guard<std::mutex> lock(_mutex);
  _pools.push_back(std::move(pool));
  logging::Logger::debug("PoolManager: added pool. total=" +
                         std::to_string(_pools.size()));
}

Pool *PoolManager::get_pool() {
  std::lock_guard<std::mutex> lock(_mutex);

  if (_pools.empty())
    return nullptr;

  // Round-robin selection
  size_t index = _nextPoolIndex % _pools.size();
  _nextPoolIndex = (_nextPoolIndex + 1) % _pools.size();

  auto *selected = _pools[index].get();
  logging::Logger::debug("PoolManager: selected pool index=" +
                         std::to_string(index));
  return selected;
}

void PoolManager::distribute_client(int fd) {
  Pool *pool = get_pool();
  if (pool) {
    pool->addClient(fd);
    logging::Logger::debug("PoolManager: distributed client fd=" +
                           std::to_string(fd));
  } else {
    throw std::runtime_error("No pools available to distribute client");
  }
}

void PoolManager::start_all() {
  std::lock_guard<std::mutex> lock(_mutex);
  for (auto &pool : _pools) {
    pool->run();
  }
  logging::Logger::info("PoolManager: started all pools count=" +
                        std::to_string(_pools.size()));
}

void PoolManager::stop_all() {
  std::lock_guard<std::mutex> lock(_mutex);
  for (auto &pool : _pools) {
    pool->stop();
  }
  logging::Logger::info("PoolManager: stopped all pools");
}

} // namespace fion::network
