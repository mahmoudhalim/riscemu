#pragma once

#include <cstdint>

#include "memory/memory.h"
#include "riscv/execution.h"
#include "riscv/instruction.h"

class Registers;
class Syscall;

// Per-instruction result. Unlike RunOutcome at the run seam, a single step can
// simply "run" (nothing terminal), so this keeps an explicit Executed state.
enum class ExecutionStatus {
  Executed,
  Exited,  // guest terminated (sys_exit / EBREAK); exit_code is valid
  Faulted, // illegal instruction; fault is valid (pc stamped by execute())
};

struct ExecutionResult {
  ExecutionStatus status = ExecutionStatus::Executed;
  int exit_code = 0;
  FaultReport fault{};
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