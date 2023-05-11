#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // TODO();
  // assert(NO <= cpu.idtr.limit);
  if (NO > cpu.idtr.limit){
    printf("NO: 0x%x, limit: 0x%x\n", NO, cpu.idtr.limit);
    TODO();
  }
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  uint32_t idt_addr = cpu.idtr.base + NO*8;
  uint32_t eip_low = vaddr_read(idt_addr, 4);
  uint32_t eip_high = vaddr_read(idt_addr + 4, 4);

  decoding.jmp_eip = ((eip_low & 0x0000ffff) | (eip_high & 0xffff0000));
  // printf("jmp_eip: 0x%x\n", decoding.jmp_eip);
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
}
