#include "core/registers.h"

#include <gtest/gtest.h>

TEST(RegistersTest, RegisterZeroIsImmutable) {
  Registers regs;
  regs.write(0, 1234);

  EXPECT_EQ(regs.read(0), 0u);
}

TEST(RegistersTest, OutOfRangeReadsReturnZero) {
  Registers regs;

  EXPECT_EQ(regs.read(33), 0u);
}

TEST(RegistersTest, OutOfRangeWritesAreIgnored) {
  Registers regs;
  regs.write(33, 999);

  EXPECT_EQ(regs.read(33), 0u);
}
