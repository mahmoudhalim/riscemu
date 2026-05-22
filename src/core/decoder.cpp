#include "decoder.h"
#include "riscv/instruction.h"

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
  ir.type = get_instruction_type(ir.format, ir.funct3, ir.funct7);
  return ir;
}

InstructionFormat Decoder::get_instruction_format(uint8_t opcode) {
  switch (opcode) {
  case 0b0110011:
    return InstructionFormat::R_TYPE;
  case 0b0010011:
    return InstructionFormat::I_TYPE;
  case 0b0000011:
    return InstructionFormat::I_TYPE;
  case 0b0100011:
    return InstructionFormat::S_TYPE;
  case 0b1100011:
    return InstructionFormat::B_TYPE;
  case 0b1101111:
    return InstructionFormat::J_TYPE;
  case 0b1100111:
    return InstructionFormat::I_TYPE;
  case 0b0110111:
    return InstructionFormat::U_TYPE;
  case 0b0010111:
    return InstructionFormat::U_TYPE;
  case 0b1110011:
    return InstructionFormat::I_TYPE;
  default:
    return InstructionFormat::UNKNOWN;
  }
}

InstructionType Decoder::get_instruction_type(InstructionFormat format,
                                              uint8_t funct3, uint8_t funct7) {
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
  }
  return InstructionType::UNKNOWN;
}
