#include "riscv/simulator.h"

#include <cstdint>

#include "core/cpu.h"
#include "loader/elf_loader.h"
#include "memory/memory.h"
#include "riscv/logging.h"
#include "system/syscall.h"

struct Simulator::Impl {
  // Member order matters: each component depends on the previous one.
  Memory memory;
  Syscall syscall;
  CPU cpu;

  explicit Impl(size_t memory_bytes)
      : memory(memory_bytes), syscall(memory), cpu(memory, syscall) {}
};

Simulator::Simulator(SimulatorConfig config)
    : impl_(std::make_unique<Impl>(config.memory_bytes)) {}

Simulator::~Simulator() = default;

std::expected<void, std::string>
Simulator::load(const std::filesystem::path& path) {
  auto res = ELFLoader::load(path, impl_->memory);
  if (!res) {
    return std::unexpected(res.error());
  }
  const uint32_t stack_top =
      static_cast<uint32_t>(impl_->memory.size_bytes()) - 16;
  impl_->cpu.initialize(res->entry_point, res->max_addr, stack_top);
  logging::log(logging::Level::Info, "elf",
               "loaded '{}': entry=0x{:08x} end=0x{:08x} sp=0x{:08x}",
               path.string(), res->entry_point, res->max_addr, stack_top);
  return {};
}

void Simulator::step() { impl_->cpu.step(); }

RunOutcome Simulator::run() { return impl_->cpu.run(); }

uint64_t Simulator::instruction_count() const {
  return impl_->cpu.instruction_count();
}

bool Simulator::halted() const { return impl_->cpu.halted(); }

int Simulator::exit_code() const { return impl_->cpu.exit_code(); }