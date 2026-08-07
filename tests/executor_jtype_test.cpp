#include "core/decoder.h"
#include "core/executor.h"
#include "core/registers.h"
#include "memory/memory.h"
#include "system/syscall.h"

#include <gtest/gtest.h>

namespace {
Memory mem(1024);
Syscall sys{mem};

constexpr uint32_t encode_j(int32_t imm, uint8_t rd = 0) {
  constexpr uint32_t opcode = 0b1101111;
  uint32_t u = static_cast<uint32_t>(imm);
  uint32_t bit_31 = (u >> 20) & 0x1;
  uint32_t bits_30_21 = (u >> 1) & 0x3FF;
  uint32_t bit_20 = (u >> 11) & 0x1;
  uint32_t bits_19_12 = (u >> 12) & 0xFF;
  return opcode | (static_cast<uint32_t>(rd) << 7) | (bits_19_12 << 12) |
         (bit_20 << 20) | (bits_30_21 << 21) | (bit_31 << 31);
}
} // namespace

TEST(ExecutorJTypeTest, JalForwardJumpsOverInstruction) {
  Registers regs;
  regs.pc = 0x40;
  auto ir = Decoder::decode(encode_j(8, 1));
  executor::execute(ir, regs, mem, sys);

  EXPECT_EQ(regs.read(1), 0x44u);
  EXPECT_EQ(regs.pc, 0x48u);
}

TEST(ExecutorJTypeTest, JalBackwardJumpsBack) {
  Registers regs;
  regs.pc = 0x100;
  auto ir = Decoder::decode(encode_j(-8));
  executor::execute(ir, regs, mem, sys);

  EXPECT_EQ(regs.pc, 0xF8u);
}

TEST(ExecutorJTypeTest, JalWithRdZeroDoesNotWriteLink) {
  Registers regs;
  regs.pc = 0x20;
  auto ir = Decoder::decode(encode_j(12));
  executor::execute(ir, regs, mem, sys);

  EXPECT_EQ(regs.read(0), 0u);
  EXPECT_EQ(regs.pc, 0x2Cu);
}

TEST(ExecutorJTypeTest, JalZeroOffsetAdvancesToPcPlusFour) {
  Registers regs;
  regs.pc = 0x80;
  auto ir = Decoder::decode(encode_j(0, 1));
  executor::execute(ir, regs, mem, sys);

  EXPECT_EQ(regs.read(1), 0x84u);
  EXPECT_EQ(regs.pc, 0x80u);
}
