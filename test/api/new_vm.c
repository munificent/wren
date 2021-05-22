#include <string.h>

#include "new_vm.h"

static void nullConfig(WrenVM* vm, void *userData)
{
  WrenVM* otherVM = wrenNewVM(NULL);

  // We should be able to execute code.
  WrenInterpretResult result = wrenInterpret(otherVM, "main", "1 + 2");
  wrenSetSlotBool(vm, 0, result == WREN_RESULT_SUCCESS);

  wrenFreeVM(otherVM);
}

static void multipleInterpretCalls(WrenVM* vm, void *userData)
{
  WrenVM* otherVM = wrenNewVM(NULL);
  WrenInterpretResult result;

  bool correct = true;

  // Handles should be valid across calls into Wren code.
  WrenHandle* absMethod = wrenMakeCallHandle(otherVM, "abs");

  result = wrenInterpret(otherVM, "main", "import \"random\" for Random");
  correct = correct && (result == WREN_RESULT_SUCCESS);

  for (int i = 0; i < 5; i++) {
    // Calling `wrenEnsureSlots()` before `wrenInterpret()` should not introduce
    // problems later.
    wrenEnsureSlots(otherVM, 2);

    // Calling a foreign function should succeed.
    result = wrenInterpret(otherVM, "main", "Random.new(12345)");
    correct = correct && (result == WREN_RESULT_SUCCESS);

    wrenEnsureSlots(otherVM, 2);
    wrenSetSlotDouble(otherVM, 0, -i);
    result = wrenCall(otherVM, absMethod);
    correct = correct && (result == WREN_RESULT_SUCCESS);

    double absValue = wrenGetSlotDouble(otherVM, 0);
    correct = correct && (absValue == (double)i);
  }

  wrenSetSlotBool(vm, 0, correct);

  wrenReleaseHandle(otherVM, absMethod);
  wrenFreeVM(otherVM);
}

WrenForeignMethodFn newVMBindMethod(const char* signature)
{
  if (strcmp(signature, "static VM.nullConfig()") == 0) return nullConfig;
  if (strcmp(signature, "static VM.multipleInterpretCalls()") == 0) return multipleInterpretCalls;

  return NULL;
}
