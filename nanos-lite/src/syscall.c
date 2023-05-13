#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  // Log("a[0]: %d\n", (int)a[0]);

  switch (a[0]) {
    case SYS_none:
    // Log("0\n");
			SYSCALL_ARG1(r) = 1;
			break;
    case SYS_exit:
    // Log("4\n");
      _halt(a[1]);
      break;
    case SYS_write:
    Log("3\n");
      switch ((int)a[1]){
        case 1:
        case 2:
          for(int i = 0; i < (size_t)a[3]; i++) {
            _putc(((char*)a[2])[i]);
            // Log("%c", ((char*)a[2])[i]);
            // Log("\n");
          }
          break;
        default:
          // Log("a[1]: %d\n", (int)a[1]);
          break;
      }
      break;
    case SYS_brk:
      _heap.end = (void *)a[1];
      SYSCALL_ARG1(r) = 0;
      break;
    case SYS_open:
    // Log("1\n");
			SYSCALL_ARG1(r) = 1;
			break;
    case SYS_read:
    // Log("2\n");
			SYSCALL_ARG1(r) = 1;
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
