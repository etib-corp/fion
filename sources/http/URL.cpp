#include "http/URL.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace fion::http {

// static utility helpers
std::string URL::toLowerCopy(const std::string &s) {
  std::string out = s;
  std::transform(out.begin(), out.end(), out.begin(), ::tolower);
  return out;
}

std::uint16_t URL::defaultPortForScheme(const std::string &scheme) {
  if (scheme == "http")
    return 80;
  if (scheme == "https")
    return 443;
  return 0; // unknown
}

std::uint16_t URL::parsePortNumber(const std::string &portStr) {
  if (portStr.empty())
    throw std::invalid_argument("Invalid port number");
  if (!std::all_of(portStr.begin(), portStr.end(),
                   [](unsigned char ch) { return std::isdigit(ch) != 0; })) {
    throw std::invalid_argument("Invalid port number");
  }
  int portNum = 0;
  try {
    portNum = std::stoi(portStr);
  } catch (const std::exception &) {
    throw std::invalid_argument("Invalid port number");
  }
  if (portNum < 0 || portNum > 65535)
    throw std::invalid_argument("Port number out of range");
  return static_cast<std::uint16_t>(portNum);
}

// member helpers that mutate state
void URL::parseScheme(const std::string &raw, std::size_t &pos) {
  const std::size_t schemeEnd = raw.find("://");
  if (schemeEnd != std::string::npos) {
    scheme = toLowerCopy(raw.substr(0, schemeEnd));
    pos = schemeEnd + 3; // skip "://"
    return;
  }

  // No explicit scheme; if it begins with '/', treat as a path-only URL
  if (!raw.empty() && raw[0] == '/') {
    pos = 0;
    scheme.clear();
    return;
  }

  // Heuristic: treat as authority without scheme; default to http
  scheme = "http";
  pos = 0;
}

void URL::parseFragment(std::string &url, std::size_t startPos) {
  const std::size_t anchorPos = url.find('#', startPos);
  if (anchorPos != std::string::npos) {
    anchor = url.substr(anchorPos + 1);
    url = url.substr(0, anchorPos);
  }
}

void URL::parseQuery(std::string &url, std::size_t startPos) {
  const std::size_t queryPos = url.find('?', startPos);
  if (queryPos == std::string::npos)
    return;

  std::string queryString = url.substr(queryPos + 1);
  url = url.substr(0, queryPos);

  std::istringstream queryStream(queryString);
  std::string param;
  while (std::getline(queryStream, param, '&')) {
    const std::size_t equalPos = param.find('=');
    if (equalPos != std::string::npos) {
      std::string key = param.substr(0, equalPos);
      std::string value = param.substr(equalPos + 1);
      queryParameters[key] = value;
    } else if (!param.empty()) {
      // Parameter without value
      queryParameters[param] = "";
    }
  }
}

void URL::parseAuthorityAndPath(const std::string &url, std::size_t pos) {
  // If it starts with '/', there's no authority; it's a path-only URL
  if (pos < url.length() && url[pos] == '/') {
    pathToResource = url.substr(pos);
    return;
  }

  // Separate authority from path
  const std::size_t pathStart = url.find('/', pos);
  std::string hostPort;
  if (pathStart != std::string::npos) {
    hostPort = url.substr(pos, pathStart - pos);
    pathToResource = url.substr(pathStart);
  } else {
    hostPort = url.substr(pos);
    pathToResource = "/";
  }

  // Strip userinfo if present (user:pass@)
  const std::size_t atPos = hostPort.find('@');
  if (atPos != std::string::npos) {
    hostPort = hostPort.substr(atPos + 1);
  }

  if (hostPort.empty()) {
    domainName.clear();
    port = 0;
    return;
  }

  // IPv6 literal in brackets
  if (hostPort[0] == '[') {
    const std::size_t ipv6End = hostPort.find(']');
    if (ipv6End == std::string::npos)
      throw std::invalid_argument("Malformed IPv6 address");

    domainName = hostPort.substr(0, ipv6End + 1); // include brackets

    // Optional :port after the closing bracket
    if (ipv6End + 1 < hostPort.length() && hostPort[ipv6End + 1] == ':') {
      const std::string portStr = hostPort.substr(ipv6End + 2);
      port = parsePortNumber(portStr);
    } else {
      const std::uint16_t def = defaultPortForScheme(scheme);
      port = (def != 0) ? def : 80; // preserve previous fallback behaviour
    }
    return;
  }

  // IPv4 or hostname: look for last ':' (to avoid issues with possible
  // IPv6-like patterns)
  const std::size_t colonPos = hostPort.rfind(':');
  if (colonPos != std::string::npos) {
    domainName = hostPort.substr(0, colonPos);
    const std::string portStr = hostPort.substr(colonPos + 1);
    port = parsePortNumber(portStr);
  } else {
    domainName = hostPort;
    const std::uint16_t def = defaultPortForScheme(scheme);
    port = (def != 0) ? def : 80; // preserve previous fallback behaviour
  }
}

URL::URL(void)
    : scheme(""), domainName(""), port(0), pathToResource("/"), anchor("") {}

URL::URL(const std::string &rawURL)
    : scheme(""), domainName(""), port(0), pathToResource("/"), anchor("") {
  if (rawURL.empty()) {
    throw std::invalid_argument("URL cannot be empty");
  }

  std::string url = rawURL;
  std::size_t pos = 0;

  // 1) scheme
  parseScheme(url, pos);

  // 2) fragment and 3) query (strip them from the working string, in that
  // order)
  parseFragment(url, pos);
  parseQuery(url, pos);

  // 4) authority (host[:port]) and path
  parseAuthorityAndPath(url, pos);
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
