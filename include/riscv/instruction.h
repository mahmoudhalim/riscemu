#pragma once
#include <cstdint>
enum class InstructionFormat {
  R_TYPE,
  I_TYPE,
  S_TYPE,
  B_TYPE,
  U_TYPE,
  J_TYPE,
  UNKNOWN
};

enum class InstructionType {
  // R Type
  ADD,
  SUB,
  SLL,
  SLT,
  SLTU,
  XOR,
  SRL,
  SRA,
  OR,
  AND,
  UNKNOWN
};

struct DecodedInstruction {
  uint32_t raw;
  InstructionFormat format;
  InstructionType type;

  uint8_t opcode;
  uint8_t rd;
  uint8_t rs1;
  uint8_t rs2;
  uint8_t funct3;
  uint8_t funct7;
  int32_t imm;
};
