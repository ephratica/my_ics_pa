#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

int current_game = 0;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet* schedule(_RegSet *prev) {
  // save the context pointer
  if (current)current->tf = prev;

  // TODO: switch to the new address space,
  // then return the new context

  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  static int count = 0;
  if(current != &pcb[2])count ++;
  else current = &pcb[current_game];

  if(count == 1000){
    current = &pcb[2];
    count = 0;
  }

  _switch(&current->as); 
  return current->tf;
}
