#pragma once

#include "riscv/instruction.h"
class Registers;

class Executor {
public:
  static void execute(DecodedInstruction ir, Registers &registers);

private:
  static void execute_r_type(DecodedInstruction ir, Registers &registers);
};
