#include "core/decoder.h"
#include "core/executor.h"
#include "memory/memory.h"
#include "core/registers.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

namespace {
std::filesystem::path write_temp_hex(const std::string &contents) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_i_u_type_" + std::to_string(std::rand()) + ".hex");
  std::ofstream f(path);
  f << contents;
  f.close();
  return path;
}
} // namespace

// Loads the program from a hex string, decodes+executes every word, and
// asserts the final register state. This is the end-to-end "lui_test" covering
// every I-type op (ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI) and
// the U-type op LUI.
TEST(LuiTestProgram, RunsAllIAndUTypeInstructions) {
  const std::string hex = R"(
00a00093
00300113
0050c193
0050e213
0050f293
0140a313
0140b393
00209413
0010d493
12345537
00001637
800006b7
4016d713
)";

  auto path = write_temp_hex(hex);
  Memory mem(64);
  size_t loaded = mem.load_file(path);
  std::filesystem::remove(path);

  ASSERT_EQ(loaded, 13u * 4u);

  Registers regs;
  for (uint32_t pc = 0; pc < loaded; pc += 4) {
    auto ir = Decoder::decode(mem.read_word(pc));
    regs.pc = pc;
    Executor::execute(ir, regs);
  }

  // I-type
  EXPECT_EQ(regs.read(1), 10u);
  EXPECT_EQ(regs.read(2), 3u);
  EXPECT_EQ(regs.read(3), 15u);  // xori: 10 ^ 5
  EXPECT_EQ(regs.read(4), 15u);  // ori:  10 | 5
  EXPECT_EQ(regs.read(5), 0u);   // andi: 10 & 5
  EXPECT_EQ(regs.read(6), 1u);   // slti
  EXPECT_EQ(regs.read(7), 1u);   // sltiu
  EXPECT_EQ(regs.read(8), 40u);  // slli: 10 << 2
  EXPECT_EQ(regs.read(9), 5u);   // srli: 10 >> 1

  // U-type
  EXPECT_EQ(regs.read(10), 0x12345000u); // lui
  EXPECT_EQ(regs.read(12), 0x00001000u); // lui
  EXPECT_EQ(regs.read(13), 0x80000000u); // lui (sign-bit-only value)
  EXPECT_EQ(regs.read(14), 0xC0000000u); // srai: 0x80000000 >> 1
}
