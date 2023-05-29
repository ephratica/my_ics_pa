#include "common.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);
extern size_t get_ramdisk_size();
extern void _map(_Protect *p, void *va, void *pa);

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);

  int pages = fs_filesz(fd) / PGSIZE + 1;
  void *pa, *va = DEFAULT_ENTRY;
  
  for (int i = 0; i < pages; ++i, va += PGSIZE) {
    pa = new_page();
    fs_read(fd, pa, PGSIZE);
    _map(as, va, pa);
  }
	fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd)); 
	fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
