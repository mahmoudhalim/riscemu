#include <stdio.h>

// Byte-order manipulation on RV32I: reverse the four bytes of an integer
// (0x12345678 <-> 0x78563412) using only shifts and masks.
static unsigned int bswap(unsigned int x) {
  return ((x & 0x000000FFu) << 24) | ((x & 0x0000FF00u) << 8) |
         ((x & 0x00FF0000u) >> 8) | ((x & 0xFF000000u) >> 24);
}

int main(void) {
  unsigned int v = 0x12345678;
  printf("0x%08x -> 0x%08x\n", v, bswap(v));
  return 0;
}