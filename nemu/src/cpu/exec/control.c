#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t code = decoding.opcode & 0xf;
  rtl_setcc(&t2, code);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(code), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  // TODO();

  rtl_push(&decoding.seq_eip);
	decoding.is_jmp = 1;
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  // TODO();
  rtl_pop(&decoding.jmp_eip);
	decoding.is_jmp = 1;
  print_asm("ret");
}

make_EHelper(call_rm) {
  // TODO();
  rtl_push(&decoding.seq_eip);
	decoding.jmp_eip = id_dest->val;
	decoding.is_jmp = 1;
  print_asm("call *%s", id_dest->str);
}
