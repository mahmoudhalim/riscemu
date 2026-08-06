#include "core/decoder.h"
#include "core/executor.h"
#include "core/registers.h"
#include "memory/memory.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
std::filesystem::path write_temp_binary(const std::vector<uint8_t>& data) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_i_u_type_" + std::to_string(std::rand()) + ".bin");
  std::ofstream f(path, std::ios::binary);
  f.write(reinterpret_cast<const char*>(data.data()), data.size());
  f.close();
  return path;
}
} // namespace

// Loads the program from a binary file, decodes+executes every word, and
// asserts the final register state. This is the end-to-end "lui_test" covering
// every I-type op (ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI) and
// the U-type op LUI.
TEST(LuiTestProgram, RunsAllIAndUTypeInstructions) {
  // Instructions encoded as little-endian 32-bit words:
  // 00a00093  addi x1, x0, 10
  // 00300113  addi x2, x0, 3
  // 0050c193  xori x3, x1, 5
  // 0050e213  ori  x4, x1, 5
  // 0050f293  andi x5, x1, 5
  // 0140a313  slti x6, x1, 20
  // 0140b393  sltiu x7, x1, 20
  // 00209413  slli x8, x1, 2
  // 0010d493  srli x9, x1, 1
  // 12345537  lui  x10, 0x12345
  // 00001637  lui  x12, 0x00001
  // 800006b7  lui  x13, 0x80000
  // 4016d713  srai x14, x13, 1
  std::vector<uint8_t> bin = {
      0x93, 0x00, 0xa0, 0x00, 0x13, 0x01, 0x30, 0x00, 0x93, 0xc1, 0x50,
      0x00, 0x13, 0xe2, 0x50, 0x00, 0x93, 0xf2, 0x50, 0x00, 0x13, 0xa3,
      0x40, 0x01, 0x93, 0xb3, 0x40, 0x01, 0x13, 0x94, 0x20, 0x00, 0x93,
      0xd4, 0x10, 0x00, 0x37, 0x55, 0x34, 0x12, 0x37, 0x16, 0x00, 0x00,
      0xb7, 0x06, 0x00, 0x80, 0x13, 0xd7, 0x16, 0x40,
  };

  auto path = write_temp_binary(bin);
  Memory mem(64);
  size_t loaded = mem.load_file(path);
  std::filesystem::remove(path);

  ASSERT_EQ(loaded, 13u * 4u);

  Registers regs;
  for (uint32_t pc = 0; pc < loaded; pc += 4) {
    auto ir = Decoder::decode(mem.read_word(pc));
    regs.pc = pc;
    Executor::execute(ir, regs, mem);
  }

  // I-type
  EXPECT_EQ(regs.read(1), 10u);
  EXPECT_EQ(regs.read(2), 3u);
  EXPECT_EQ(regs.read(3), 15u); // xori: 10 ^ 5
  EXPECT_EQ(regs.read(4), 15u); // ori:  10 | 5
  EXPECT_EQ(regs.read(5), 0u);  // andi: 10 & 5
  EXPECT_EQ(regs.read(6), 1u);  // slti
  EXPECT_EQ(regs.read(7), 1u);  // sltiu
  EXPECT_EQ(regs.read(8), 40u); // slli: 10 << 2
  EXPECT_EQ(regs.read(9), 5u);  // srli: 10 >> 1

  // U-type
  EXPECT_EQ(regs.read(10), 0x12345000u); // lui
  EXPECT_EQ(regs.read(12), 0x00001000u); // lui
  EXPECT_EQ(regs.read(13), 0x80000000u); // lui (sign-bit-only value)
  EXPECT_EQ(regs.read(14), 0xC0000000u); // srai: 0x80000000 >> 1
}
