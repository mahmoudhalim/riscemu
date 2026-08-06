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

#ifdef RISCEMU_TESTING
  // Test-only accessors. Define RISCEMU_TESTING in the test target's
  // compile definitions. No production source file sees these.
  Registers& regs() { return regs_; }
  const Registers& regs() const { return regs_; }
  bool halted() const { return halted_; }
  int exit_code() const { return exit_code_; }
#endif

private:
  Registers regs_{};
  Memory mem_;
  Syscall syscall_;
  uint32_t program_end_addr_ = 0;
  bool halted_ = false;
  int exit_code_ = 0;
};
