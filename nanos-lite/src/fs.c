#include "fs.h"

extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = _screen.width * _screen.height * 4;
}

int fs_open(const char *pathname, int flags, int mode){
  for(int i = 0; i < NR_FILES; i++){
    if(strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      return i;
    }
  }
  Log("no such file %s\n", pathname);
  assert(0);
  return 0;
}

size_t fs_filesz(int fd){
  return file_table[fd].size;
}


ssize_t fs_read(int fd, void *buf, size_t len) {
	ssize_t f_size = fs_filesz(fd);
	switch(fd) {
		case FD_STDOUT:
		case FD_FB:
			break;
    case FD_EVENTS:
    Log("FD_EVENTS\n");
			break;
		case FD_DISPINFO:	
    Log("FD_DISPINFO\n");
			break;
		default:
      len = len < f_size - file_table[fd].open_offset ? len: f_size - file_table[fd].open_offset;
			ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
      assert(file_table[fd].open_offset <= f_size);
			break;
	}
	return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
	ssize_t f_size = fs_filesz(fd);
	switch(fd) {
		case FD_STDOUT:
		case FD_STDERR:
			for(int i = 0; i < len; i++) {
					_putc(((char*)buf)[i]);
			}
			break;
		default:
      if(file_table[fd].open_offset + len > f_size)
        return 0;
			len = len < f_size - file_table[fd].open_offset ? len: f_size - file_table[fd].open_offset;
			ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
      assert(file_table[fd].open_offset <= f_size);
			break;
	}
	return len;
}

off_t fs_lseek(int fd, off_t offset, int whence){
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  
  default:
    return -1;
    break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd){
  return 0;
}
