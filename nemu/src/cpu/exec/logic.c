#include "cpu/exec.h"

make_EHelper(test) {
  // TODO();
  rtl_and(&t1, &id_dest->val, &id_src->val);
  // printf("t0:0x%x width:%d\n", t0, id_dest->width);
  rtl_update_ZFSF(&t1, id_dest->width);
  
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
	rtl_and(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  // printf("result:0x%x\n", t0);

	rtl_update_ZFSF(&t1, id_dest->width);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtl_xor(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  rtl_update_ZFSF(&t1, id_dest->width);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
	rtl_or(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  // printf("result:0x%x\n", t0);

	rtl_update_ZFSF(&t1, id_dest->width);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sext(&t0, &id_dest->val, id_dest->width);
  rtl_sar(&t1, &t0, &id_src->val);
  operand_write(id_dest, &t1);

  rtl_update_ZFSF(&t1, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  // TODO();
  t0 = id_dest->val;
  rtl_not(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(not);
}

make_EHelper(rol){
  rtl_mv(&t2, &id_dest->val);
  for (int i=0;i<id_src->val;i++) {
    rtl_msb(&t1, &t2, id_dest->width);
    rtl_shli(&t2, &t2, 1);
    rtl_or(&t2, &t2, &t1);
  }
  operand_write(id_dest, &t2);
  rtl_set_CF(&t1);
  rtl_msb(&t2, &t2, id_dest->width);
  rtl_xor(&t1, &t2, &t1);
  rtl_set_OF(&t1);
  
  print_asm_template2(rol);
}

make_EHelper(shrd) {
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  if(decoding.is_operand_size_16){
    t1 = 16;
  }
  else{
    t1 = 32;
  }
  rtl_sub(&t1, &t1, &id_src->val);
  rtl_shl(&t3, &id_src2->val, &t1);

  rtl_or(&t0, &t0, &t3);
  operand_write(id_dest, &t0);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(shrd);
}

make_EHelper(shld) {
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  if(decoding.is_operand_size_16){
    t1 = 16;
  }
  else{
    t1 = 32;
  }
  rtl_sub(&t1, &t1, &id_src->val);
  rtl_shr(&t3, &id_src2->val, &t1);

  rtl_or(&t0, &t0, &t3);
  operand_write(id_dest, &t0);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(shld);
}
