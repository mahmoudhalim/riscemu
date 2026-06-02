#include "core/cpu.h"
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <executable>\n";
    return 1;
  }
  CPU c{argv[1]};
  c.run();
  c.print();
  return 0;
}
