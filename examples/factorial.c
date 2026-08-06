#include <stdio.h>

// Recursive factorial: demonstrates call/return through JAL/JALR and the
// software stack.
static long factorial(int n) {
  if (n <= 1)
    return 1;
  return n * factorial(n - 1);
}

int main(void) {
  printf("10! = %ld\n", factorial(10));
  return 0;
}