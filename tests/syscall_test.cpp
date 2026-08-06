#include "memory/memory.h"
#include "system/syscall.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

namespace {

class SyscallTest : public ::testing::Test {
protected:
  Memory mem_{4 * 1024 * 1024};
  Syscall syscall_{mem_};
};

// Redirect std::cout for the duration of a sys_write test.
class CaptureStdout {
public:
  CaptureStdout() { old_ = std::cout.rdbuf(stream_.rdbuf()); }
  ~CaptureStdout() { std::cout.rdbuf(old_); }
  std::string str() const { return stream_.str(); }

private:
  std::stringstream stream_;
  std::streambuf* old_ = nullptr;
};

} // namespace

TEST_F(SyscallTest, ExitSetsShouldExitWithCode) {
  auto result = syscall_.handle(93, 42, 0, 0, 0);
  EXPECT_TRUE(result.should_exit);
  EXPECT_EQ(result.exit_code, 42);
}

TEST_F(SyscallTest, WriteToStdoutEmitsBytes) {
  const char data[] = "riscemu";
  mem_.load_segment(0x1000, reinterpret_cast<const uint8_t*>(data),
                    sizeof(data) - 1);

  CaptureStdout capture;
  auto result = syscall_.handle(64, 1, 0x1000, 7, 0);

  EXPECT_EQ(result.return_value, 7);
  EXPECT_FALSE(result.should_exit);
  EXPECT_EQ(capture.str(), "riscemu");
}

TEST_F(SyscallTest, WriteToUnsupportedFdReturnsMinusOne) {
  auto result = syscall_.handle(64, 3, 0x1000, 4, 0);
  EXPECT_EQ(result.return_value, -1);
}

TEST_F(SyscallTest, BrkQueryReturnsStart) {
  auto result = syscall_.handle(214, 0, 0, 0, 0);
  EXPECT_EQ(result.return_value, 0x20000);
}

TEST_F(SyscallTest, BrkBumpsAndStaysMonotonic) {
  syscall_.handle(214, 0x21000, 0, 0, 0);
  auto result = syscall_.handle(214, 0, 0, 0, 0);
  EXPECT_EQ(result.return_value, 0x21000);

  // Moving the break backwards is rejected.
  auto back = syscall_.handle(214, 0x1F000, 0, 0, 0);
  EXPECT_EQ(back.return_value, 0x21000);
}

TEST_F(SyscallTest, FstatWritesCharMode) {
  auto result = syscall_.handle(80, 1, 0x2000, 0, 0);
  EXPECT_EQ(result.return_value, 0);
  EXPECT_EQ(mem_.read_word(0x2000 + 4), 0x2000u); // st_mode = S_IFCHR
}

TEST_F(SyscallTest, CloseReturnsZero) {
  auto result = syscall_.handle(57, 1, 0, 0, 0);
  EXPECT_EQ(result.return_value, 0);
}

TEST_F(SyscallTest, UnimplementedSyscallReturnsMinusOne) {
  auto result = syscall_.handle(999, 0, 0, 0, 0);
  EXPECT_EQ(result.return_value, -1);
}