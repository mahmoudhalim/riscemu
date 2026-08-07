#pragma once

#include <format>
#include <iostream>
#include <optional>
#include <print>
#include <string_view>

// Minimal leveled logger. All internal messages go to stderr so the guest's
// stdout stays clean. A single process-wide default Logger lets CPU, Executor
// and Syscall log without carrying a reference through their API.
namespace logging {

enum class Level : int { Error = 0, Warn = 1, Info = 2, Debug = 3, Trace = 4 };

constexpr std::string_view name(Level level) {
  switch (level) {
  case Level::Error:
    return "error";
  case Level::Warn:
    return "warn";
  case Level::Info:
    return "info";
  case Level::Debug:
    return "debug";
  case Level::Trace:
    return "trace";
  }
  return "?";
}

std::optional<Level> parse_level(std::string_view value);

// The process-wide threshold. Defaults to Level::Warn so ordinary runs stay
// quiet unless a CLI flag raises it.
void set_level(Level level);
Level level();
bool enabled(Level level);

template <typename... Args>
void log(Level level, std::string_view domain, std::format_string<Args...> fmt,
         Args&&... args) {
  if (!enabled(level))
    return;
  std::print(std::cerr, "[{}][{}] {}\n", logging::name(level), domain,
             std::format(fmt, std::forward<Args>(args)...));
}
} // namespace logging