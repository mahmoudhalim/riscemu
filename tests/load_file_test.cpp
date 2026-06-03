#include "memory/memory.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
std::filesystem::path write_temp_binary(const std::vector<uint8_t> &data) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_load_file_" + std::to_string(std::rand()) + ".bin");
  std::ofstream f(path, std::ios::binary);
  f.write(reinterpret_cast<const char *>(data.data()), data.size());
  f.close();
  return path;
}
} // namespace

TEST(LoadFileTest, LoadsBinaryIntoMemory) {
  std::vector<uint8_t> bin = {0x44, 0x33, 0x22, 0x11, 0xb3, 0x81, 0x20, 0x00,
                              0xef, 0xbe, 0xad, 0xde};
  auto path = write_temp_binary(bin);
  Memory mem(64);

  size_t loaded = mem.load_file(path);

  EXPECT_EQ(loaded, 12u);
  EXPECT_EQ(mem.read_word(0), 0x11223344u);
  EXPECT_EQ(mem.read_word(4), 0x002081b3u);
  EXPECT_EQ(mem.read_word(8), 0xdeadbeefu);

  std::filesystem::remove(path);
}

TEST(LoadFileTest, NonExistentFileExits) {
  Memory mem(64);
  EXPECT_DEATH(mem.load_file("/nonexistent/file.bin"), ".*");
}
