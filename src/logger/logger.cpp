#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include "logger.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

std::vector<LogEntry> Logger::messages_;

std::string Logger::CurrentDateTimeToString() {
  // to_time_t takes a timepoint returned from now() and converts it to number of seconds since 1970 (POSIX)
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  // strftime() needs an existing string to work with, so we create and provide an array of empty characters
  std::string timestamp(30, '\0');
  std::strftime(&timestamp[0], timestamp.size(), "%d-%b-%Y %H:%M:%S", std::localtime(&now));

  return timestamp;
}

std::string Logger::CheckSDLError() {
  const std::string error = SDL_GetError();
  if (error.empty()) {
    return std::string("Not Available");
  }
  SDL_ClearError();
  return error;
}

std::string Logger::CheckIMGError() {
  const std::string error = IMG_GetError();
  if (error.empty()) {
    return std::string("Not Available");
  }
  return error;
}

std::string Logger::CheckTTFError() {
  const std::string error = TTF_GetError();
  if (error.empty()) {
    return std::string("Not Available");
  }
  return error;
}

void Logger::Log(LogLevel level, const std::string& message, std::function<std::string()> error_check) {
  std::string error_message;
  if (error_check) {
    error_message = error_check();
  }

  std::string formatted_message = FormatMessage(level, message, error_message);

  LogEntry log_entry;
  log_entry.level = level;
  log_entry.message = formatted_message;

  std::cout << formatted_message << std::endl;
  messages_.push_back(log_entry);
}

std::string Logger::FormatMessage(LogLevel level, const std::string& message, const std::string& error_message) {
  std::string level_str;
  std::string color_code;

  switch (level) {
    case LogLevel::INFO:      level_str = "INFO"; color_code = "\x1b[32m"; break;
    case LogLevel::WARNING:   level_str = "WARN"; color_code = "\x1b[33m"; break;
    case LogLevel::ERROR:     level_str = " ERR"; color_code = "\x1b[31m"; break;
    case LogLevel::CRITICAL:  level_str = "CRIT"; color_code = "\x1b[41m"; break;
  }

  std::string formatted_message = color_code + level_str + "\033[0m [" + CurrentDateTimeToString() + "] - " + message;
  if (!error_message.empty()) {
    formatted_message += " (SDL Error: " + error_message + ")";
  }
  return formatted_message;
}

void Logger::Info(const std::string& message, std::function<std::string()> error_check) {
  Log(LogLevel::INFO, message, error_check);
}

void Logger::Warn(const std::string& message, std::function<std::string()> error_check) {
  Log(LogLevel::WARNING, message, error_check);
}

void Logger::Err(const std::string& message, std::function<std::string()> error_check) {
  Log(LogLevel::ERROR, message, error_check);
}

void Logger::Crit(const std::string& message, std::function<std::string()> error_check) {
  Log(LogLevel::CRITICAL, message, error_check);
}
