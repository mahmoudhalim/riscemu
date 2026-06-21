#include "core/decoder.h"
#include "core/executor.h"
#include "core/registers.h"
#include "memory/memory.h"

#include <gtest/gtest.h>
#include <stdexcept>

namespace {
Memory mem(1024);
constexpr uint32_t encode_b(int32_t imm, uint8_t rs2, uint8_t rs1,
                            uint8_t funct3) {
  constexpr uint32_t opcode = 0x63;
  uint32_t uimm = static_cast<uint32_t>(imm);
  uint32_t b12 = (uimm >> 12) & 0x1;
  uint32_t b11 = (uimm >> 11) & 0x1;
  uint32_t b10_5 = (uimm >> 5) & 0x3F;
  uint32_t b4_1 = (uimm >> 1) & 0xF;

  return opcode | (b11 << 7) | (b4_1 << 8) | (static_cast<uint32_t>(funct3) << 12) |
         (static_cast<uint32_t>(rs1) << 15) | (static_cast<uint32_t>(rs2) << 20) |
         (b10_5 << 25) | (b12 << 31);
}
} // namespace

TEST(ExecutorBTypeTest, BeqTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 10);
  regs.write(2, 10);

  auto ir = Decoder::decode(encode_b(12, 2, 1, 0b000)); // BEQ x1, x2, +12
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x100u + 12);
}

TEST(ExecutorBTypeTest, BeqNotTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 10);
  regs.write(2, 11);

  auto ir = Decoder::decode(encode_b(12, 2, 1, 0b000)); // BEQ x1, x2, +12
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x100u + 4);
}

TEST(ExecutorBTypeTest, BneTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 10);
  regs.write(2, 11);

  auto ir = Decoder::decode(encode_b(-8, 2, 1, 0b001)); // BNE x1, x2, -8
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x100u - 8);
}

TEST(ExecutorBTypeTest, BltSignedTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 0xFFFFFFFF); // -1
  regs.write(2, 1);          // 1

  auto ir = Decoder::decode(encode_b(4, 2, 1, 0b100)); // BLT x1, x2, +4
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x104u);
}

TEST(ExecutorBTypeTest, BltSignedNotTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 1);
  regs.write(2, 0xFFFFFFFF); // -1

  auto ir = Decoder::decode(encode_b(4, 2, 1, 0b100)); // BLT x1, x2, +4
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x104u); // PC+4 because not taken
}

TEST(ExecutorBTypeTest, BltuUnsignedTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 1);
  regs.write(2, 0xFFFFFFFF);

  auto ir = Decoder::decode(encode_b(4, 2, 1, 0b110)); // BLTU x1, x2, +4
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x104u);
}

TEST(ExecutorBTypeTest, BgeSignedTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 10);
  regs.write(2, 10);

  auto ir = Decoder::decode(encode_b(4, 2, 1, 0b101)); // BGE x1, x2, +4
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x104u);
}

TEST(ExecutorBTypeTest, BgeuUnsignedNotTaken) {
  Registers regs;
  regs.pc = 0x100;
  regs.write(1, 1);
  regs.write(2, 0xFFFFFFFF);

  auto ir = Decoder::decode(encode_b(4, 2, 1, 0b111)); // BGEU x1, x2, +4
  Executor::execute(ir, regs, mem);

  EXPECT_EQ(regs.pc, 0x104u); // PC+4 because not taken
}
