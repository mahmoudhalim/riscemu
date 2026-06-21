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
  UNKNOWN,

  // I TYPE
  ADDI,
  SLTI,
  SLTIU,
  XORI,
  ORI,
  ANDI,
  SLLI,
  SRLI,
  SRAI,
  LW,
  LH,
  LHU,
  LB,
  LBU,

  // U TYPE
  LUI,
  AUIPC,

  // J TYPE
  JAL,
  JALR,

  // B TYPE
  BEQ,
  BNE,
  BLT,
  BLTU,
  BGE,
  BGEU,

  // S TYPE
  SB,
  SH,
  SW,
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
