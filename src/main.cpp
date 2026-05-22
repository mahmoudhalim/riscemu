#include "cpu.h"

int main() {
  CPU c{};
  c.run_all();
  c.print();
  return 0;
}
