#include "core/cpu.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

namespace {

class CaptureStdout {
public:
  CaptureStdout() { old_ = std::cout.rdbuf(stream_.rdbuf()); }
  ~CaptureStdout() { std::cout.rdbuf(old_); }
  std::string str() const { return stream_.str(); }

private:
  std::stringstream stream_;
  std::streambuf* old_ = nullptr;
};

std::filesystem::path elf_path(const char* name) {
  return std::filesystem::path(RISCEMU_INTEGRATION_DIR) / name;
}

} // namespace

// Requirement 2: a minimal C program with just `return 42;` should only
// exercise SYS_exit and terminate with exit code 42 through the full
// ELF loader + CPU + Syscall pipeline.
TEST(IntegrationTest, Exit42TraversesCrtAndHonorsExitCode) {
  CPU cpu(elf_path("exit42.elf"));
  cpu.run();
  EXPECT_TRUE(cpu.halted());
  EXPECT_EQ(cpu.exit_code(), 42);
}

// Requirement 3: a program calling write() directly (no stdio init) emits
// the exact bytes and exits 0.
TEST(IntegrationTest, DirectWriteEmitsBytes) {
  CPU cpu(elf_path("write_test.elf"));
  CaptureStdout capture;
  cpu.run();
  EXPECT_TRUE(cpu.halted());
  EXPECT_EQ(cpu.exit_code(), 0);
  EXPECT_NE(capture.str().find("write-ok\n"), std::string::npos);
}

// Requirement 4: full printf exercises brk + fstat + write together.
TEST(IntegrationTest, PrintfExercisesBrkFstatWrite) {
  CPU cpu(elf_path("printf_test.elf"));
  CaptureStdout capture;
  cpu.run();
  EXPECT_TRUE(cpu.halted());
  EXPECT_EQ(cpu.exit_code(), 0);
  EXPECT_NE(capture.str().find("printf-ok\n"), std::string::npos);
}