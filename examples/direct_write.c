#include <string.h>
#include <unistd.h>

// Raw write() syscall without newlib's stdio layer: no brk/fstat involved.
static void emit(const char* s) { write(1, s, (unsigned)strlen(s)); }

int main(void) {
  const char* lines[] = {"three\n", "two\n", "one\n", "liftoff!\n"};
  for (int i = 0; i < 4; i++)
    emit(lines[i]);
  return 0;
}