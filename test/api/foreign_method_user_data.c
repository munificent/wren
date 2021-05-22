#include <string.h>

#include "foreign_method_user_data.h"

static const double zero = 0;
static const double answer = 42;
static const double elite = 1337;
static const double oneZeroOne = 101;
static const double oneZeroThree = 103;
static const double oneZeroFive = 105;

static void test(WrenVM* vm, void *userData)
{
  wrenSetSlotDouble(vm, 0, *(double *)userData);
}

WrenBindForeignMethodResult foreignMethodUserDataBindMethod(const char* signature)
{
  WrenBindForeignMethodResult retval = {0};
  retval.executeFn = test;
  retval.userData = (void *)&zero;  // avoid segfaults

  if (strcmp(signature, "static JustForeign.fortyTwo") == 0) {
    retval.userData = (void *)&answer;
  } else if (strcmp(signature, "static ForeignFirst.oneThreeThreeSeven") == 0) {
    retval.userData = (void *)&elite;
  } else if (strcmp(signature, "static ForeignSecond.oneZeroOne") == 0) {
    retval.userData = (void *)&oneZeroOne;
  } else if (strcmp(signature, "static Mixed.oneZeroThree") == 0) {
    retval.userData = (void *)&oneZeroThree;
  } else if (strcmp(signature, "static Mixed.oneZeroFive") == 0) {
    retval.userData = (void *)&oneZeroFive;
  }

  return retval;
}
