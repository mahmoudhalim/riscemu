#pragma once

#include "memory/memory.h"
#include "riscv/instruction.h"
class Registers;

class Executor {
public:
  static void execute(const DecodedInstruction& ir, Registers& registers,
                      Memory& memory);

private:
  static void execute_r_type(const DecodedInstruction& ir,
                             Registers& registers);
  static void execute_i_type(const DecodedInstruction& ir, Registers& registers,
                             const Memory& memory);
  static void execute_u_type(const DecodedInstruction& ir,
                             Registers& registers);
  static void execute_jump(const DecodedInstruction& ir, Registers& registers);

  static void execute_b_type(const DecodedInstruction& ir,
                             Registers& registers);
  static void execute_s_type(const DecodedInstruction& ir, Registers& registers,
                             Memory& memory);
};
