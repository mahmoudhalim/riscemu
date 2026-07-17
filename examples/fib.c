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

  // Trapping loop to stop the simulation gracefully
  while (1)
    ;
  return 0;
}
