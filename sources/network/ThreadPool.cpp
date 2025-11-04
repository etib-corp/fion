#include "network/ThreadPool.hpp"

namespace fion::network {
ThreadPool::ThreadPool(size_t numThreads) : _stop(false) {
  for (size_t i = 0; i < numThreads; ++i) {
    _threads.emplace_back([this]() { worker_loop(); });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _stop = true;
  }
  _cv.notify_all();

  for (auto &thread : _threads) {
    if (thread.joinable())
      thread.join();
  }
}

void ThreadPool::worker_loop() {
  while (true) {
    std::function<void()> task;

    {
      std::unique_lock<std::mutex> lock(_mutex);
      _cv.wait(lock, [this]() { return _stop || !_tasks.empty(); });

      if (_stop && _tasks.empty())
        return;

      if (!_tasks.empty()) {
        task = std::move(_tasks.front());
        _tasks.pop();
      }
    }

    if (task)
      task();
  }
}

} // namespace fion::network
