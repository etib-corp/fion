#include "http/URL.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace fion::http {

URL::URL(void)
    : scheme(""), domainName(""), port(0), pathToResource("/"), anchor("") {}

URL::URL(const std::string &rawURL)
    : scheme(""), domainName(""), port(0), pathToResource("/"), anchor("") {
  if (rawURL.empty()) {
    throw std::invalid_argument("URL cannot be empty");
  }

  std::string url = rawURL;
  size_t pos = 0;

  // Parse scheme (e.g., "http://", "https://")
  size_t schemeEnd = url.find("://");
  if (schemeEnd != std::string::npos) {
    scheme = url.substr(0, schemeEnd);
    // Normalize scheme to lowercase
    std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
    pos = schemeEnd + 3; // Move past "://"
  } else {
    // No scheme found, assume it starts with domain or path
    // Check if it starts with '/' (path only)
    if (url[0] == '/') {
      pos = 0;
    } else {
      // Assume it's a domain without scheme
      scheme = "http"; // Default scheme
      pos = 0;
    }
  }

  // Parse anchor (fragment) - do this early to remove it from processing
  size_t anchorPos = url.find('#', pos);
  if (anchorPos != std::string::npos) {
    anchor = url.substr(anchorPos + 1);
    url = url.substr(0, anchorPos); // Remove anchor from URL
  }

  // Parse query parameters - also do this early
  size_t queryPos = url.find('?', pos);
  std::string queryString;
  if (queryPos != std::string::npos) {
    queryString = url.substr(queryPos + 1);
    url = url.substr(0, queryPos); // Remove query from URL
  }

  // If URL starts with '/', it's just a path (no domain/port)
  if (pos < url.length() && url[pos] == '/') {
    pathToResource = url.substr(pos);
  } else {
    // Parse domain and port
    size_t pathStart = url.find('/', pos);
    std::string hostPort;

    if (pathStart != std::string::npos) {
      hostPort = url.substr(pos, pathStart - pos);
      pathToResource = url.substr(pathStart);
    } else {
      hostPort = url.substr(pos);
      pathToResource = "/";
    }

    // Check for authentication (user:pass@)
    size_t atPos = hostPort.find('@');
    if (atPos != std::string::npos) {
      // Skip authentication part, just take the host:port after @
      hostPort = hostPort.substr(atPos + 1);
    }

    // Check for IPv6 addresses [2001:db8::1]
    if (!hostPort.empty() && hostPort[0] == '[') {
      size_t ipv6End = hostPort.find(']');
      if (ipv6End != std::string::npos) {
        domainName = hostPort.substr(0, ipv6End + 1); // Include brackets

        // Check if there's a port after the IPv6 address
        if (ipv6End + 1 < hostPort.length() && hostPort[ipv6End + 1] == ':') {
          std::string portStr = hostPort.substr(ipv6End + 2);
          try {
            int portNum = std::stoi(portStr);
            if (portNum < 0 || portNum > 65535) {
              throw std::invalid_argument("Port number out of range");
            }
            port = static_cast<std::uint16_t>(portNum);
          } catch (const std::exception &) {
            throw std::invalid_argument("Invalid port number");
          }
        } else {
          // Set default port based on scheme
          if (scheme == "http") {
            port = 80;
          } else if (scheme == "https") {
            port = 443;
          } else {
            port = 80; // Default fallback
          }
        }
      } else {
        throw std::invalid_argument("Malformed IPv6 address");
      }
    } else {
      // Parse port from hostPort (IPv4 or domain)
      size_t portPos =
          hostPort.rfind(':'); // Use rfind to handle IPv6-like patterns
      if (portPos != std::string::npos) {
        domainName = hostPort.substr(0, portPos);
        std::string portStr = hostPort.substr(portPos + 1);

        try {
          int portNum = std::stoi(portStr);
          if (portNum < 0 || portNum > 65535) {
            throw std::invalid_argument("Port number out of range");
          }
          port = static_cast<std::uint16_t>(portNum);
        } catch (const std::exception &) {
          throw std::invalid_argument("Invalid port number");
        }
      } else {
        domainName = hostPort;
        // Set default port based on scheme
        if (scheme == "http") {
          port = 80;
        } else if (scheme == "https") {
          port = 443;
        } else {
          port = 80; // Default fallback
        }
      }
    }
  }

  // Parse query parameters
  if (!queryString.empty()) {
    std::istringstream queryStream(queryString);
    std::string param;

    while (std::getline(queryStream, param, '&')) {
      size_t equalPos = param.find('=');
      if (equalPos != std::string::npos) {
        std::string key = param.substr(0, equalPos);
        std::string value = param.substr(equalPos + 1);
        queryParameters[key] = value;
      } else {
        // Parameter without value
        queryParameters[param] = "";
      }
    }
  }
}

URL::~URL(void) {}

std::string URL::getScheme(void) const { return scheme; }

std::string URL::getDomainName(void) const { return domainName; }

std::uint16_t URL::getPort(void) const { return port; }

std::string URL::getPathToResource(void) const { return pathToResource; }

std::map<std::string, std::string> URL::getQueryParameters(void) const {
  return queryParameters;
}

std::string URL::getAnchor(void) const { return anchor; }

std::string URL::toString(void) const {
  std::ostringstream oss;

  // Add scheme if present
  if (!scheme.empty()) {
    oss << scheme << "://";
  }

  // Add domain if present
  if (!domainName.empty()) {
    oss << domainName;

    // Add port if not default
    bool isDefaultPort =
        (scheme == "http" && port == 80) || (scheme == "https" && port == 443);
    if (port != 0 && !isDefaultPort) {
      oss << ":" << port;
    }
  }

  // Add path
  oss << pathToResource;

  // Add query parameters
  if (!queryParameters.empty()) {
    oss << "?";
    bool first = true;
    for (const auto &param : queryParameters) {
      if (!first) {
        oss << "&";
      }
      oss << param.first;
      if (!param.second.empty()) {
        oss << "=" << param.second;
      }
      first = false;
    }
  }

  // Add anchor
  if (!anchor.empty()) {
    oss << "#" << anchor;
  }

  return oss.str();
}

} // namespace fion::http
