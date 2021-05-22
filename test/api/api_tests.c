#include "./api_tests.h"

static const char* testName = NULL;

WrenBindForeignMethodResult APITest_bindForeignMethod(
    WrenVM* vm, const char* module, const char* className,
    bool isStatic, const char* signature)
{
#define RETURN_IF_NONNULL(m) \
  do { if (method != NULL) { \
    result.executeFn = method; \
    return result; \
  } } while(0)

  WrenBindForeignMethodResult result = {0};

  if (strncmp(module, "./test/", 7) != 0) return result;

  // For convenience, concatenate all of the method qualifiers into a single
  // signature string.
  char fullName[256];
  fullName[0] = '\0';
  if (isStatic) strcat(fullName, "static ");
  strcat(fullName, className);
  strcat(fullName, ".");
  strcat(fullName, signature);

  WrenForeignMethodFn method = NULL;

  method = benchmarkBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = callCallsForeignBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = errorBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = getVariableBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = foreignClassBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = handleBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = listsBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = mapsBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = newVMBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = resolutionBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = slotsBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  method = userDataBindMethod(fullName);
  RETURN_IF_NONNULL(method);

  WrenBindForeignMethodResult foreignMethodUserData =
      foreignMethodUserDataBindMethod(fullName);
  if(foreignMethodUserData.executeFn != NULL) {
    return foreignMethodUserData;
  }

  fprintf(stderr,
      "Unknown foreign method '%s' for test '%s'\n", fullName, testName);
  exit(1);
  return result;
#undef RETURN_IF_NONNULL
}

WrenForeignClassMethods APITest_bindForeignClass(
    WrenVM* vm, const char* module, const char* className)
{
  WrenForeignClassMethods methods = { NULL, NULL };
  if (strncmp(module, "./test/api", 7) != 0) return methods;

  foreignClassBindClass(className, &methods);
  if (methods.allocate != NULL) return methods;

  resetStackAfterForeignConstructBindClass(className, &methods);
  if (methods.allocate != NULL) return methods;

  slotsBindClass(className, &methods);
  if (methods.allocate != NULL) return methods;

  fprintf(stderr,
          "Unknown foreign class '%s' for test '%s'\n", className, testName);
  exit(1);
  return methods;
}

int APITest_Run(WrenVM* vm, const char* inTestName)
{
  testName = inTestName;
  if (strstr(inTestName, "/call.wren") != NULL)
  {
    return callRunTests(vm);
  }
  else if (strstr(inTestName, "/call_calls_foreign.wren") != NULL)
  {
    return callCallsForeignRunTests(vm);
  }
  else if (strstr(inTestName, "/call_wren_call_root.wren") != NULL)
  {
    return callWrenCallRootRunTests(vm);
  }
  else if (strstr(inTestName, "/reset_stack_after_call_abort.wren") != NULL)
  {
    return resetStackAfterCallAbortRunTests(vm);
  }
  else if (strstr(inTestName, "/reset_stack_after_foreign_construct.wren") != NULL)
  {
    return resetStackAfterForeignConstructRunTests(vm);
  }

  return 0;
}
