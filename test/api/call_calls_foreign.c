#include <stdio.h>
#include <string.h>

#include "wren.h"

static void api(WrenVM *vm, void *userData) {
  // Grow the slot array. This should trigger the stack to be moved.
  wrenEnsureSlots(vm, 10);
  wrenSetSlotNewList(vm, 0);

  for (int i = 1; i < 10; i++)
  {
    wrenSetSlotDouble(vm, i, i);
    wrenInsertInList(vm, 0, -1, i);
  }
}

WrenForeignMethodFn callCallsForeignBindMethod(const char* signature)
{
  if (strcmp(signature, "static CallCallsForeign.api()") == 0) return api;

  return NULL;
}

int callCallsForeignRunTests(WrenVM* vm)
{
  wrenEnsureSlots(vm, 1);
  wrenGetVariable(vm, "./test/api/call_calls_foreign", "CallCallsForeign", 0);
  WrenHandle* apiClass = wrenGetSlotHandle(vm, 0);
  WrenHandle *call = wrenMakeCallHandle(vm, "call(_)");

  wrenEnsureSlots(vm, 2);
  wrenSetSlotHandle(vm, 0, apiClass);
  wrenSetSlotString(vm, 1, "parameter");

  printf("slots before %d\n", wrenGetSlotCount(vm));
  wrenCall(vm, call);

  // We should have a single slot count for the return.
  printf("slots after %d\n", wrenGetSlotCount(vm));

  wrenReleaseHandle(vm, call);
  wrenReleaseHandle(vm, apiClass);
  return 0;
}
