#ifndef CPU_H
#define CPU_H
#include "instructions.h"
#include <cstdint>
class CPU {
public:
  uint32_t registers[32] = {0};
  uint32_t pc;

  CPU();
  ~CPU();
  Instruction *decode(uint32_t raw);
  void execute(uint32_t raw_instruction);
  void print() const;
};
#endif // CPU_H