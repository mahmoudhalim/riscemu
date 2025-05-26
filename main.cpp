#include "cpu.h"
#include <cstdint>

int main() {
  cpu c;
  uint32_t IM[] = {0x007302b3};
  auto instr = Rinstruction(IM[0]);
  c.exec(instr);
  // instr.print();
  c.print();
  return 0;
}