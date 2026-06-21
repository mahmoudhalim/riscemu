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

TEST(DecoderTest, DecodesLb) {
  // lb x1, 0(x0)
  constexpr uint32_t lb = 0x00000083;
  auto ir = Decoder::decode(lb);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::LB);
  EXPECT_EQ(ir.opcode, 0x03);
  EXPECT_EQ(ir.rd, 1);
  EXPECT_EQ(ir.rs1, 0);
  EXPECT_EQ(ir.funct3, 0);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesLh) {
  // lh x2, 0(x0)
  constexpr uint32_t lh = 0x00001103;
  auto ir = Decoder::decode(lh);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::LH);
  EXPECT_EQ(ir.opcode, 0x03);
  EXPECT_EQ(ir.rd, 2);
  EXPECT_EQ(ir.rs1, 0);
  EXPECT_EQ(ir.funct3, 1);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesLw) {
  // lw x3, 0(x0)
  constexpr uint32_t lw = 0x00002183;
  auto ir = Decoder::decode(lw);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::LW);
  EXPECT_EQ(ir.opcode, 0x03);
  EXPECT_EQ(ir.rd, 3);
  EXPECT_EQ(ir.rs1, 0);
  EXPECT_EQ(ir.funct3, 2);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesLbu) {
  // lbu x4, 0(x0)
  constexpr uint32_t lbu = 0x00004203;
  auto ir = Decoder::decode(lbu);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::LBU);
  EXPECT_EQ(ir.opcode, 0x03);
  EXPECT_EQ(ir.rd, 4);
  EXPECT_EQ(ir.rs1, 0);
  EXPECT_EQ(ir.funct3, 4);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesLhu) {
  // lhu x5, 0(x0)
  constexpr uint32_t lhu = 0x00005283;
  auto ir = Decoder::decode(lhu);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::LHU);
  EXPECT_EQ(ir.opcode, 0x03);
  EXPECT_EQ(ir.rd, 5);
  EXPECT_EQ(ir.rs1, 0);
  EXPECT_EQ(ir.funct3, 5);
  EXPECT_EQ(ir.imm, 0);
}

TEST(DecoderTest, DecodesLwWithOffset) {
  // lw x6, 8(x7)
  constexpr uint32_t lw = 0x0083A303;
  auto ir = Decoder::decode(lw);

  EXPECT_EQ(ir.format, InstructionFormat::I_TYPE);
  EXPECT_EQ(ir.type, InstructionType::LW);
  EXPECT_EQ(ir.rd, 6);
  EXPECT_EQ(ir.rs1, 7);
  EXPECT_EQ(ir.funct3, 2);
  EXPECT_EQ(ir.imm, 8);
}

TEST(DecoderTest, DecodesLbDoesNotDecodeAsAddi) {
  // lb x1, 0(x0) has funct3=000, opcode=0x03 (not 0x13)
  // It must NOT be decoded as ADDI
  constexpr uint32_t lb = 0x00000083;
  auto ir = Decoder::decode(lb);

  EXPECT_NE(ir.type, InstructionType::ADDI);
  EXPECT_EQ(ir.type, InstructionType::LB);
}

TEST(DecoderTest, DecodesAddiDoesNotDecodeAsLb) {
  // addi x1, x0, 0 has funct3=000, opcode=0x13 (not 0x03)
  // It must NOT be decoded as LB
  constexpr uint32_t addi = 0x00000093;
  auto ir = Decoder::decode(addi);

  EXPECT_NE(ir.type, InstructionType::LB);
  EXPECT_EQ(ir.type, InstructionType::ADDI);
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
