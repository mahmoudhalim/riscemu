#pragma once

#include <cstdint>
#include <expected>
#include <string_view>

// Outcome vocabulary for the run seam. run() reports either the guest's exit
// code (it called sys_exit, or hit EBREAK) or a FaultReport describing why the
// machine stopped before the guest chose to:
//   - IllegalInstruction: an unknown opcode / instruction type was fetched.
//   - FetchOutsideEnd: the fetch loop walked past the loaded program image.
enum class FaultKind {
  IllegalInstruction,
  FetchOutsideEnd,
};

constexpr std::string_view to_string(FaultKind kind) {
  switch (kind) {
  case FaultKind::IllegalInstruction:
    return "illegal instruction";
  case FaultKind::FetchOutsideEnd:
    return "fetch outside program image";
  }
  return "?";
}

struct FaultReport {
  FaultKind kind = FaultKind::IllegalInstruction;
  uint32_t pc = 0; // fetch address that faulted
};

// run() returns the guest's exit code on success, or the fault that halted
// the machine on failure.
using RunOutcome = std::expected<int, FaultReport>;