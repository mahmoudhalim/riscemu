#include "cpu.h"
#include <cstdint>

int main() {
  CPU c;
  uint32_t IM[] = {0x007312b3};
  c.execute(IM[0]);
  // instr.print();
  c.print();
  return 0;
}