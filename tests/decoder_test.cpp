#include "core/decoder.h"

#include <gtest/gtest.h>

TEST(DecoderTest, DecodesAddInstruction) {
  constexpr uint32_t add = 0x002081b3; // add x3, x1, x2
  auto ir = Decoder::decode(add);

  EXPECT_EQ(ir.format, InstructionFormat::R_TYPE);
  EXPECT_EQ(ir.type, InstructionType::ADD);
  EXPECT_EQ(ir.opcode, 0x33);
  EXPECT_EQ(ir.rd, 3);
  EXPECT_EQ(ir.rs1, 1);
  EXPECT_EQ(ir.rs2, 2);
  EXPECT_EQ(ir.funct3, 0);
  EXPECT_EQ(ir.funct7, 0);
}

TEST(DecoderTest, DecodesSubInstruction) {
  constexpr uint32_t sub = 0x40218233; // sub x4, x3, x2
  auto ir = Decoder::decode(sub);

  EXPECT_EQ(ir.format, InstructionFormat::R_TYPE);
  EXPECT_EQ(ir.type, InstructionType::SUB);
  EXPECT_EQ(ir.opcode, 0x33);
  EXPECT_EQ(ir.rd, 4);
  EXPECT_EQ(ir.rs1, 3);
  EXPECT_EQ(ir.rs2, 2);
  EXPECT_EQ(ir.funct3, 0);
  EXPECT_EQ(ir.funct7, 0x20);
}
