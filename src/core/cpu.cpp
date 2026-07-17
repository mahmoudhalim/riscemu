#include "cpu.h"
#include "core/decoder.h"
#include "core/executor.h"
#include "loader/elf_loader.h"
#include <bitset>
#include <iostream>

CPU::CPU(std::filesystem::path path) : mem_(4 * 1024 * 1024) {
  auto res = ELFLoader::load(path, mem_);
  if (!res) {
    std::cerr << res.error() << '\n';
    exit(1);
  }
  regs_.pc = res->entry_point;
  program_end_addr_ = res->max_addr;
  uint32_t STACK_TOP = mem_.size_bytes() - 16; // Near top of memory
  regs_.write(2, STACK_TOP);
}

void CPU::step() {
  uint32_t cur_pc = regs_.pc;
  auto ir = Decoder::decode(mem_.read_word(cur_pc));
  std::cout << std::bitset<32>(ir.raw) << '\n';
  Executor::execute(ir, regs_, mem_);
  std::cout << "Executed " << (int)ir.type << " | x" << (int)ir.rd << " = "
            << regs_.read(ir.rd) << std::endl;
}

void CPU::run() {
  while (regs_.pc < program_end_addr_) {
    step();
  }
}

void CPU::print() const {
  for (int i = 0; i < 32; i++) {
    std::cout << "X" << i << " = " << regs_.read(i) << std::endl;
  }
}
