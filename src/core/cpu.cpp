#include "cpu.h"
#include "core/decoder.h"
#include "core/executor.h"
#include <bitset>
#include <iostream>

CPU::CPU(std::filesystem::path path) : mem_(4 * 1024 * 1024) {
  pc = 0;
  // for testing
  regs_.write(1, 10); // x1 = 10
  regs_.write(2, 3);  // x2 = 3

  program_size_bytes_ = mem_.load_file(path);
}

void CPU::run() {
  for (pc = 0; pc < program_size_bytes_; pc += 4) {
    regs_.pc = pc;
    auto ir = Decoder::decode(mem_.read_word(pc));
    std::cout << std::bitset<32>(ir.raw) << '\n';

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
