#include <stdio.h>
#include <string.h>

#include "wren.h"

static void values(WrenVM *vm) {
  wrenSetForeignReturnValues(vm, 4);
  wrenEnsureSlots(vm, 4);
  wrenSetSlotDouble(vm, 0, 1);       // slot, value
  wrenSetSlotDouble(vm, 1, 2);
  wrenSetSlotDouble(vm, 2, 3);
  wrenSetSlotDouble(vm, 3, 4);
}

WrenForeignMethodFn multipleReturnValuesForeignBindMethod(const char* signature)
{
  if (strcmp(signature, "static MultipleReturnValuesForeign.values()") == 0) return values;

  return NULL;
}
