#include <stdio.h>
#include <string.h>

#include "foreign_class_user_data.h"

// WrenForeignClassMethods.userData testing:
// - Apple and Banana have different userData values.
// - We test allocate() by retrieving the userData value from the
//   created instance.
// - We test finalize() by storing the userData value into this file and
//   retrieving it using a static property of Banana.
static const double AppleUserData = 12345;
static const double BananaAllocateUserData = 8;
static const double BananaFinalizeUserData = 57005; // 0xdead

// Where bananaFinalize() will store userData.  The initial value is so we know
// bananaFinalize() hasn't been called.
static double bananaFinalizeResult = 404;

static void instanceAllocate(WrenVM* vm, void *allocateUserData)
{
  double* instance = (double*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(double));
  *instance = *(double *)allocateUserData;
}

// Return a foreign class's userdata
static void getClassUserData(WrenVM* vm, void *methodUserData)
{
  double* instance = (double*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, *instance);
}

static void bananaFinalize(void *data, void *userData)
{
  bananaFinalizeResult = *(double *)userData;
}

static void bananaGetFinalizeResult(WrenVM* vm, void *userData)
{
  wrenSetSlotDouble(vm, 0, bananaFinalizeResult);
}

WrenForeignMethodFn foreignClassUserDataBindMethod(const char* signature)
{
  if (strcmp(signature, "Apple.instanceUserData") == 0) return getClassUserData;
  if (strcmp(signature, "Banana.instanceUserData") == 0) return getClassUserData;
  if (strcmp(signature, "static Banana.finalizeResult") == 0) return bananaGetFinalizeResult;

  return NULL;
}

void foreignClassUserDataBindClass(
    const char* className, WrenForeignClassMethods* methods)
{
  if (strcmp(className, "Apple") == 0)
  {
    methods->allocate = instanceAllocate;
    methods->allocateUserData = (void *)&AppleUserData;
    return;
  }

  if (strcmp(className, "Banana") == 0)
  {
    methods->allocate = instanceAllocate;
    methods->finalize = bananaFinalize;
    methods->allocateUserData = (void *)&BananaAllocateUserData;
    methods->finalizeUserData = (void *)&BananaFinalizeUserData;
    return;
  }
}
