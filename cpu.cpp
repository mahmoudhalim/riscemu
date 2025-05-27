#include "cpu.h"
#include "instructions.h"
#include <cstdint>
#include <iostream>

cpu::cpu() {
  pc = 0;
  // for testing
  registers[6] = 4;
  registers[7] = 30;
}

cpu::~cpu() {}

void cpu::exec(const Rinstruction &instr) {
  switch (instr.opcode) {
  case OpCode::Rtype:
    execRType(instr);
    break;
  default:
    break;
  }
}

void cpu::execRType(const Rinstruction &instr) {
  switch (instr.funct3) {
  case Funct3::addSub:
    if (instr.funct7 == 0) {
      registers[instr.rd] = registers[instr.rs1] + registers[instr.rs2];
    } else
      registers[instr.rd] = registers[instr.rs1] - registers[instr.rs2];
    break;
  case Funct3::sll:
    registers[instr.rd] = registers[instr.rs1]
                          << (registers[instr.rs2] & 0b1111);
    break;
  case Funct3::slt:
    registers[instr.rd] = static_cast<int32_t>(registers[instr.rs1]) <
                          static_cast<int32_t>(registers[instr.rs2]);
    break;
  case Funct3::sltu:
    registers[instr.rd] = registers[instr.rs1] < registers[instr.rs2];
    break;
  case Funct3::xorr:
    registers[instr.rd] = registers[instr.rs1] ^ registers[instr.rs2];
    break;
  case Funct3::sr:
    if (instr.funct7 == 0) {
      registers[instr.rd] = registers[instr.rs1]
                            >> (registers[instr.rs2] & 0b1111);
    } else {
      registers[instr.rd] = static_cast<int32_t>(registers[instr.rs1])
                            >> (registers[instr.rs2] & 0b1111);
    }
    break;
  case Funct3::orr:
    registers[instr.rd] = registers[instr.rs1] | registers[instr.rs2];
    break;
  case Funct3::andd:
    registers[instr.rd] = registers[instr.rs1] & registers[instr.rs2];
    break;

  default:
    break;
  }
}
void cpu::print() {
  for (int i = 0; i < 32; i++) {
    std::cout << "X" << i << " = " << registers[i] << std::endl;
  }
}