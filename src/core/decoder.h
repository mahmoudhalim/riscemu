#pragma once

#include "riscv/instruction.h"
class Decoder {
public:
  static DecodedInstruction decode(uint32_t instruction);

private:
  static InstructionFormat get_instruction_format(uint8_t opcode);
  static InstructionType get_instruction_type(InstructionFormat format,
                                              uint8_t opcode, uint8_t funct3,
                                              uint8_t funct7, uint8_t rs2);
};
