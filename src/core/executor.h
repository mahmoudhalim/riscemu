#pragma once

#include "memory/memory.h"
#include "riscv/instruction.h"
class Registers;
class Syscall;

struct ExecutionResult {
  bool halt = false;
  int exit_code = 0;
};

class Executor {
public:
  static ExecutionResult execute(const DecodedInstruction& ir,
                                 Registers& registers, Memory& memory,
                                 Syscall& syscall);
  // Convenience overload for tests that never execute SYSTEM instructions:
  // a fresh, stateless Syscall is created per call.
  static ExecutionResult execute(const DecodedInstruction& ir,
                                 Registers& registers, Memory& memory);

private:
  static ExecutionResult execute_r_type(const DecodedInstruction& ir,
                                        Registers& registers);
  static ExecutionResult execute_i_type(const DecodedInstruction& ir,
                                        Registers& registers,
                                        const Memory& memory);
  static ExecutionResult execute_u_type(const DecodedInstruction& ir,
                                        Registers& registers);
  static ExecutionResult execute_jump(const DecodedInstruction& ir,
                                      Registers& registers);
  static ExecutionResult execute_b_type(const DecodedInstruction& ir,
                                        Registers& registers);
  static ExecutionResult execute_s_type(const DecodedInstruction& ir,
                                        Registers& registers, Memory& memory);
  static ExecutionResult exec_system(const DecodedInstruction& instr,
                                     Registers& regs, Syscall& syscall);
};
