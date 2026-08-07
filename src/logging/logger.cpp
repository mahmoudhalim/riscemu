#include "logging/logger.h"

#include <string_view>

namespace logging {
namespace {
Level g_level = Level::Warn;
}

void set_level(Level level) { g_level = level; }

Level level() { return g_level; }

bool enabled(Level level) { return level <= g_level; }

std::optional<Level> parse_level(std::string_view value) {
  for (Level l :
       {Level::Error, Level::Warn, Level::Info, Level::Debug, Level::Trace}) {
    if (value == name(l)) {
      return l;
    }
  }
  return std::nullopt;
}
} // namespace logging