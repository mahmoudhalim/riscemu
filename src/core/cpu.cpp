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

ExecutionResult CPU::step() {
  uint32_t cur_pc = regs_.pc;
  auto ir = Decoder::decode(mem_.read_word(cur_pc));
  logging::log(logging::Level::Trace, "cpu", "0x{:08x}: {}", cur_pc,
               to_string(ir.type));
  ExecutionResult result = Executor::execute(ir, regs_, mem_, syscall_);
  ++instruction_count_;
  if (result.status != ExecutionStatus::Executed) {
    last_result_ = result;
    halted_ = true;
    if (result.status == ExecutionStatus::Exited) {
      exit_code_ = result.exit_code;
    }
  }
  return result;
}

RunOutcome CPU::run() {
  logging::log(logging::Level::Info, "cpu", "starting at 0x{:08x}", regs_.pc);
  while (regs_.pc < program_end_addr_ && !halted_) {
    step();
  }
  if (!halted_) {
    // The loop stopped because the fetch walked off the loaded image;
    // treat that as an i-fetch access fault.
    logging::log(logging::Level::Info, "cpu",
                 "halted: fetch outside program image at 0x{:08x}", regs_.pc);
    return std::unexpected(
        FaultReport{.kind = FaultKind::FetchOutsideEnd, .pc = regs_.pc});
  }
  if (last_result_.status == ExecutionStatus::Faulted) {
    logging::log(logging::Level::Info, "cpu", "halted: {} at pc=0x{:08x}",
                 to_string(last_result_.fault.kind), last_result_.fault.pc);
    return std::unexpected(last_result_.fault);
  }
  logging::log(logging::Level::Info, "cpu",
               "halted: exit_code={} after {} "
               "instructions",
               exit_code_, instruction_count_);
  return last_result_.exit_code;
}