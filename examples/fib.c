volatile unsigned int result;

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

int main() {
  // Calculate the 10th Fibonacci number (55, or 0x37 in hex)
  result = fibonacci(10);

  // The default crt0 calls exit() -> ECALL SYS_exit(93), terminating cleanly.
  return 0;
}
