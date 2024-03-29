#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);
extern _RegSet* schedule(_RegSet *prev);

static _RegSet* do_event(_Event e, _RegSet* r) {
  // Log("event: %d\n", e.event);
  switch (e.event) {
    case _EVENT_SYSCALL:
    // Log("event\n");
      do_syscall(r);
			return schedule(r);
      // return do_syscall(r);
      break;
    case _EVENT_TRAP: 
      return schedule(r);
      break;
    case _EVENT_IRQ_TIME: 
      // Log("_EVENT_IRQ_TIME\n");
      // return schedule(r);
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
