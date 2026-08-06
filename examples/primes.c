#include <stdio.h>

// Trial-division primality loop with integer sqrt approximation.
static int is_prime(int n) {
  if (n < 2)
    return 0;
  for (int d = 2; d * d <= n; d++) {
    if (n % d == 0)
      return 0;
  }
  return 1;
}

int main(void) {
  int primes[32];
  int count = 0;
  for (int n = 2; n < 100; n++) {
    if (is_prime(n))
      primes[count++] = n;
  }

  printf("primes < 100 (%d found):\n", count);
  for (int i = 0; i < count; i++) {
    printf("%d ", primes[i]);
  }
  printf("\n");
  return 0;
}