#include "cpu.h"
#include "core/decoder.h"
#include "core/executor.h"
#include <iostream>

CPU::CPU() : mem_(4 * 1024 * 1024) {
  pc = 0;
  // for testing
  regs_.write(1, 10); // x1 = 10
  regs_.write(2, 3);  // x2 = 3

  for (size_t i = 0; i < program.size(); ++i) {
    mem_.write_word(static_cast<uint32_t>(i * 4), program[i]);
  }
}

void CPU::run() {
  for (pc = 0; pc < program.size() * 4; pc += 4) {
    auto ir = Decoder::decode(mem_.read_word(pc));
    Executor::execute(ir, regs_);
    std::cout << "Executed " << (int)ir.type << " | x" << (int)ir.rd << " = "
              << regs_.read(ir.rd) << std::endl;
  }
}

void CPU::print() const {
  for (int i = 0; i < 32; i++) {
    std::cout << "X" << i << " = " << regs_.read(i) << std::endl;
  }
}
