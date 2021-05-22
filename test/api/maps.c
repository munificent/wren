#include <string.h>

#include "maps.h"

static void newMap(WrenVM* vm, void *userData)
{
  wrenSetSlotNewMap(vm, 0);
}

static void invalidInsert(WrenVM* vm, void *userData)
{
  wrenSetSlotNewMap(vm, 0);
  
  wrenEnsureSlots(vm, 3);
  // Foreign Class is in slot 1
  wrenSetSlotString(vm, 2, "England");
  wrenSetMapValue(vm, 0, 1, 2); // expect this to cause errors
}

static void insert(WrenVM* vm, void *userData)
{
  wrenSetSlotNewMap(vm, 0);
  
  wrenEnsureSlots(vm, 3);

  // Insert String
  wrenSetSlotString(vm, 1, "England");
  wrenSetSlotString(vm, 2, "London");
  wrenSetMapValue(vm, 0, 1, 2);

  // Insert Double
  wrenSetSlotDouble(vm, 1, 1.0);
  wrenSetSlotDouble(vm, 2, 42.0);
  wrenSetMapValue(vm, 0, 1, 2);

  // Insert Boolean
  wrenSetSlotBool(vm, 1, false);
  wrenSetSlotBool(vm, 2, true);
  wrenSetMapValue(vm, 0, 1, 2);

  // Insert Null
  wrenSetSlotNull(vm, 1);
  wrenSetSlotNull(vm, 2);
  wrenSetMapValue(vm, 0, 1, 2);

  // Insert List
  wrenSetSlotString(vm, 1, "Empty");
  wrenSetSlotNewList(vm, 2);
  wrenSetMapValue(vm, 0, 1, 2);
}

static void removeKey(WrenVM* vm, void *userData)
{
  wrenEnsureSlots(vm, 3);

  wrenSetSlotString(vm, 2, "key");
  wrenRemoveMapValue(vm, 1, 2, 0);
}

static void countWren(WrenVM* vm, void *userData)
{
  int count = wrenGetMapCount(vm, 1);
  wrenSetSlotDouble(vm, 0, count);
}

static void countAPI(WrenVM* vm, void *userData)
{
  insert(vm, userData);
  int count = wrenGetMapCount(vm, 0);
  wrenSetSlotDouble(vm, 0, count);
}

static void containsWren(WrenVM* vm, void *userData)
{
  bool result = wrenGetMapContainsKey(vm, 1, 2);
  wrenSetSlotBool(vm, 0, result);
}


static void containsAPI(WrenVM* vm, void *userData)
{
  insert(vm, userData);
  
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 1, "England");

  bool result = wrenGetMapContainsKey(vm, 0, 1);
  wrenSetSlotBool(vm, 0, result);
}

static void containsAPIFalse(WrenVM* vm, void *userData)
{
  insert(vm, userData);

  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 1, "DefinitelyNotARealKey");

  bool result = wrenGetMapContainsKey(vm, 0, 1);
  wrenSetSlotBool(vm, 0, result);
}


WrenForeignMethodFn mapsBindMethod(const char* signature)
{
  if (strcmp(signature, "static Maps.newMap()") == 0) return newMap;
  if (strcmp(signature, "static Maps.insert()") == 0) return insert;
  if (strcmp(signature, "static Maps.remove(_)") == 0) return removeKey;
  if (strcmp(signature, "static Maps.count(_)") == 0) return countWren;
  if (strcmp(signature, "static Maps.count()") == 0) return countAPI;
  if (strcmp(signature, "static Maps.contains()") == 0) return containsAPI;
  if (strcmp(signature, "static Maps.containsFalse()") == 0) return containsAPIFalse;
  if (strcmp(signature, "static Maps.contains(_,_)") == 0) return containsWren;
  if (strcmp(signature, "static Maps.invalidInsert(_)") == 0) return invalidInsert;

  return NULL;
}

void foreignAllocate(WrenVM* vm, void *userData) {
  wrenSetSlotNewForeign(vm, 0, 0, 0);
}

void mapBindClass(
    const char* className, WrenForeignClassMethods* methods)
{
  if (strcmp(className, "ForeignClass") == 0)
  {
    methods->allocate = foreignAllocate;
    return;
  }
}
