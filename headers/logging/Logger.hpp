#pragma once

#include <mutex>
#include <string>
#include <syslog.h>

namespace fion::logging {

enum class LogLevel {
  Emergency, // system is unusable
  Alert,     // action must be taken immediately
  Critical,  // critical conditions
  Error,     // error conditions
  Warning,   // warning conditions
  Notice,    // normal but significant condition
  Info,      // informational
  Debug      // debug-level messages
};

/**
 * @brief Thin wrapper around syslog providing a simple API.
 */
class Logger {
public:
  // Initialize syslog with ident and facility. If logToStderr is true, also log
  // to stderr.
  static void init(const std::string &ident, int facility = LOG_USER,
                   bool logToStderr = true);

  // Close syslog
  static void shutdown();

  static void set_level(LogLevel level);
  static LogLevel level();

  static void log(LogLevel level, const std::string &message);
  static void emergency(const std::string &message) {
    log(LogLevel::Emergency, message);
  }
  static void alert(const std::string &message) {
    log(LogLevel::Alert, message);
  }
  static void critical(const std::string &message) {
    log(LogLevel::Critical, message);
  }
  static void error(const std::string &message) {
    log(LogLevel::Error, message);
  }
  static void warning(const std::string &message) {
    log(LogLevel::Warning, message);
  }
  static void notice(const std::string &message) {
    log(LogLevel::Notice, message);
  }
  static void info(const std::string &message) { log(LogLevel::Info, message); }
  static void debug(const std::string &message) {
    log(LogLevel::Debug, message);
  }

private:
  static int to_priority(LogLevel level);
  static int to_mask(LogLevel level);
  static std::mutex &mutex();
  static LogLevel &current_level();
  static bool &initialized();
};

} // namespace fion::logging
