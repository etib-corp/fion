#pragma once

#include <string>

namespace fion::http {
/**
 * @brief HTTP protocol versions
 *
 * Enumeration of supported HTTP protocol versions
 */
enum class Version {
  HTTP_0_9, ///< Represents HTTP version 0.9
  HTTP_1_0, ///< Represents HTTP version 1.0
  HTTP_1_1, ///< Represents HTTP version 1.1
  HTTP_2_0, ///< Represents HTTP version 2.0
  HTTP_3_0  ///< Represents HTTP version 3.0
};

/**
 * @brief Convert a string to an HTTP Version enum
 *
 * @param rawVersion The HTTP version as a string (e.g., "HTTP/1.1")
 * @return The corresponding Version enum value
 * @throws std::invalid_argument if the version string is not recognized
 */
const Version stringToVersion(const std::string &rawVersion);

/**
 * @brief Convert an HTTP Version enum to its string representation
 *
 * @param Version The HTTP version enum
 * @return The string representation (e.g., "HTTP/1.1")
 */
const std::string VersionToString(const Version Version);
} // namespace fion::http
