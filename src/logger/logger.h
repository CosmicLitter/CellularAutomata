#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <functional>

enum LogLevel {
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

struct LogEntry {
  LogLevel level;
  std::string message;
};

class Logger {
  static std::vector<LogEntry> messages_;
  static std::string CurrentDateTimeToString();
  static std::string FormatMessage(LogLevel level, const std::string& message, const std::string& error_message);
  static void Log(LogLevel level, const std::string& message, std::function<std::string()> error_check = nullptr);

public:
  static void Info(const std::string& message, std::function<std::string()> error_check = nullptr);
  static void Warn(const std::string& message, std::function<std::string()> error_check = nullptr);
  static void Err(const std::string& message, std::function<std::string()> error_check = nullptr);
  static void Crit(const std::string& message, std::function<std::string()> error_check = nullptr);
  static std::string CheckSDLError();
  static std::string CheckIMGError();
  static std::string CheckTTFError();
};

#endif
