#pragma once
#include <cstdint>
class Registers {
public:
  uint32_t read(uint8_t reg) const {
    if (reg == 0 || reg > 31)
      return 0;
    return registers_[reg];
  }
  void write(uint8_t reg, uint32_t value) {
    if (reg == 0 || reg > 31)
      return;
    registers_[reg] = value;
  }

  // Program counter. the executor is the only writer.
  uint32_t pc = 0;

private:
  uint32_t registers_[32] = {};
};
