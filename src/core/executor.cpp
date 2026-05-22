#include "executor.h"
#include "core/registers.h"
#include "riscv/instruction.h"
#include <stdexcept>

void Executor::execute(DecodedInstruction ir, Registers &registers) {
  switch (ir.format) {
  case InstructionFormat::R_TYPE:
    return Executor::execute_r_type(ir, registers);
  default:
    throw std::runtime_error("How Did You do this ?");
  }
}

void Executor::execute_r_type(DecodedInstruction ir, Registers &registers) {
  uint8_t rd = ir.rd;

  uint32_t rs1 = registers.read(ir.rs1);
  uint32_t rs2 = registers.read(ir.rs2);

  uint32_t shamt = rs2 & 0x1F;

  switch (ir.type) {
  case InstructionType::ADD:
    registers.write(rd, rs1 + rs2);
    break;

  case InstructionType::SUB:
    registers.write(rd, rs1 - rs2);
    break;

  case InstructionType::SLL:
    registers.write(rd, rs1 << shamt);
    break;

  case InstructionType::SLT:
    registers.write(rd, ((int32_t)rs1 < (int32_t)rs2) ? 1 : 0);
    break;

  case InstructionType::SLTU:
    registers.write(rd, (rs1 < rs2) ? 1 : 0);
    break;

  case InstructionType::XOR:
    registers.write(rd, rs1 ^ rs2);
    break;

  case InstructionType::SRL:
    registers.write(rd, rs1 >> shamt);
    break;

  case InstructionType::SRA:
    registers.write(rd, (uint32_t)((int32_t)rs1 >> shamt));
    break;

  case InstructionType::OR:
    registers.write(rd, rs1 | rs2);
    break;

  case InstructionType::AND:
    registers.write(rd, rs1 & rs2);
    break;

  [[unlikely]]
  default:
    throw std::runtime_error("Inrsid R type Instruction");
  }
}
