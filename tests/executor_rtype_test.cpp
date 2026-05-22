#include "core/decoder.h"
#include "core/executor.h"
#include "core/registers.h"

#include <gtest/gtest.h>
#include <stdexcept>

namespace {
constexpr uint32_t encode_r(uint8_t rd, uint8_t rs1, uint8_t rs2,
                            uint8_t funct3, uint8_t funct7) {
  constexpr uint32_t opcode = 0x33;
  return static_cast<uint32_t>(opcode) | (static_cast<uint32_t>(rd) << 7) |
         (static_cast<uint32_t>(funct3) << 12) |
         (static_cast<uint32_t>(rs1) << 15) |
         (static_cast<uint32_t>(rs2) << 20) |
         (static_cast<uint32_t>(funct7) << 25);
}
} // namespace

TEST(ExecutorRTypeTest, AddsRegisters) {
  Registers regs;
  regs.write(1, 10);
  regs.write(2, 3);

  auto ir = Decoder::decode(encode_r(3, 1, 2, 0b000, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(3), 13u);
}

TEST(ExecutorRTypeTest, SubtractsRegisters) {
  Registers regs;
  regs.write(3, 10);
  regs.write(2, 4);

  auto ir = Decoder::decode(encode_r(4, 3, 2, 0b000, 0b0100000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(4), 6u);
}

TEST(ExecutorRTypeTest, ShiftsLeftLogical) {
  Registers regs;
  regs.write(1, 1);
  regs.write(2, 4);

  auto ir = Decoder::decode(encode_r(5, 1, 2, 0b001, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(5), 16u);
}

TEST(ExecutorRTypeTest, SetsLessThanSigned) {
  Registers regs;
  regs.write(1, static_cast<uint32_t>(-1));
  regs.write(2, 1);

  auto ir = Decoder::decode(encode_r(6, 1, 2, 0b010, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(6), 1u);
}

TEST(ExecutorRTypeTest, SetsLessThanUnsigned) {
  Registers regs;
  regs.write(1, 1);
  regs.write(2, 2);

  auto ir = Decoder::decode(encode_r(7, 1, 2, 0b011, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(7), 1u);
}

TEST(ExecutorRTypeTest, XorsRegisters) {
  Registers regs;
  regs.write(1, 0b1010);
  regs.write(2, 0b1100);

  auto ir = Decoder::decode(encode_r(8, 1, 2, 0b100, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(8), 0b0110u);
}

TEST(ExecutorRTypeTest, ShiftsRightLogical) {
  Registers regs;
  regs.write(1, 0b1000);
  regs.write(2, 2);

  auto ir = Decoder::decode(encode_r(9, 1, 2, 0b101, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(9), 0b10u);
}

TEST(ExecutorRTypeTest, ShiftsRightArithmetic) {
  Registers regs;
  regs.write(1, 0x80000000u);
  regs.write(2, 1);

  auto ir = Decoder::decode(encode_r(10, 1, 2, 0b101, 0b0100000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(10), 0xC0000000u);
}

TEST(ExecutorRTypeTest, OrsRegisters) {
  Registers regs;
  regs.write(1, 0b1010);
  regs.write(2, 0b1100);

  auto ir = Decoder::decode(encode_r(11, 1, 2, 0b110, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(11), 0b1110u);
}

TEST(ExecutorRTypeTest, AndsRegisters) {
  Registers regs;
  regs.write(1, 0b1010);
  regs.write(2, 0b1100);

  auto ir = Decoder::decode(encode_r(12, 1, 2, 0b111, 0b0000000));
  Executor::execute(ir, regs);

  EXPECT_EQ(regs.read(12), 0b1000u);
}

TEST(ExecutorRTypeTest, ThrowsOnNonRType) {
  Registers regs;
  auto ir = Decoder::decode(0x00000013); // addi x0, x0, 0

  EXPECT_THROW(Executor::execute(ir, regs), std::runtime_error);
}
