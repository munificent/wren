#include "wren_opt_mirror.h"

#if WREN_OPT_MIRROR

#include <string.h>

#include "wren_vm.h"
#include "wren_opt_mirror.wren.inc"

static ObjClass* mirrorGetSlotClass(WrenVM* vm, int slot)
{
  Value classVal = *wrenSlotAtUnsafe(vm, slot);
  if (!IS_CLASS(classVal)) return NULL;

  return AS_CLASS(classVal);
}

static ObjClosure* mirrorGetSlotClosure(WrenVM* vm, int slot)
{
  Value closureVal = *wrenSlotAtUnsafe(vm, slot);
  if (!IS_CLOSURE(closureVal)) return NULL;

  return AS_CLOSURE(closureVal);
}

static ObjModule* mirrorGetSlotModule(WrenVM* vm, int slot)
{
  Value moduleVal = *wrenSlotAtUnsafe(vm, slot);
  if (!IS_MODULE(moduleVal)) return NULL;

  return AS_MODULE(moduleVal);
}

static void mirrorClassMirrorHasMethod(WrenVM* vm)
{
  ObjClass* classObj = mirrorGetSlotClass(vm, 1);
  const char* signature = wrenGetSlotString(vm, 2);

  bool hasMethod = false;
  if (classObj != NULL &&
      signature != NULL)
  {
    int symbol = wrenSymbolTableFind(&vm->methodNames,
                                     signature, strlen(signature));
    hasMethod = wrenClassGetMethod(vm, classObj, symbol) != NULL;
  }
  wrenSetSlotBool(vm, 0, hasMethod);
}

static void mirrorClassMirrorMethodNames(WrenVM* vm)
{
  ObjClass* classObj = mirrorGetSlotClass(vm, 1);

  if (!classObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  wrenSetSlotNewList(vm, 0);
  for (size_t symbol = 0; symbol < classObj->methods.count; symbol++)
  {
    Method* method = wrenClassGetMethod(vm, classObj, symbol);
    if (method == NULL) continue;

    *wrenSlotAtUnsafe(vm, 1) = OBJ_VAL(vm->methodNames.data[symbol]);
    wrenInsertInList(vm, 0, -1, 1);
  }
}

static void mirrorMethodMirrorBoundToClass_(WrenVM* vm)
{
  ObjClosure* closureObj = mirrorGetSlotClosure(vm, 1);

  if (!closureObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(closureObj->fn->boundToClass);
}

static void mirrorMethodMirrorModule_(WrenVM* vm)
{
  ObjClosure* closureObj = mirrorGetSlotClosure(vm, 1);

  if (!closureObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(closureObj->fn->module);
}

static void mirrorMethodMirrorSignature_(WrenVM* vm)
{
  ObjClosure* closureObj = mirrorGetSlotClosure(vm, 1);

  if (!closureObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  wrenSetSlotString(vm, 0, closureObj->fn->debug->name);
}

static void mirrorModuleMirrorFromName_(WrenVM* vm)
{
  const char* moduleName = wrenGetSlotString(vm, 1);

  if (!moduleName)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  // Special case for "core"
  if (strcmp(moduleName, "core") == 0)
  {
    wrenSetSlotNull(vm, 1);
  }

  ObjModule* module = wrenGetModule(vm, *wrenSlotAtUnsafe(vm, 1));
  if (module != NULL)
  {
    *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(module);
  }
  else
  {
    wrenSetSlotNull(vm, 0);
  }
}

static void mirrorModuleMirrorName_(WrenVM* vm)
{
  ObjModule* moduleObj = mirrorGetSlotModule(vm, 1);
  if (!moduleObj)
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  if (moduleObj != NULL)
  {
    if (moduleObj->name)
    {
      *wrenSlotAtUnsafe(vm, 0) = OBJ_VAL(moduleObj->name);
    }
    else
    {
      // Special case for "core"
      wrenSetSlotString(vm, 0, "core");
    }
  }
  else
  {
    wrenSetSlotNull(vm, 0);
  }
}

static void mirrorObjectMirrorCanInvoke(WrenVM* vm)
{
  ObjClass* classObj = wrenGetClassInline(vm, vm->apiStack[1]);
  vm->apiStack[1] = OBJ_VAL(classObj);

  mirrorClassMirrorHasMethod(vm);
}

const char* wrenMirrorSource()
{
  return mirrorModuleSource;
}

WrenForeignMethodFn wrenMirrorBindForeignMethod(WrenVM* vm,
                                                const char* className,
                                                bool isStatic,
                                                const char* signature)
{
  if (strcmp(className, "ClassMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "hasMethod(_,_)") == 0)
    {
      return mirrorClassMirrorHasMethod;
    }
    if (isStatic &&
        strcmp(signature, "methodNames(_)") == 0)
    {
      return mirrorClassMirrorMethodNames;
    }
  }

  if (strcmp(className, "MethodMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "boundToClass_(_)") == 0)
    {
      return mirrorMethodMirrorBoundToClass_;
    }
    if (isStatic &&
        strcmp(signature, "module_(_)") == 0)
    {
      return mirrorMethodMirrorModule_;
    }
    if (isStatic &&
        strcmp(signature, "signature_(_)") == 0)
    {
      return mirrorMethodMirrorSignature_;
    }
  }

  if (strcmp(className, "ModuleMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "fromName_(_)") == 0)
    {
      return mirrorModuleMirrorFromName_;
    }
    if (isStatic &&
        strcmp(signature, "name_(_)") == 0)
    {
      return mirrorModuleMirrorName_;
    }
  }

  if (strcmp(className, "ObjectMirror") == 0)
  {
    if (isStatic &&
        strcmp(signature, "canInvoke(_,_)") == 0)
    {
      return mirrorObjectMirrorCanInvoke;
    }
  }

  ASSERT(false, "Unknown method.");
  return NULL;
}

#endif
