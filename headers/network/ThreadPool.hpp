#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace fion::network {
/**
 * @brief Thread pool for executing handler tasks
 *
 * This optional component offloads CPU-bound handler execution
 * to worker threads, preventing blocking of I/O threads.
 */
class ThreadPool {
private:
  std::vector<std::thread> _threads;
  std::queue<std::function<void()>> _tasks;
  mutable std::mutex _mutex; // mutable to allow locking in const methods
  std::condition_variable _cv;
  std::atomic<bool> _stop;

  /**
   * @brief Worker thread loop
   *
   * Continuously fetches and executes tasks from the queue.
   */
  void worker_loop();

public:
  /**
   * @brief Construct a new Thread Pool object
   *
   * @param numThreads The number of worker threads to create
   */
  explicit ThreadPool(size_t numThreads);

  /**
   * @brief Destroy the Thread Pool object
   *
   * Stops all threads and waits for them to finish.
   */
  ~ThreadPool();

  // Prevent copying
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  // Prevent moving (due to thread ownership)
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  /**
   * @brief Enqueue a task for execution
   *
   * @tparam F The function type
   * @tparam Args The argument types
   * @param f The function to execute
   * @param args The arguments to pass to the function
   */
  template <typename F, typename... Args> void enqueue(F &&f, Args &&...args) {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      if (_stop)
        return;

      _tasks.emplace([f = std::forward<F>(f),
                      ... args = std::forward<Args>(args)]() mutable {
        f(std::forward<Args>(args)...);
      });
    }
    _cv.notify_one();
  }

  /**
   * @brief Get the number of worker threads
   *
   * @return size_t The number of threads in the pool
   */
  size_t thread_count() const { return _threads.size(); }

  /**
   * @brief Get the number of pending tasks
   *
   * @return size_t The number of tasks in the queue
   */
  size_t pending_tasks() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _tasks.size();
  }
};

} // namespace fion::network
