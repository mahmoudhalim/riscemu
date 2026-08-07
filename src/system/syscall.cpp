#include "syscall.h"

#include "logging/logger.h"
#include "memory/memory.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

namespace {
// S_IFCHR from the toolchain's sys/stat.h, placed at st_mode (offset 4 in
// the RV32 newlib struct stat, verified against the installed headers).
constexpr uint32_t STAT_MODE_OFFSET = 4;
constexpr uint32_t S_IFCHR = 0x2000;

constexpr std::string_view name(uint32_t num) {
  switch (num) {
  case 57:
    return "close";
  case 63:
    return "read";
  case 64:
    return "write";
  case 80:
    return "fstat";
  case 93:
    return "exit";
  case 214:
    return "brk";
  default:
    return "unknown";
  }
}
} // namespace

Syscall::Syscall(Memory& memory) : memory_(memory) {}

Syscall::Result Syscall::handle(uint32_t syscall_num, uint32_t a0, uint32_t a1,
                                uint32_t a2, uint32_t a3) {
  logging::log(logging::Level::Debug, "syscall",
               "{} num={} a0={} a1={} a2={} a3={}", name(syscall_num),
               syscall_num, a0, a1, a2, a3);
  Result res;
  switch (syscall_num) {
  case 93:
    res = sys_exit(a0);
    break;
  case 64:
    res = sys_write(a0, a1, a2);
    break;
  case 214:
    res = sys_brk(a0);
    break;
  case 80:
    res = sys_fstat(a0, a1);
    break;
  case 63:
    res = sys_read(a0, a1, a2);
    break;
  case 57:
    res = sys_close(a0);
    break;
  default:
    logging::log(logging::Level::Warn, "syscall",
                 "unimplemented: num={} a0={} a1={} a2={} a3={}", syscall_num,
                 a0, a1, a2, a3);
    res = {.return_value = -1};
    break;
  }
  logging::log(logging::Level::Trace, "syscall", "{} -> {}", name(syscall_num),
               res.return_value);
  return res;
}

Syscall::Result Syscall::sys_exit(uint32_t code) {
  return {.return_value = 0,
          .exit_code = static_cast<uint8_t>(code),
          .should_exit = true};
}

Syscall::Result Syscall::sys_write(uint32_t fd, uint32_t buf_addr,
                                   uint32_t count) {
  if (fd != 1 && fd != 2) {
    return {.return_value = -1};
  }
  std::vector<uint8_t> data = memory_.read_span(buf_addr, count);
  std::ostream& out = (fd == 2) ? std::cerr : std::cout;
  out.write(reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size()));
  out.flush();
  return {.return_value = static_cast<int32_t>(count)};
}

Syscall::Result Syscall::sys_brk(uint32_t new_brk) {
  if (new_brk == 0) {
    return {.return_value = static_cast<int32_t>(heap_break_)};
  }
  // Only grow the break; any requested address below the current one is
  // rejected (returns the previous break unchanged).
  if (new_brk < heap_break_) {
    return {.return_value = static_cast<int32_t>(heap_break_)};
  }
  heap_break_ = new_brk;
  return {.return_value = static_cast<int32_t>(heap_break_)};
}

Syscall::Result Syscall::sys_fstat(uint32_t, uint32_t statbuf_addr) {
  // Minimal stub: present stdout as a character device so newlib picks
  // line-buffered output. Only st_mode is populated; the rest stays zero.
  memory_.write_word(statbuf_addr + STAT_MODE_OFFSET, S_IFCHR);
  return {.return_value = 0};
}

Syscall::Result Syscall::sys_read(uint32_t fd, uint32_t buf_addr,
                                  uint32_t count) {
  if (fd != 0) {
    return {.return_value = -1};
  }
  std::vector<uint8_t> buf(count);
  size_t n = std::fread(buf.data(), 1, count, stdin);
  for (size_t i = 0; i < n; ++i) {
    memory_.write_byte(buf_addr + static_cast<uint32_t>(i), buf[i]);
  }
  return {.return_value = static_cast<int32_t>(n)};
}

Syscall::Result Syscall::sys_close(uint32_t) { return {.return_value = 0}; }
