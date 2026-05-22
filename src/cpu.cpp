#include "cpu.h"
#include "core/decoder.h"
#include "core/executor.h"
#include <iostream>

CPU::CPU() {
  pc = 0;
  // for testing
  regs_.write(1, 10); // x1 = 10
  regs_.write(2, 3);  // x2 = 3
}

void CPU::run_all() {
  for (uint32_t hex : program) {
    auto ir = Decoder::decode(hex);
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
