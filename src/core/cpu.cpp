#include "cpu.h"

#include <cstdint>

#include "core/decoder.h"
#include "core/executor.h"
#include "riscv/logging.h"

CPU::CPU(Memory& mem, Syscall& syscall) : mem_(mem), syscall_(syscall) {}

void CPU::initialize(uint32_t entry_point, uint32_t program_end,
                     uint32_t stack_top) {
  regs_ = {};
  regs_.pc = entry_point;
  program_end_addr_ = program_end;
  regs_.write(2, stack_top); // stack pointer near the top of memory
  halted_ = false;
  exit_code_ = 0;
  instruction_count_ = 0;
}

void CPU::step() {
  uint32_t cur_pc = regs_.pc;
  auto ir = Decoder::decode(mem_.read_word(cur_pc));
  logging::log(logging::Level::Trace, "cpu", "0x{:08x}: {}", cur_pc,
               to_string(ir.type));
  auto result = Executor::execute(ir, regs_, mem_, syscall_);
  ++instruction_count_;
  if (result.halt) {
    halted_ = true;
    exit_code_ = result.exit_code;
  }
}

void CPU::run() {
  logging::log(logging::Level::Info, "cpu", "starting at 0x{:08x}", regs_.pc);
  while (regs_.pc < program_end_addr_ && !halted_) {
    step();
  }
  logging::log(logging::Level::Info, "cpu",
               "halted: exit_code={} after {} instructions", exit_code_,
               instruction_count_);
}