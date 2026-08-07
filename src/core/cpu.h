#pragma once

#include <cstdint>

#include "memory/memory.h"
#include "registers.h"
#include "system/syscall.h"

// Execution core of the simulator. Borrows the host components (the memory
// and Syscall front-end) owned by the Simulator; owns only its architectural
// state (registers, PC, halt/exit state, program end).
class CPU {
public:
  CPU(Memory& mem, Syscall& syscall);

  void step();
  void run();

  // Prepare for execution of a freshly loaded image. Called by Simulator after
  // loading an ELF. Resets all architectural state, then points the PC at
  // entry_point and the stack pointer at stack_top. program_end marks the end
  // of the loaded image (run() halts by falling off it).
  void initialize(uint32_t entry_point, uint32_t program_end,
                  uint32_t stack_top);

  // Number of instructions executed since construction (or since the last
  // initialize()); used for run summaries and tracing.
  uint64_t instruction_count() const { return instruction_count_; }
  // True once an ECALL exit has been executed.
  bool halted() const { return halted_; }
  int exit_code() const { return exit_code_; }

#ifdef RISCEMU_TESTING
  // Test-only accessor. Define RISCEMU_TESTING in the test target's
  // compile definitions. No production source file sees this.
  Registers& regs() { return regs_; }
  const Registers& regs() const { return regs_; }
#endif

private:
  Memory& mem_;
  Syscall& syscall_;

  Registers regs_{};
  uint32_t program_end_addr_ = 0;
  bool halted_ = false;
  int exit_code_ = 0;
  uint64_t instruction_count_ = 0;
};