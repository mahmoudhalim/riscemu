#include "cpu.h"
#include "core/decoder.h"
#include "core/executor.h"
#include "loader/elf_loader.h"
#include "logging/logger.h"

CPU::CPU(std::filesystem::path path) : mem_(4 * 1024 * 1024), syscall_(mem_) {
  auto res = ELFLoader::load(path, mem_);
  if (!res) {
    logging::log(logging::Level::Error, "elf", "failed to load '{}': {}",
                 path.string(), res.error());
    exit(1);
  }
  regs_.pc = res->entry_point;
  program_end_addr_ = res->max_addr;
  uint32_t STACK_TOP = mem_.size_bytes() - 16; // Near top of memory
  regs_.write(2, STACK_TOP);
  logging::log(logging::Level::Info, "elf",
               "loaded '{}': entry=0x{:08x} "
               "end=0x{:08x} sp=0x{:08x}",
               path.string(), res->entry_point, res->max_addr, STACK_TOP);
}

void CPU::step() {
  uint32_t cur_pc = regs_.pc;
  auto ir = Decoder::decode(mem_.read_word(cur_pc));
  logging::log(logging::Level::Trace, "cpu", "0x{:08x}: {}", cur_pc,
               to_string(ir.type));
  auto result = Executor::execute(ir, regs_, mem_, syscall_);
  ++instruction_count_;
  if (result.halt) {
    halted_ = true;
    exit_code_ = result.exit_code;
  }
}

void CPU::run() {
  logging::log(logging::Level::Info, "cpu", "starting at 0x{:08x}", regs_.pc);
  while (regs_.pc < program_end_addr_ && !halted_) {
    step();
  }
  logging::log(logging::Level::Info, "cpu",
               "halted: exit_code={} after {} "
               "instructions",
               exit_code_, instruction_count_);
}