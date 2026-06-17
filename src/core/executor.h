#pragma once

#include "riscv/instruction.h"
class Registers;

class Executor {
public:
  static void execute(const DecodedInstruction &ir, Registers &registers);

private:
  static void execute_r_type(const DecodedInstruction &ir,
                             Registers &registers);
  static void execute_i_type(const DecodedInstruction &ir,
                             Registers &registers);
  static void execute_u_type(const DecodedInstruction &ir,
                             Registers &registers);
  static void execute_jump(const DecodedInstruction &ir, Registers &registers);

  static void execute_b_type(const DecodedInstruction &ir,
                             Registers &registers);
};
