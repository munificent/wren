#include <string.h>
#include <time.h>

#include "benchmark.h"

static void arguments(WrenVM* vm, void *userData)
{
  double result = 0;

  result += wrenGetSlotDouble(vm, 1);
  result += wrenGetSlotDouble(vm, 2);
  result += wrenGetSlotDouble(vm, 3);
  result += wrenGetSlotDouble(vm, 4);

  wrenSetSlotDouble(vm, 0, result);
}

const char* testScript =
"class Test {\n"
"  static method(a, b, c, d) { a + b + c + d }\n"
"}\n";

static void call(WrenVM* vm, void *userData)
{
  int iterations = (int)wrenGetSlotDouble(vm, 1);

  // Since the VM is not re-entrant, we can't call from within this foreign
  // method. Instead, make a new VM to run the call test in.
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  WrenVM* otherVM = wrenNewVM(&config);

  wrenInterpret(otherVM, "main", testScript);

  WrenHandle* method = wrenMakeCallHandle(otherVM, "method(_,_,_,_)");

  wrenEnsureSlots(otherVM, 1);
  wrenGetVariable(otherVM, "main", "Test", 0);
  WrenHandle* testClass = wrenGetSlotHandle(otherVM, 0);

  double startTime = (double)clock() / CLOCKS_PER_SEC;

  double result = 0;
  for (int i = 0; i < iterations; i++)
  {
    wrenEnsureSlots(otherVM, 5);
    wrenSetSlotHandle(otherVM, 0, testClass);
    wrenSetSlotDouble(otherVM, 1, 1.0);
    wrenSetSlotDouble(otherVM, 2, 2.0);
    wrenSetSlotDouble(otherVM, 3, 3.0);
    wrenSetSlotDouble(otherVM, 4, 4.0);

    wrenCall(otherVM, method);

    result += wrenGetSlotDouble(otherVM, 0);
  }

  double elapsed = (double)clock() / CLOCKS_PER_SEC - startTime;

  wrenReleaseHandle(otherVM, testClass);
  wrenReleaseHandle(otherVM, method);
  wrenFreeVM(otherVM);

  if (result == (1.0 + 2.0 + 3.0 + 4.0) * iterations)
  {
    wrenSetSlotDouble(vm, 0, elapsed);
  }
  else
  {
    // Got the wrong result.
    wrenSetSlotBool(vm, 0, false);
  }
}

WrenForeignMethodFn benchmarkBindMethod(const char* signature)
{
  if (strcmp(signature, "static Benchmark.arguments(_,_,_,_)") == 0) return arguments;
  if (strcmp(signature, "static Benchmark.call(_)") == 0) return call;

  return NULL;
}
