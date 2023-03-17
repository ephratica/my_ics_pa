#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static WP list[NR_WP];

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
  if(free_ == NULL){
    Log("sorry, watchpointer drained.\n");
    return NULL;
  }
  else{
    WP *ret = free_;
    free_ = free_->next;
    ret->next = head;
    head = ret;
    return ret;
  }
}

void free_wp(WP *wp){
  head = wp->next;
  wp->next = free_;
  free_ = wp;
}

void Print_wp(){
  if (head == NULL) {
    printf("no watchpoint\n");
    return ;
  }

  WP *wp = head;
  while (wp) {
    printf("%d\t%s\t0x%08x\n", wp->NO, wp->expr, wp->value);
    wp = wp->next;
  }

  return;
}

void change_list() {
  WP *wp = head;
  bool flag = true;
  int cnt = 0;

  while (wp) {
    uint32_t value;
    value = expr(wp->expr, &flag);
    if (value != wp->value) {
      wp->value = value;
      list[cnt++] = *wp;
    }
      wp = wp->next;
  }
  list[cnt].NO = -1;
  return ;
}