#include <unistd.h>

int main(void) {
  const char msg[] = "write-ok\n";
  ssize_t n = write(1, msg, sizeof(msg) - 1);
  return (n == sizeof(msg) - 1) ? 0 : 1;
}
