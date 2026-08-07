#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>

// High-level simulator front-end. Owns the machine components (memory, the
// Syscall front-end, and the CPU core) behind an opaque Impl so consumers of
// the public API never see the internal headers. This is the only entry point
// the CLI needs.
struct SimulatorConfig {
  size_t memory_bytes = 4 * 1024 * 1024;
};

class Simulator {
public:
  explicit Simulator(SimulatorConfig config = {});
  ~Simulator();
  Simulator(const Simulator&) = delete;
  Simulator& operator=(const Simulator&) = delete;

  // Load a guest ELF into memory and reset the machine for execution.
  // Returns an error string instead of terminating the process.
  [[nodiscard]] std::expected<void, std::string>
  load(const std::filesystem::path& path);

  void step();
  void run();

  uint64_t instruction_count() const;
  bool halted() const;
  int exit_code() const;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};