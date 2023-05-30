#include <unistd.h>
#include <stdio.h>

// typedef int			intptr_t;
// extern void* _sbrk(intptr_t increment);

int main() {
  // _sbrk(0);
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (j < 35000) {
    j ++;
    if (j % 10000 == 0) {
      printf("Hello World for the %dth time\n", i ++);
      j = 0;
    }
  }
  return 0;
}
