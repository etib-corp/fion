#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace fion::http {
/**
 * @brief Represents a Uniform Resource Locator (URL)
 *
 * This class parses and stores the components of a URL including
 * scheme, domain name, port, path, query parameters, and anchor.
 */
class URL {
private:
  std::string scheme;         ///< The URL scheme (e.g., "http", "https")
  std::string domainName;     ///< The domain name or IP address
  std::uint16_t port;         ///< The port number
  std::string pathToResource; ///< The path to the resource
  std::map<std::string, std::string>
      queryParameters; ///< Query parameters as key-value pairs
  std::string anchor;  ///< The URL fragment/anchor

  // Internal parsing helpers (private); some are static utilities while others
  // mutate the instance state.
  static std::string toLowerCopy(const std::string &s);
  static std::uint16_t defaultPortForScheme(const std::string &scheme);
  static std::uint16_t parsePortNumber(const std::string &portStr);
  void parseScheme(const std::string &raw, std::size_t &pos);
  void parseFragment(std::string &url, std::size_t startPos);
  void parseQuery(std::string &url, std::size_t startPos);
  void parseAuthorityAndPath(const std::string &url, std::size_t pos);

public:
  /**
   * @brief Construct an empty URL object
   */
  URL(void);

  /**
   * @brief Construct a URL object by parsing a raw URL string
   *
   * @param rawURL The raw URL string to parse
   * @throws std::invalid_argument if the URL format is invalid
   */
  URL(const std::string &rawURL);

  /**
   * @brief Destroy the URL object
   */
  ~URL(void);

  /**
   * @brief Get the URL scheme
   *
   * @return The scheme (e.g., "http", "https")
   */
  std::string getScheme(void) const;

  /**
   * @brief Get the domain name
   *
   * @return The domain name or IP address
   */
  std::string getDomainName(void) const;

  /**
   * @brief Get the port number
   *
   * @return The port number
   */
  std::uint16_t getPort(void) const;

  /**
   * @brief Get the path to the resource
   *
   * @return The resource path
   */
  std::string getPathToResource(void) const;

  /**
   * @brief Get the query parameters
   *
   * @return A map of query parameters as key-value pairs
   */
  std::map<std::string, std::string> getQueryParameters(void) const;

  /**
   * @brief Get the URL anchor/fragment
   *
   * @return The anchor string
   */
  std::string getAnchor(void) const;

  /**
   * @brief Convert the URL object back to its string representation
   *
   * @return The full URL as a string
   */
  std::string toString(void) const;
};

} // namespace fion::http
