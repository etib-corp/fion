#include "logging/Logger.hpp"
#include <cstdio>
#include <cstring>

namespace fion::logging {

static std::mutex g_logger_mutex;
static LogLevel g_current_level = LogLevel::Debug; // default verbose
static bool g_initialized = false;

void Logger::init(const std::string &ident, int facility, bool logToStderr) {
  std::lock_guard<std::mutex> lock(g_logger_mutex);
  if (g_initialized)
    return;

  int options = LOG_PID;
  if (logToStderr)
    options |= LOG_PERROR;
  ::openlog(ident.c_str(), options, facility);
  ::setlogmask(to_mask(g_current_level));
  g_initialized = true;
}

void Logger::shutdown() {
  std::lock_guard<std::mutex> lock(g_logger_mutex);
  if (!g_initialized)
    return;
  ::closelog();
  g_initialized = false;
}

void Logger::set_level(LogLevel level) {
  std::lock_guard<std::mutex> lock(g_logger_mutex);
  g_current_level = level;
  ::setlogmask(to_mask(level));
}

LogLevel Logger::level() { return g_current_level; }

void Logger::log(LogLevel level, const std::string &message) {
  // Fast-path drop if below threshold
  if (static_cast<int>(level) > static_cast<int>(g_current_level))
    return;
  ::syslog(to_priority(level), "%s", message.c_str());
}

int Logger::to_priority(LogLevel level) {
  switch (level) {
  case LogLevel::Emergency:
    return LOG_EMERG;
  case LogLevel::Alert:
    return LOG_ALERT;
  case LogLevel::Critical:
    return LOG_CRIT;
  case LogLevel::Error:
    return LOG_ERR;
  case LogLevel::Warning:
    return LOG_WARNING;
  case LogLevel::Notice:
    return LOG_NOTICE;
  case LogLevel::Info:
    return LOG_INFO;
  case LogLevel::Debug:
    return LOG_DEBUG;
  }
  return LOG_INFO;
}

int Logger::to_mask(LogLevel level) {
  // Allow everything from 'level' and above (lower numerical severity)
  switch (level) {
  case LogLevel::Emergency:
    return LOG_MASK(LOG_EMERG);
  case LogLevel::Alert:
    return LOG_MASK(LOG_EMERG) | LOG_MASK(LOG_ALERT);
  case LogLevel::Critical:
    return LOG_UPTO(LOG_CRIT);
  case LogLevel::Error:
    return LOG_UPTO(LOG_ERR);
  case LogLevel::Warning:
    return LOG_UPTO(LOG_WARNING);
  case LogLevel::Notice:
    return LOG_UPTO(LOG_NOTICE);
  case LogLevel::Info:
    return LOG_UPTO(LOG_INFO);
  case LogLevel::Debug:
    return LOG_UPTO(LOG_DEBUG);
  }
  return LOG_UPTO(LOG_INFO);
}

std::mutex &Logger::mutex() { return g_logger_mutex; }
LogLevel &Logger::current_level() { return g_current_level; }
bool &Logger::initialized() { return g_initialized; }

} // namespace fion::logging
