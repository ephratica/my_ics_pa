#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[256];
  uint32_t value;

} WP;

void init_wp_pool();
WP* new_wp();
void free_wp(int no);
void Print_wp();
int change_list();

#endif
