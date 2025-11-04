#pragma once

#include <cstring>
#include <mutex>
#include <string_view>
#include <vector>

namespace fion::network {
/**
 * @brief Thread-safe buffer for storing raw bytes
 *
 * This class provides a thread-safe container for storing and managing
 * raw byte data, typically used for request/response buffering.
 */
class Buffer {
private:
  std::vector<char> _data;
  mutable std::mutex _mutex;

public:
  /**
   * @brief Construct a new Buffer object
   */
  Buffer() = default;

  /**
   * @brief Destroy the Buffer object
   */
  ~Buffer() = default;

  // Prevent copying
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  // Prevent moving (mutex cannot be moved)
  Buffer(Buffer &&) = delete;
  Buffer &operator=(Buffer &&) = delete;

  /**
   * @brief Append data to the buffer
   *
   * @param data Pointer to the data to append
   * @param len Length of the data in bytes
   */
  void append(const char *data, size_t len) {
    std::lock_guard<std::mutex> lock(_mutex);
    _data.insert(_data.end(), data, data + len);
  }

  /**
   * @brief Clear all data from the buffer
   */
  void clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _data.clear();
  }

  /**
   * @brief Get a view of the buffer data
   *
   * @return std::string_view A view of the buffer contents
   */
  std::string_view getData() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return std::string_view(_data.data(), _data.size());
  }

  /**
   * @brief Get the size of the buffer
   *
   * @return size_t The number of bytes in the buffer
   */
  size_t size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _data.size();
  }

  /**
   * @brief Check if the buffer is empty
   *
   * @return true if the buffer is empty
   * @return false otherwise
   */
  bool empty() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _data.empty();
  }
};

} // namespace fion::network
