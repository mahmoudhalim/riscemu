#pragma once
#include <cstdint>
#include <string_view>
enum class InstructionFormat {
  R_TYPE,
  I_TYPE,
  S_TYPE,
  B_TYPE,
  U_TYPE,
  J_TYPE,
  SYSTEM,
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

  // SYSTEM
  ECALL,
  EBREAK,
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

// Human-readable mnemonic for trace logging.
constexpr std::string_view to_string(InstructionType type) {
  switch (type) {
  case InstructionType::ADD:
    return "add";
  case InstructionType::SUB:
    return "sub";
  case InstructionType::SLL:
    return "sll";
  case InstructionType::SLT:
    return "slt";
  case InstructionType::SLTU:
    return "sltu";
  case InstructionType::XOR:
    return "xor";
  case InstructionType::SRL:
    return "srl";
  case InstructionType::SRA:
    return "sra";
  case InstructionType::OR:
    return "or";
  case InstructionType::AND:
    return "and";
  case InstructionType::ADDI:
    return "addi";
  case InstructionType::SLTI:
    return "slti";
  case InstructionType::SLTIU:
    return "sltiu";
  case InstructionType::XORI:
    return "xori";
  case InstructionType::ORI:
    return "ori";
  case InstructionType::ANDI:
    return "andi";
  case InstructionType::SLLI:
    return "slli";
  case InstructionType::SRLI:
    return "srli";
  case InstructionType::SRAI:
    return "srai";
  case InstructionType::LW:
    return "lw";
  case InstructionType::LH:
    return "lh";
  case InstructionType::LHU:
    return "lhu";
  case InstructionType::LB:
    return "lb";
  case InstructionType::LBU:
    return "lbu";
  case InstructionType::LUI:
    return "lui";
  case InstructionType::AUIPC:
    return "auipc";
  case InstructionType::JAL:
    return "jal";
  case InstructionType::JALR:
    return "jalr";
  case InstructionType::BEQ:
    return "beq";
  case InstructionType::BNE:
    return "bne";
  case InstructionType::BLT:
    return "blt";
  case InstructionType::BLTU:
    return "bltu";
  case InstructionType::BGE:
    return "bge";
  case InstructionType::BGEU:
    return "bgeu";
  case InstructionType::SB:
    return "sb";
  case InstructionType::SH:
    return "sh";
  case InstructionType::SW:
    return "sw";
  case InstructionType::ECALL:
    return "ecall";
  case InstructionType::EBREAK:
    return "ebreak";
  case InstructionType::UNKNOWN:
    return "?";
  }
  return "?";
}
