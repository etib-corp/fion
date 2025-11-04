#pragma once

#include "network/Pool.hpp"
#include <memory>
#include <mutex>
#include <vector>

namespace fion::network {
/**
 * @brief Manages multiple I/O pools and distributes clients among them
 *
 * This class coordinates multiple Pool instances, distributing new
 * client connections using a round-robin strategy.
 */
class PoolManager {
private:
  std::vector<std::unique_ptr<Pool>> _pools;
  mutable std::mutex _mutex;
  size_t _nextPoolIndex;

public:
  /**
   * @brief Construct a new Pool Manager object
   */
  PoolManager();

  /**
   * @brief Destroy the Pool Manager object
   */
  ~PoolManager();

  // Prevent copying
  PoolManager(const PoolManager &) = delete;
  PoolManager &operator=(const PoolManager &) = delete;

  // Prevent moving (mutex cannot be moved)
  PoolManager(PoolManager &&) = delete;
  PoolManager &operator=(PoolManager &&) = delete;

  /**
   * @brief Add a new pool to the manager
   *
   * @param pool Unique pointer to the pool to add
   */
  void add_pool(std::unique_ptr<Pool> pool);

  /**
   * @brief Get the next pool for load balancing
   *
   * Uses round-robin selection to distribute load evenly.
   *
   * @return Pool* Pointer to the selected pool, or nullptr if no pools exist
   */
  Pool *get_pool();

  /**
   * @brief Distribute a client to the next available pool
   *
   * @param fd The file descriptor for the client socket
   */
  void distribute_client(int fd);

  /**
   * @brief Get the number of pools
   *
   * @return size_t The number of pools managed
   */
  size_t pool_count() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _pools.size();
  }

  /**
   * @brief Start all pools
   */
  void start_all();

  /**
   * @brief Stop all pools
   */
  void stop_all();
};

} // namespace fion::network
