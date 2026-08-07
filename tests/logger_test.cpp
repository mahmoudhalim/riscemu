#include "logging/logger.h"

#include <gtest/gtest.h>

TEST(LoggerTest, DefaultLevelIsWarn) {
  logging::set_level(logging::Level::Warn);
  EXPECT_EQ(logging::level(), logging::Level::Warn);
}

TEST(LoggerTest, ParseLevel) {
  EXPECT_EQ(logging::parse_level("error"), logging::Level::Error);
  EXPECT_EQ(logging::parse_level("warn"), logging::Level::Warn);
  EXPECT_EQ(logging::parse_level("info"), logging::Level::Info);
  EXPECT_EQ(logging::parse_level("debug"), logging::Level::Debug);
  EXPECT_EQ(logging::parse_level("trace"), logging::Level::Trace);
  EXPECT_FALSE(logging::parse_level("verbose").has_value());
  EXPECT_FALSE(logging::parse_level("").has_value());
}

TEST(LoggerTest, EnabledOnlyAtOrBelowThreshold) {
  logging::set_level(logging::Level::Info);
  EXPECT_TRUE(logging::enabled(logging::Level::Error));
  EXPECT_TRUE(logging::enabled(logging::Level::Warn));
  EXPECT_TRUE(logging::enabled(logging::Level::Info));
  EXPECT_FALSE(logging::enabled(logging::Level::Debug));
  EXPECT_FALSE(logging::enabled(logging::Level::Trace));
  logging::set_level(logging::Level::Warn);
}

TEST(LoggerTest, LevelNames) {
  EXPECT_EQ(logging::name(logging::Level::Error), "error");
  EXPECT_EQ(logging::name(logging::Level::Trace), "trace");
}