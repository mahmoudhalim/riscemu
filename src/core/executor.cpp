#include "executor.h"
#include "core/registers.h"
#include "riscv/instruction.h"
#include <cstdint>
#include <stdexcept>

void Executor::execute(const DecodedInstruction& ir, Registers& registers,
                       Memory& memory) {
  switch (ir.format) {
  case InstructionFormat::R_TYPE:
    return Executor::execute_r_type(ir, registers);
  case InstructionFormat::I_TYPE:
    return Executor::execute_i_type(ir, registers, memory);
  case InstructionFormat::U_TYPE:
    return Executor::execute_u_type(ir, registers);
  case InstructionFormat::J_TYPE:
    return Executor::execute_jump(ir, registers);
  case InstructionFormat::B_TYPE:
    return Executor::execute_b_type(ir, registers);
  case InstructionFormat::S_TYPE:
    return Executor::execute_s_type(ir, registers, memory);
  default:
    throw std::runtime_error("How Did You do this ?");
  }
}

void Executor::execute_r_type(const DecodedInstruction& ir,
                              Registers& registers) {
  registers.pc += 4;
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
    throw std::runtime_error("Unknown R type Instruction");
  }
}
void Executor::execute_i_type(const DecodedInstruction& ir,
                              Registers& registers, const Memory& memory) {
  uint8_t rd = ir.rd;
  uint32_t rs1 = registers.read(ir.rs1);
  int32_t imm = ir.imm;

  switch (ir.type) {
  case InstructionType::JALR: {
    uint32_t link = registers.pc + 4;
    uint32_t target = (rs1 + imm) & ~1u;
    registers.write(rd, link);
    registers.pc = target;
    break;
  }
  default:
    registers.pc += 4;
    break;
  }

  switch (ir.type) {
  case InstructionType::ADDI:
    registers.write(rd, rs1 + imm);
    break;
  case InstructionType::SLTI:
    registers.write(rd, ((int32_t)rs1 < imm) ? 1 : 0);
    break;
  case InstructionType::SLTIU:
    registers.write(rd, (rs1 < (uint32_t)imm) ? 1 : 0);
    break;
  case InstructionType::SLLI:
    registers.write(rd, rs1 << (imm & 0x1F));
    break;
  case InstructionType::SRLI:
    registers.write(rd, rs1 >> (imm & 0x1F));
    break;
  case InstructionType::SRAI:
    registers.write(
        rd, static_cast<uint32_t>(static_cast<int32_t>(rs1) >> (imm & 0x1F)));
    break;
  case InstructionType::ANDI:
    registers.write(rd, rs1 & imm);
    break;
  case InstructionType::ORI:
    registers.write(rd, rs1 | imm);
    break;
  case InstructionType::XORI:
    registers.write(rd, rs1 ^ imm);
    break;
  case InstructionType::JALR:
    // PC and link already written above.
    break;
  case InstructionType::LW:
    registers.write(rd, memory.read_word(rs1 + imm));
    break;
  case InstructionType::LB:
    registers.write(rd, static_cast<int32_t>(memory.read_byte(rs1 + imm)));
    break;
  case InstructionType::LBU:
    registers.write(rd, memory.read_byte(rs1 + imm));
    break;
  case InstructionType::LH:
    registers.write(rd, static_cast<int32_t>(memory.read_halfword(rs1 + imm)));
    break;
  case InstructionType::LHU:
    registers.write(rd, memory.read_halfword(rs1 + imm));
    break;

  [[unlikely]]
  default:
    throw std::runtime_error("Unknown I type Instruction");
  }
}

void Executor::execute_u_type(const DecodedInstruction& ir,
                              Registers& registers) {
  uint8_t rd = ir.rd;
  int32_t imm = ir.imm;
  uint32_t pc_before = registers.pc;
  registers.pc += 4;

  switch (ir.type) {
  case InstructionType::LUI:
    // Load upper immediate: rd = imm (which already has the upper-20 in
    // [31:12]).
    registers.write(rd, static_cast<uint32_t>(imm));
    break;
  case InstructionType::AUIPC:
    // rd = pc + imm (AUIPC: add upper immediate to PC). PC is the address
    // of the AUIPC instruction itself, so we use the saved pre-advance
    // value.
    registers.write(rd, static_cast<uint32_t>(imm + pc_before));
    break;
  [[unlikely]]
  default:
    throw std::runtime_error("Unknown U type Instruction");
  }
}

void Executor::execute_jump(const DecodedInstruction& ir,
                            Registers& registers) {
  registers.write(ir.rd, registers.pc + 4);
  registers.pc = registers.pc + ir.imm;
}

void Executor::execute_b_type(const DecodedInstruction& ir,
                              Registers& registers) {
  bool taken = false;
  switch (ir.type) {
  case InstructionType::BEQ:
    taken = (registers.read(ir.rs1) == registers.read(ir.rs2));
    break;
  case InstructionType::BNE:
    taken = (registers.read(ir.rs1) != registers.read(ir.rs2));
    break;
  case InstructionType::BLT:
    taken = ((int32_t)registers.read(ir.rs1) < (int32_t)registers.read(ir.rs2));
    break;
  case InstructionType::BGE:
    taken =
        ((int32_t)registers.read(ir.rs1) >= (int32_t)registers.read(ir.rs2));
    break;
  case InstructionType::BLTU:
    taken = (registers.read(ir.rs1) < registers.read(ir.rs2));
    break;
  case InstructionType::BGEU:
    taken = (registers.read(ir.rs1) >= registers.read(ir.rs2));
    break;
  [[unlikely]]
  default:
    throw std::runtime_error("Unknown B type Instruction");
  }

  if (taken) {
    registers.pc += ir.imm;
  } else {
    registers.pc += 4;
  }
}

void Executor::execute_s_type(const DecodedInstruction& ir,
                              Registers& registers, Memory& memory) {
  uint32_t rs1 = registers.read(ir.rs1);
  uint32_t rs2 = registers.read(ir.rs2);
  int32_t imm = ir.imm;
  registers.pc += 4;
  switch (ir.type) {
  case InstructionType::SB:
    memory.write_byte(rs1 + imm, rs2 & 0xFF);
    break;
  case InstructionType::SH:
    memory.write_halfword(rs1 + imm, rs2 & 0xFFFF);
    break;
  case InstructionType::SW:
    memory.write_word(rs1 + imm, rs2);
    break;
  [[unlikely]]
  default:
    throw std::runtime_error("Unknown S type Instruction");
  }
}
