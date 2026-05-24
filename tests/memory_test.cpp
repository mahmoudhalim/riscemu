#include "memory/memory.h"

#include <gtest/gtest.h>

TEST(MemoryTest, ReadWrite32IsLittleEndian) {
  Memory mem(16);
  mem.write_word(0, 0x11223344u);

  EXPECT_EQ(mem.read_byte(0), 0x44u);
  EXPECT_EQ(mem.read_byte(1), 0x33u);
  EXPECT_EQ(mem.read_byte(2), 0x22u);
  EXPECT_EQ(mem.read_byte(3), 0x11u);

  EXPECT_EQ(mem.read_word(0), 0x11223344u);
}

TEST(MemoryTest, ReadWrite16IsLittleEndian) {
  Memory mem(8);
  mem.write_halfword(0, 0xABCDu);

  EXPECT_EQ(mem.read_byte(0), 0xCDu);
  EXPECT_EQ(mem.read_byte(1), 0xABu);
  EXPECT_EQ(mem.read_halfword(0), 0xABCDu);
}

TEST(MemoryTest, ReadWrite8RoundTrips) {
  Memory mem(4);
  mem.write_byte(3, 0x7Fu);
  EXPECT_EQ(mem.read_byte(3), 0x7Fu);
}

#ifndef NDEBUG
TEST(MemoryDeathTest, Read32UnalignedDies) {
  Memory mem(16);
  EXPECT_DEATH((void)mem.read_word(2), ".*");
}

TEST(MemoryDeathTest, Write32UnalignedDies) {
  Memory mem(16);
  EXPECT_DEATH(mem.write_word(2, 0u), ".*");
}

TEST(MemoryDeathTest, Read16UnalignedDies) {
  Memory mem(16);
  EXPECT_DEATH((void)mem.read_halfword(1), ".*");
}

TEST(MemoryDeathTest, Write16UnalignedDies) {
  Memory mem(16);
  EXPECT_DEATH(mem.write_halfword(1, 0u), ".*");
}

TEST(MemoryDeathTest, Read8OutOfBoundsDies) {
  Memory mem(4);
  EXPECT_DEATH((void)mem.read_byte(4), ".*");
}

TEST(MemoryDeathTest, Write8OutOfBoundsDies) {
  Memory mem(4);
  EXPECT_DEATH(mem.write_byte(4, 0u), ".*");
}

TEST(MemoryDeathTest, Read32OutOfBoundsDies) {
  Memory mem(4);
  EXPECT_DEATH((void)mem.read_word(4), ".*");
}

TEST(MemoryDeathTest, Write32OutOfBoundsDies) {
  Memory mem(4);
  EXPECT_DEATH(mem.write_word(4, 0u), ".*");
}
#endif
