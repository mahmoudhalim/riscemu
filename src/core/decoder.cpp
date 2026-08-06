#include "decoder.h"
#include "riscv/instruction.h"
#include <cstdint>

DecodedInstruction Decoder::decode(uint32_t instruction) {
  DecodedInstruction ir;
  ir.raw = instruction;
  ir.opcode = instruction & 0x7f;         // bits [6:0]
  ir.rd = (instruction >> 7) & 0x1f;      // bits [11:7]
  ir.funct3 = (instruction >> 12) & 0x07; // bits [14:12]
  ir.rs1 = (instruction >> 15) & 0x1f;    // bits [19:15]
  ir.rs2 = (instruction >> 20) & 0x1f;    // bits [24:20]
  ir.funct7 = (instruction >> 25) & 0x7f; // bits [31:25]

  ir.format = get_instruction_format(ir.opcode);
  if (ir.format == InstructionFormat::I_TYPE) {
    ir.imm = (static_cast<int32_t>(instruction) >> 20);
  } else if (ir.format == InstructionFormat::U_TYPE) {
    ir.imm = static_cast<int32_t>(instruction & 0xFFFFF000u);
  } else if (ir.format == InstructionFormat::J_TYPE) {
    uint32_t imm_sign = instruction >> 31 & 0x01;
    uint32_t imm_19_12 = instruction >> 12 & 0xFF;
    uint32_t imm_11 = instruction >> 20 & 0x01;
    uint32_t imm_10_1 = instruction >> 21 & 0x3FF;
    uint32_t imm =
        imm_sign << 20 | imm_19_12 << 12 | imm_11 << 11 | imm_10_1 << 1;
    if (imm & (1 << 20)) {
      imm |= 0xFFF00000;
    }
    ir.imm = imm;
  } else if (ir.format == InstructionFormat::B_TYPE) {
    uint32_t imm_sign = instruction >> 31 & 0x01;
    uint32_t imm11 = instruction >> 7 & 0x01;
    uint32_t imm10_5 = instruction >> 25 & 0x3F;
    uint32_t imm4_1 = instruction >> 8 & 0x0F;

    uint32_t imm = imm_sign << 12 | imm11 << 11 | imm10_5 << 5 | imm4_1 << 1;
    if (imm & (1 << 12)) {
      imm |= 0xFFFFE000;
    }
    ir.imm = imm;
  } else if (ir.format == InstructionFormat::S_TYPE) {
    uint32_t imm11_5 = instruction >> 25 & 0x7F;
    uint32_t imm4_0 = instruction >> 7 & 0x1F;

    uint32_t raw_imm = imm11_5 << 5 | imm4_0;
    int32_t imm = (raw_imm & 0x800) ? static_cast<int32_t>(raw_imm | 0xFFFFF000)
                                    : static_cast<int32_t>(raw_imm);
    ir.imm = imm;
  }
  ir.type =
      get_instruction_type(ir.format, ir.opcode, ir.funct3, ir.funct7, ir.rs2);
  return ir;
}

InstructionFormat Decoder::get_instruction_format(uint8_t opcode) {
  switch (opcode) {
  case 0b0110011:
    return InstructionFormat::R_TYPE;
  case 0b0010011: // OP-IMM
  case 0b0000011: // LOAD
  case 0b1100111: // JALR
    return InstructionFormat::I_TYPE;
  case 0b1110011: // SYSTEM
    return InstructionFormat::SYSTEM;
  case 0b0100011:
    return InstructionFormat::S_TYPE;
  case 0b1100011:
    return InstructionFormat::B_TYPE;
  case 0b1101111:
    return InstructionFormat::J_TYPE;
  case 0b0110111: // LUI
  case 0b0010111: // AUIPC
    return InstructionFormat::U_TYPE;
  default:
    return InstructionFormat::UNKNOWN;
  }
}

InstructionType Decoder::get_instruction_type(InstructionFormat format,
                                              uint8_t opcode, uint8_t funct3,
                                              uint8_t funct7, uint8_t rs2) {
  // R-type instructions
  if (format == InstructionFormat::R_TYPE) {
    if (funct3 == 0b000) {
      return (funct7 == 0b0000000) ? InstructionType::ADD
                                   : InstructionType::SUB;
    }
    if (funct3 == 0b001)
      return InstructionType::SLL;
    if (funct3 == 0b010)
      return InstructionType::SLT;
    if (funct3 == 0b011)
      return InstructionType::SLTU;
    if (funct3 == 0b100)
      return InstructionType::XOR;
    if (funct3 == 0b101) {
      return (funct7 == 0b0000000) ? InstructionType::SRL
                                   : InstructionType::SRA;
    }
    if (funct3 == 0b110)
      return InstructionType::OR;
    if (funct3 == 0b111)
      return InstructionType::AND;
  } else if (format == InstructionFormat::I_TYPE) {
    if (opcode == 0b1100111) {
      return InstructionType::JALR;
    }
    if (opcode == 0b0010011) {
      if (funct3 == 0b000)
        return InstructionType::ADDI;
      if (funct3 == 0b010)
        return InstructionType::SLTI;
      if (funct3 == 0b011)
        return InstructionType::SLTIU;
      if (funct3 == 0b100)
        return InstructionType::XORI;
      if (funct3 == 0b110)
        return InstructionType::ORI;
      if (funct3 == 0b111)
        return InstructionType::ANDI;
      if (funct3 == 0b001)
        return InstructionType::SLLI;
      if (funct3 == 0b101) {
        return (funct7 == 0b0000000) ? InstructionType::SRLI
                                     : InstructionType::SRAI;
      }
    }
    if (opcode == 0b0000011) {
      if (funct3 == 0b000)
        return InstructionType::LB;
      if (funct3 == 0b001)
        return InstructionType::LH;
      if (funct3 == 0b010)
        return InstructionType::LW;
      if (funct3 == 0b100)
        return InstructionType::LBU;
      if (funct3 == 0b101)
        return InstructionType::LHU;
    }
  } else if (format == InstructionFormat::SYSTEM) {
    // ECALL: funct3=0, rs2=0. EBREAK: funct3=0, rs2=1.
    if (funct3 == 0b000) {
      return (rs2 == 1) ? InstructionType::EBREAK : InstructionType::ECALL;
    }
  } else if (format == InstructionFormat::U_TYPE) {
    if (opcode == 0b0110111)
      return InstructionType::LUI;
    if (opcode == 0b0010111)
      return InstructionType::AUIPC;
  } else if (format == InstructionFormat::J_TYPE) {
    return InstructionType::JAL;
  } else if (format == InstructionFormat::B_TYPE) {
    if (funct3 == 0b000)
      return InstructionType::BEQ;
    if (funct3 == 0b001)
      return InstructionType::BNE;
    if (funct3 == 0b100)
      return InstructionType::BLT;
    if (funct3 == 0b101)
      return InstructionType::BGE;
    if (funct3 == 0b110)
      return InstructionType::BLTU;
    if (funct3 == 0b111)
      return InstructionType::BGEU;
  } else if (format == InstructionFormat::S_TYPE) {
    if (funct3 == 0b000)
      return InstructionType::SB;
    if (funct3 == 0b001)
      return InstructionType::SH;
    if (funct3 == 0b010)
      return InstructionType::SW;
  }
  return InstructionType::UNKNOWN;
}
