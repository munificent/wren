#include <string.h>

#include "get_variable.h"

static void beforeDefined(WrenVM* vm, void *userData)
{
  wrenGetVariable(vm, "./test/api/get_variable", "A", 0);
}

static void afterDefined(WrenVM* vm, void *userData)
{
  wrenGetVariable(vm, "./test/api/get_variable", "A", 0);
}

static void afterAssigned(WrenVM* vm, void *userData)
{
  wrenGetVariable(vm, "./test/api/get_variable", "A", 0);
}

static void otherSlot(WrenVM* vm, void *userData)
{
  wrenEnsureSlots(vm, 3);
  wrenGetVariable(vm, "./test/api/get_variable", "B", 2);

  // Move it into return position.
  const char* string = wrenGetSlotString(vm, 2);
  wrenSetSlotString(vm, 0, string);
}

static void otherModule(WrenVM* vm, void *userData)
{
  wrenGetVariable(vm, "./test/api/get_variable_module", "Variable", 0);
}

static void hasVariable(WrenVM* vm, void *userData)
{
  const char* module = wrenGetSlotString(vm, 1);
  const char* variable = wrenGetSlotString(vm, 2);

  bool result = wrenHasVariable(vm, module, variable);
  wrenEnsureSlots(vm, 1);
  wrenSetSlotBool(vm, 0, result);
}

static void hasModule(WrenVM* vm, void *userData)
{
  const char* module = wrenGetSlotString(vm, 1);

  bool result = wrenHasModule(vm, module);
  wrenEnsureSlots(vm, 1);
  wrenSetSlotBool(vm, 0, result);
}

WrenForeignMethodFn getVariableBindMethod(const char* signature)
{
  if (strcmp(signature, "static GetVariable.beforeDefined()") == 0) return beforeDefined;
  if (strcmp(signature, "static GetVariable.afterDefined()") == 0) return afterDefined;
  if (strcmp(signature, "static GetVariable.afterAssigned()") == 0) return afterAssigned;
  if (strcmp(signature, "static GetVariable.otherSlot()") == 0) return otherSlot;
  if (strcmp(signature, "static GetVariable.otherModule()") == 0) return otherModule;
  
  if (strcmp(signature, "static Has.variable(_,_)") == 0) return hasVariable;
  if (strcmp(signature, "static Has.module(_)") == 0) return hasModule;

  return NULL;
}
