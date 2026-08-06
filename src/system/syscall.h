#pragma once

#include <cstdint>

class Memory;

class Syscall {
public:
  explicit Syscall(Memory& memory);

  struct Result {
    int32_t return_value = 0;
    uint8_t exit_code = 0;
    bool should_exit = false;
  };

  Result handle(uint32_t syscall_num, uint32_t a0, uint32_t a1, uint32_t a2,
                uint32_t a3);

private:
  // Safely above the ELF loader's highest segment (fib.elf .bss ends at
  // 0x13a5c) and far below the stack top (memory size - 16).
  static constexpr uint32_t HEAP_START = 0x20000;

  Memory& memory_;
  uint32_t heap_break_ = HEAP_START; // bump allocator state for brk

  Result sys_exit(uint32_t code);
  Result sys_write(uint32_t fd, uint32_t buf_addr, uint32_t count);
  Result sys_brk(uint32_t new_brk);
  Result sys_fstat(uint32_t fd, uint32_t statbuf_addr);
  Result sys_read(uint32_t fd, uint32_t buf_addr, uint32_t count);
  Result sys_close(uint32_t fd);
};
