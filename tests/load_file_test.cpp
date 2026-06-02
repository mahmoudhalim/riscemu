#include "memory/memory.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

namespace {
std::filesystem::path write_temp_hex(const std::string &contents) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_load_file_" + std::to_string(std::rand()) + ".hex");
  std::ofstream f(path);
  f << contents;
  f.close();
  return path;
}
} // namespace

TEST(LoadFileTest, LoadsWordsIntoMemory) {
  auto path = write_temp_hex("11223344\n002081b3\ndeadbeef\n");
  Memory mem(64);

  size_t loaded = mem.load_file(path);

  EXPECT_EQ(loaded, 12u);
  EXPECT_EQ(mem.read_word(0), 0x11223344u);
  EXPECT_EQ(mem.read_word(4), 0x002081b3u);
  EXPECT_EQ(mem.read_word(8), 0xdeadbeefu);

  std::filesystem::remove(path);
}

TEST(LoadFileTest, MalformedTokenStopsLoading) {
  auto path = write_temp_hex("11223344\nzzzz\ndeadbeef\n");
  Memory mem(64);

  size_t loaded = mem.load_file(path);

  EXPECT_EQ(loaded, 4u);
  EXPECT_EQ(mem.read_word(0), 0x11223344u);
  EXPECT_EQ(mem.read_word(4), 0u);

  std::filesystem::remove(path);
}
