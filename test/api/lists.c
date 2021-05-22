#include <string.h>

#include "lists.h"

static void newList(WrenVM* vm, void *userData)
{
  wrenSetSlotNewList(vm, 0);
}

// Helper function to store a double in a slot then insert it into the list at
// slot zero.
static void insertNumber(WrenVM* vm, int index, double value)
{
  wrenSetSlotDouble(vm, 1, value);
  wrenInsertInList(vm, 0, index, 1);
}

// Helper function to append a double in a slot then insert it into the list at
// slot zero.
static void appendNumber(WrenVM* vm, double value)
{
  wrenSetSlotDouble(vm, 1, value);
  wrenInsertInList(vm, 0, -1, 1);
}

static void insert(WrenVM* vm, void *userData)
{
  wrenSetSlotNewList(vm, 0);

  wrenEnsureSlots(vm, 2);

  // Appending.
  insertNumber(vm, 0, 1.0);
  insertNumber(vm, 1, 2.0);
  insertNumber(vm, 2, 3.0);

  // Inserting.
  insertNumber(vm, 0, 4.0);
  insertNumber(vm, 1, 5.0);
  insertNumber(vm, 2, 6.0);

  // Negative indexes.
  insertNumber(vm, -1, 7.0);
  insertNumber(vm, -2, 8.0);
  insertNumber(vm, -3, 9.0);
}

static void get(WrenVM* vm, void *userData)
{
  int listSlot = 1;
  int index = (int)wrenGetSlotDouble(vm, 2);

  wrenGetListElement(vm, listSlot, index, 0);
}

static void set(WrenVM* vm, void *userData)
{
  wrenSetSlotNewList(vm, 0);

  wrenEnsureSlots(vm, 2);

  appendNumber(vm, 1.0);
  appendNumber(vm, 2.0);
  appendNumber(vm, 3.0);
  appendNumber(vm, 4.0);
  
  //list[2] = 33
  wrenSetSlotDouble(vm, 1, 33);
  wrenSetListElement(vm, 0, 2, 1);

  //list[-1] = 44
  wrenSetSlotDouble(vm, 1, 44);
  wrenSetListElement(vm, 0, -1, 1);
}

WrenForeignMethodFn listsBindMethod(const char* signature)
{
  if (strcmp(signature, "static Lists.newList()") == 0) return newList;
  if (strcmp(signature, "static Lists.insert()") == 0) return insert;
  if (strcmp(signature, "static Lists.set()") == 0) return set;
  if (strcmp(signature, "static Lists.get(_,_)") == 0) return get;

  return NULL;
}
