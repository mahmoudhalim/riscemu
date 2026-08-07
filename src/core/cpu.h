#pragma once

#include <cstddef>
#include <filesystem>

#include "memory/memory.h"
#include "registers.h"
#include "system/syscall.h"

class CPU {
public:
  CPU(std::filesystem::path path);
  void step();
  void run();

  // Number of instructions executed since construction (or since step())
  // returned); used for run summaries and tracing.
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
  Registers regs_{};
  Memory mem_;
  Syscall syscall_;
  uint32_t program_end_addr_ = 0;
  bool halted_ = false;
  int exit_code_ = 0;
  uint64_t instruction_count_ = 0;
};
