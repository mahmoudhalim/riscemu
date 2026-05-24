#pragma once
#include "memory/memory.h"
#include "registers.h"
#include <vector>
class CPU {
public:
  uint32_t pc;

  CPU();
  void step();
  void run();
  void print() const;

private:
  Registers regs_{};
  Memory mem_;
  std::vector<uint32_t> program = {
      0x002081b3, // add x3, x1, x2
      0x40218233, // sub x4, x3, x2
      0x0020f2b3, // and x5, x1, x2
      0x00239433  // sll x8, x7, x2 (requires slt result)
  };
};
