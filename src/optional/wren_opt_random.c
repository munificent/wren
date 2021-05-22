#include "wren_opt_random.h"

#if WREN_OPT_RANDOM

#include <string.h>
#include <time.h>

#include "wren.h"
#include "wren_vm.h"

#include "wren_opt_random.wren.inc"

// Implements the well equidistributed long-period linear PRNG (WELL512a).
//
// https://en.wikipedia.org/wiki/Well_equidistributed_long-period_linear
typedef struct
{
  uint32_t state[16];
  uint32_t index;
} Well512;

// Code from: http://www.lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf
static uint32_t advanceState(Well512* well)
{
  uint32_t a, b, c, d;
  a = well->state[well->index];
  c = well->state[(well->index + 13) & 15];
  b =  a ^ c ^ (a << 16) ^ (c << 15);
  c = well->state[(well->index + 9) & 15];
  c ^= (c >> 11);
  a = well->state[well->index] = b ^ c;
  d = a ^ ((a << 5) & 0xda442d24U);

  well->index = (well->index + 15) & 15;
  a = well->state[well->index];
  well->state[well->index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
  return well->state[well->index];
}

static void randomAllocate(WrenVM* vm, void* userData)
{
  Well512* well = (Well512*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(Well512));
  well->index = 0;
}

static void randomSeed0(WrenVM* vm, void* userData)
{
  Well512* well = (Well512*)wrenGetSlotForeign(vm, 0);

  srand((uint32_t)time(NULL));
  for (int i = 0; i < 16; i++)
  {
    well->state[i] = rand();
  }
}

static void randomSeed1(WrenVM* vm, void* userData)
{
  Well512* well = (Well512*)wrenGetSlotForeign(vm, 0);

  srand((uint32_t)wrenGetSlotDouble(vm, 1));
  for (int i = 0; i < 16; i++)
  {
    well->state[i] = rand();
  }
}

static void randomSeed16(WrenVM* vm, void* userData)
{
  Well512* well = (Well512*)wrenGetSlotForeign(vm, 0);

  for (int i = 0; i < 16; i++)
  {
    well->state[i] = (uint32_t)wrenGetSlotDouble(vm, i + 1);
  }
}

static void randomFloat(WrenVM* vm, void* userData)
{
  Well512* well = (Well512*)wrenGetSlotForeign(vm, 0);

  // A double has 53 bits of precision in its mantissa, and we'd like to take
  // full advantage of that, so we need 53 bits of random source data.

  // First, start with 32 random bits, shifted to the left 21 bits.
  double result = (double)advanceState(well) * (1 << 21);

  // Then add another 21 random bits.
  result += (double)(advanceState(well) & ((1 << 21) - 1));

  // Now we have a number from 0 - (2^53). Divide be the range to get a double
  // from 0 to 1.0 (half-inclusive).
  result /= 9007199254740992.0;

  wrenSetSlotDouble(vm, 0, result);
}

static void randomInt0(WrenVM* vm, void* userData)
{
  Well512* well = (Well512*)wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, (double)advanceState(well));
}

const char* wrenRandomSource()
{
  return randomModuleSource;
}

WrenForeignClassMethods wrenRandomBindForeignClass(WrenVM* vm,
                                                   const char* module,
                                                   const char* className)
{
  ASSERT(strcmp(className, "Random") == 0, "Should be in Random class.");
  WrenForeignClassMethods methods = {0};
  methods.allocate = randomAllocate;
  return methods;
}

WrenBindForeignMethodResult wrenRandomBindForeignMethod(WrenVM* vm,
                                                const char* className,
                                                bool isStatic,
                                                const char* signature)
{
  WrenBindForeignMethodResult result = {0};
  ASSERT(strcmp(className, "Random") == 0, "Should be in Random class.");

  if (strcmp(signature, "<allocate>") == 0) result.executeFn = randomAllocate;
  else if (strcmp(signature, "seed_()") == 0) result.executeFn = randomSeed0;
  else if (strcmp(signature, "seed_(_)") == 0) result.executeFn = randomSeed1;

  else if (strcmp(signature, "seed_(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_)") == 0)
  {
    result.executeFn =  randomSeed16;
  }

  else if (strcmp(signature, "float()") == 0) result.executeFn = randomFloat;
  else if (strcmp(signature, "int()") == 0) result.executeFn = randomInt0;

  ASSERT(result.executeFn != NULL, "Unknown method.");
  return result;
}

#endif
