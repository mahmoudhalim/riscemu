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

TEST(DecoderTest, DecodesJalZeroOffset) {
  // jal x0, 0  (a near-NOP)
  constexpr uint32_t jal = 0x0000006F;
  auto ir = Decoder::decode(jal);

  EXPECT_EQ(ir.format, InstructionFormat::J_TYPE);
  EXPECT_EQ(ir.type, InstructionType::JAL);
  EXPECT_EQ(ir.opcode, 0x6F);
  EXPECT_EQ(ir.rd, 0);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesJalForwardOffset) {
  // jal x0, 8  (skip one instruction)
  constexpr uint32_t jal = 0x0080006F;
  auto ir = Decoder::decode(jal);

  EXPECT_EQ(ir.format, InstructionFormat::J_TYPE);
  EXPECT_EQ(ir.type, InstructionType::JAL);
  EXPECT_EQ(ir.rd, 0);
  EXPECT_EQ(ir.imm, 8);
}

TEST(DecoderTest, DecodesJalWithLinkRegister) {
  // jal x1, 0  (call: x1 = return address)
  constexpr uint32_t jal = 0x000000EF;
  auto ir = Decoder::decode(jal);

  EXPECT_EQ(ir.format, InstructionFormat::J_TYPE);
  EXPECT_EQ(ir.type, InstructionType::JAL);
  EXPECT_EQ(ir.rd, 1);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesJalr) {
  // jalr x1, x2, 16
  constexpr uint32_t jalr = 0x010100E7;
  auto ir = Decoder::decode(jalr);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::JALR);
  EXPECT_EQ(ir.opcode, 0x67);
  EXPECT_EQ(ir.rd, 1);
  EXPECT_EQ(ir.rs1, 2);
  EXPECT_EQ(ir.funct3, 0);
  EXPECT_EQ(ir.imm, 16);
}
