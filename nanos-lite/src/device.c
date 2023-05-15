#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  int key = _read_key();
  bool down = false;
  if (key == _KEY_NONE)
    sprintf((char *)buf, "t %d\n", _uptime());
  else {
    if (key & 0x8000) {
      key ^= 0x8000;
      down = 1;
    }
    if(down)sprintf((char *)buf, "kd %s\n", keyname[key]);
    else sprintf((char *)buf, "ku %s\n", keyname[key]);
  } 
  return strlen((char *)buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  offset >>= 2;
  len >>= 2;
  int x = offset % _screen.width, y = offset / _screen.width;
  int w1 = len < _screen.width - x? len: _screen.width - x, w2;
  _draw_rect((uint32_t *)buf, x, y, w1, 1);
  len -= w1;
  if(len > 0){
    w2 = len / _screen.width;
    _draw_rect((uint32_t *)(buf + w1), 0, y + 1, _screen.width, w2);
    len -= w2 * _screen.width;
  }
  if(len > 0){
    _draw_rect((uint32_t *)(buf + w1 + w2*_screen.width), 0, y + w2 + 1, len, 1);
  }
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", _screen.width, _screen.height);
}
