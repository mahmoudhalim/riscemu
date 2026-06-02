#pragma once

#include <cstddef>
#include <filesystem>

#include "memory/memory.h"
#include "registers.h"

class CPU {
public:
  uint32_t pc;

  CPU(std::filesystem::path path);
  void step();
  void run();
  void print() const;

private:
  Registers regs_{};
  Memory mem_;
  size_t program_size_bytes_ = 0;
};
