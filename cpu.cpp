#include "cpu.h"
#include <iostream>

cpu::cpu() {
  pc = 0;
  registers[6] = 69;
  registers[7] = 655;
}

cpu::~cpu() {}

void cpu::exec(const Rinstruction &instr) {
  switch (instr.opcode) {
  case OpCode::Rtype:
    if (instr.funct3 == Funct3::addSub) {
      if (instr.funct7 == Funct7::add) {
        registers[instr.rd] = registers[instr.rs1] + registers[instr.rs2];
        std::cout << instr.rs1 << " + " << instr.rs2 << " = " << instr.rd
                  << std::endl;
      } else
        registers[instr.rd] = registers[instr.rs1] - registers[instr.rs2];
    }
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