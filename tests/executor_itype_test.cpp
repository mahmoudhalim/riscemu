#include "core/decoder.h"
#include "core/executor.h"
#include "core/registers.h"

#include <gtest/gtest.h>
#include <stdexcept>

namespace {
constexpr uint32_t encode_i(uint8_t rd, uint8_t rs1, uint8_t funct3,
                            int32_t imm, uint8_t funct7 = 0) {
  // I-type encoding: opcode=0x13 (OP-IMM), imm[11:0] in bits[31:20].
  return static_cast<uint32_t>(0x13) |
         (static_cast<uint32_t>(rd) << 7) |
         (static_cast<uint32_t>(funct3) << 12) |
         (static_cast<uint32_t>(rs1) << 15) |
         (static_cast<uint32_t>(imm) & 0xFFF) << 20 |
         (static_cast<uint32_t>(funct7) << 25);
}
} // namespace

TEST(ExecutorITypeTest, AddiAddsImmediate) {
  Registers regs;
  regs.write(1, 10);
  auto ir = Decoder::decode(encode_i(3, 1, 0b000, 5));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 15u);
}

TEST(ExecutorITypeTest, SltiSignedComparison) {
  Registers regs;
  regs.write(1, static_cast<uint32_t>(-1));
  auto ir = Decoder::decode(encode_i(3, 1, 0b010, 1));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 1u);
}

TEST(ExecutorITypeTest, SltiuUnsignedComparison) {
  Registers regs;
  // rs1 = 0xFFFFFFFF, imm = 1 (zero-extended). Unsigned: rs1 < imm is false.
  regs.write(1, 0xFFFFFFFFu);
  auto ir = Decoder::decode(encode_i(3, 1, 0b011, 1));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 0u);

  // rs1 = 0, imm = 1. Unsigned: 0 < 1 is true.
  Registers regs2;
  regs2.write(1, 0u);
  auto ir2 = Decoder::decode(encode_i(3, 1, 0b011, 1));
  Executor::execute(ir2, regs2);
  EXPECT_EQ(regs2.read(3), 1u);
}

TEST(ExecutorITypeTest, SlliShiftsLeft) {
  Registers regs;
  regs.write(1, 1u);
  // SLLI: funct3=0b001, shamt=4, funct7=0.
  auto ir = Decoder::decode(encode_i(3, 1, 0b001, 4, 0));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 16u);
}

TEST(ExecutorITypeTest, SrliShiftsRightLogical) {
  Registers regs;
  regs.write(1, 0b1000u);
  // SRLI: funct3=0b101, shamt=2, funct7=0.
  auto ir = Decoder::decode(encode_i(3, 1, 0b101, 2, 0));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 0b10u);
}

TEST(ExecutorITypeTest, SraiShiftsRightArithmetic) {
  Registers regs;
  regs.write(1, 0x80000000u);
  // SRAI: funct3=0b101, shamt=1, funct7=0b0100000.
  auto ir = Decoder::decode(encode_i(3, 1, 0b101, 1, 0b0100000));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 0xC0000000u);
}

TEST(ExecutorITypeTest, SlliShamtComesFromImmediateLow5Bits) {
  // Regression: shamt lives in imm[4:0] (low 5 bits of the 12-bit immediate),
  // not in ir.rs2. Here the top 7 bits of imm are set (0x80) but the low
  // 5 bits are still 4. Expected shift amount is 4.
  Registers regs;
  regs.write(1, 1u);
  auto ir = Decoder::decode(encode_i(3, 1, 0b001, 0x804, 0));
  Executor::execute(ir, regs);
  EXPECT_EQ(regs.read(3), 16u);
}
