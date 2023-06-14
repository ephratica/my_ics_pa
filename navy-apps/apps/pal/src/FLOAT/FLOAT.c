#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  return ((uint64_t)a * (uint64_t)b) >> 16;;
}

FLOAT Fabs(FLOAT x) { return x < 0 ? -x : x;}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  assert(b != 0);
  uint32_t sign = ((a ^ b) & 0x80000000) >> 31;
  a = Fabs(a);
  b = Fabs(b);
  int result = (a / b) << 16;
  // result |= (((uint64_t)(a%b) << 16) / (uint64_t)b) & 0x0000ffff;
  uint32_t MOD = a%b, left;
	for (int i = 0; i < 16; i++, MOD <<= 1, left <<= 1) {
		if (MOD >= b) {
			MOD -= b;
			left++;
		}
	}
  result |= left;
  return sign ? -result : result;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

  uint32_t *f = (uint32_t *)&a;
  uint32_t f_sign = (*f & 0x80000000) >> 31;
  uint32_t f_exp = (*f & 0x7f800000) >> 23;
  uint32_t f_frac = *f & 0x007fffff;
  uint32_t F_exp, F_frac, result;

  
  if (f_exp == 0xff) { // NaN, Inf
    assert(0);
  } 
  else if (f_exp == 0) { // Subnormal numbers
    F_exp = 1 - 127;
    F_frac = f_frac;
  } 
  else {
    F_exp = f_exp - 127;
    F_frac = f_frac | (1 << 23);
  }
<<<<<<< HEAD

  if (F_exp >= 7 && F_exp < 22) result = F_frac << (F_exp - 7);
  else if (F_exp < 7) result = F_frac >> (7 - F_exp);

=======

  if (F_exp >= 7 && F_exp < 22) result = F_frac << (F_exp - 7);
  else if (F_exp < 7) result = F_frac >> (7 - F_exp);

>>>>>>> pa5-test
  return f_sign? -result : result;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}