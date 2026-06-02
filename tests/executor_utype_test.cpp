#include "core/decoder.h"
#include "core/executor.h"
#include "core/registers.h"

#include <gtest/gtest.h>

namespace {
constexpr uint32_t encode_u(uint8_t rd, uint32_t imm20) {
  // U-type: opcode in [6:0], rd in [11:7], imm[31:12] in [31:12].
  // imm20 is the 20-bit value placed at [31:12].
  constexpr uint32_t lui_opcode = 0b0110111;
  return lui_opcode | (static_cast<uint32_t>(rd) << 7) |
         ((imm20 & 0xFFFFF) << 12);
}
} // namespace

TEST(ExecutorUTypeTest, LuiLoadsUpperImmediate) {
  Registers regs;
  // LUI x3, 0x12345 -> x3 = 0x12345000.
  auto ir = Decoder::decode(encode_u(3, 0x12345));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(3), 0x12345000u);
}

TEST(ExecutorUTypeTest, LuiClearsLow12Bits) {
  Registers regs;
  // LUI x4, 0x1 -> x4 = 0x00001000 (low 12 bits are zero).
  auto ir = Decoder::decode(encode_u(4, 0x1));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(4), 0x00001000u);
}

TEST(ExecutorUTypeTest, LuiDoesNotReadRegisters) {
  // LUI must not depend on rs1, even if the source register is x0.
  Registers regs;
  regs.write(0, 0xDEADBEEF); // ignored (x0)
  auto ir = Decoder::decode(encode_u(5, 0xABCDE));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(5), 0xABCDE000u);
}
