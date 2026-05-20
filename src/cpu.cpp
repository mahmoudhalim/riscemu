#include "cpu.h"
#include "instructions.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>

CPU::CPU() {
  pc = 0;
  // for testing
  registers[6] = 4;
  registers[7] = 30;
}

CPU::~CPU() {}

Instruction *CPU::decode(uint32_t raw) {
  OpCode opCode = static_cast<OpCode>(raw & 0b1111111);
  switch (opCode) {
  case OpCode::Rtype:
    return new RTypeInstruction(raw);
    break;
  default:
    throw std::runtime_error("Not supported instruction");
    break;
  }
}

void CPU::execute(uint32_t raw) {
  Instruction *instr = decode(raw);
  instr->execute(*this);
}

void CPU::print() const {
  for (int i = 0; i < 32; i++) {
    std::cout << "X" << i << " = " << registers[i] << std::endl;
  }
}
