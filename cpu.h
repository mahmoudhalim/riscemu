#ifndef CPU_H
#define CPU_H
#include "instructions.h"
#include <cstdint>
class cpu {
public:
  uint32_t registers[32] = {0};
  uint32_t pc;

  cpu();
  ~cpu();
  void exec(const Rinstruction &instr);
  void print();
  // void decode(const Rinstruction &instr);
};
#endif // CPU_H