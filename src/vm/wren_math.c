#include "wren_math.h"

inline double wrenDoubleFromBits(uint64_t bits)
{
  WrenDoubleBits data;
  data.bits64 = bits;
  return data.num;
}

inline uint64_t wrenDoubleToBits(double num)
{
  WrenDoubleBits data;
  data.num = num;
  return data.bits64;
}
