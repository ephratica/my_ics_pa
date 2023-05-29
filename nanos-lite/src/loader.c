#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  Log("filename: %s\n", filename);
  int fd = fs_open(filename, 0, 0);
	Log("fd: %d\n", fd);
	fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd)); 
	fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
