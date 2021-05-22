#include <stdio.h>
#include <string.h>

#include "error.h"

static void runtimeError(WrenVM* vm, void *userData)
{
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 0, "Error!");
  wrenAbortFiber(vm, 0);
}

WrenForeignMethodFn errorBindMethod(const char* signature)
{
  if (strcmp(signature, "static Error.runtimeError") == 0) return runtimeError;

  return NULL;
}
