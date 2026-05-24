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

TEST(RegistersTest, NormalReadWriteRoundTrips) {
  Registers regs;

  EXPECT_EQ(regs.read(1), 0u);
  regs.write(1, 123u);
  regs.write(2, 456u);

  EXPECT_EQ(regs.read(1), 123u);
  EXPECT_EQ(regs.read(2), 456u);
}
