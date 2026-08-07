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

// The executor is a single seam: exactly one entry point, a total function
// (every instruction returns a result; nothing throws). The per-format
// handlers are file-local helpers in executor.cpp.
namespace executor {

ExecutionResult execute(const DecodedInstruction& ir, Registers& registers,
                        Memory& memory, Syscall& syscall);

} // namespace executor
