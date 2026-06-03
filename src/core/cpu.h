#pragma once

#include <cstddef>
#include <filesystem>

#include "memory/memory.h"
#include "registers.h"

class CPU {
public:
  CPU(std::filesystem::path path);
  void step();
  void run();
  void print() const;

#ifdef RISCEMU_TESTING
  // Test-only accessors. Define RISCEMU_TESTING in the test target's
  // compile definitions. No production source file sees these.
  Registers &regs() { return regs_; }
  const Registers &regs() const { return regs_; }
#endif

private:
  Registers regs_{};
  Memory mem_;
  size_t program_size_bytes_ = 0;
};
