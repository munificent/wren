#ifndef wren_math_h
#define wren_math_h

#include <math.h>
#include <stdint.h>

// A union to let us reinterpret a double as raw bits and back.
typedef union
{
  uint64_t bits64;
  uint32_t bits32[2];
  double num;
} WrenDoubleBits;

#define WREN_DOUBLE_QNAN_POS_MIN_BITS (UINT64_C(0x7FF8000000000000))
#define WREN_DOUBLE_QNAN_POS_MAX_BITS (UINT64_C(0x7FFFFFFFFFFFFFFF))

#define WREN_DOUBLE_NAN (wrenDoubleFromBits(WREN_DOUBLE_QNAN_POS_MIN_BITS))

double wrenDoubleFromBits(uint64_t bits);

uint64_t wrenDoubleToBits(double num);

#endif
