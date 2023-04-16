#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);


// control
make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(jmp_rm);
make_EHelper(call);
make_EHelper(ret);
make_EHelper(call_rm);

// mov data
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);

// arith
make_EHelper(sub);
make_EHelper(add);

// logic
make_EHelper(xor);
make_EHelper(and);

// special
make_EHelper(nop);
