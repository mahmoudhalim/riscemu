#include <stdio.h>

unsigned int fibonacci(int n) {
  if (n <= 0)
    return 0;
  if (n == 1)
    return 1;

  unsigned int prev2 = 0;
  unsigned int prev1 = 1;
  unsigned int current = 0;

  for (int i = 2; i <= n; i++) {
    current = prev1 + prev2;
    prev2 = prev1;
    prev1 = current;
  }
  return current;
}

int main(void) {
  printf("fibonacci(10) = %u\n", fibonacci(10));
  return 0;
}