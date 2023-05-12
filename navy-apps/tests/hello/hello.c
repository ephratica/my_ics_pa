#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

extern int _syscall_(int, intptr_t, intptr_t, intptr_t);

int main() {
  _syscall_(0, (intptr_t)1, (intptr_t)0, (intptr_t)0);
  _syscall_(1, (intptr_t)1, (intptr_t)0, (intptr_t)0);
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World for the %dth time\n", i ++);
      j = 0;
    }
  }
  return 0;
}
