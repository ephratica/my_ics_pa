#include "common.h"
#include "syscall.h"

extern ssize_t fs_read(int fd, void *buf, size_t len);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);

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
    // Log("write. len:%d\n", (size_t)a[3]);
      SYSCALL_ARG1(r) = fs_write(a[1], (void *)a[2], a[3]);
      break;
    case SYS_brk:
    // Log("brk\n");
      // _heap.end = (void *)a[1];
      SYSCALL_ARG1(r) = 0;
      break;
    case SYS_open:
    // Log("open\n");
			SYSCALL_ARG1(r) = fs_open((char *)a[1], a[2], a[3]);
			break;
    case SYS_read:
    // Log("read, len:%d\n", a[3]);
			SYSCALL_ARG1(r) = fs_read(a[1], (void *)a[2], a[3]);
			break;
    case SYS_lseek:
      // Log("sleek\n");
      SYSCALL_ARG1(r) = fs_lseek(a[1], a[2], a[3]);
      break;
    case SYS_close:
      // Log("close\n");
      SYSCALL_ARG1(r) = fs_close(a[1]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
