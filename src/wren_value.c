#include <stdio.h>
#include <string.h>

#include "wren.h"
#include "wren_value.h"
#include "wren_vm.h"

#ifdef _MSC_VER
  #pragma warning( disable : 4996 )
#endif

// TODO: Tune these.
// The initial (and minimum) capacity of a non-empty list object.
#define LIST_MIN_CAPACITY (16)

// The rate at which a list's capacity grows when the size exceeds the current
// capacity. The new capacity will be determined by *multiplying* the old
// capacity by this. Growing geometrically is necessary to ensure that adding
// to a list has O(1) amortized complexity.
#define LIST_GROW_FACTOR (2)

DEFINE_BUFFER(Value, Value);
DEFINE_BUFFER(Method, Method);

#define ALLOCATE(vm, type) \
    ((type*)wrenReallocate(vm, NULL, 0, sizeof(type)))
#define ALLOCATE_FLEX(vm, type, extra) \
    ((type*)wrenReallocate(vm, NULL, 0, sizeof(type) + extra))
#define ALLOCATE_ARRAY(vm, type, count) \
    ((type*)wrenReallocate(vm, NULL, 0, sizeof(type) * count))

static void initObj(WrenVM* vm, Obj* obj, ObjType type, ObjClass* classObj)
{
  obj->type = type;
  obj->flags = 0;
  obj->classObj = classObj;
  obj->next = vm->first;
  vm->first = obj;
}

ObjClass* wrenNewSingleClass(WrenVM* vm, int numFields, ObjString* name)
{
  ObjClass* classObj = ALLOCATE(vm, ObjClass);
  initObj(vm, &classObj->obj, OBJ_CLASS, NULL);
  classObj->superclass = NULL;
  classObj->numFields = numFields;
  classObj->name = name;

  WREN_PIN(vm, classObj);
  wrenMethodBufferInit(vm, &classObj->methods);
  WREN_UNPIN(vm);

  return classObj;
}

void wrenBindSuperclass(WrenVM* vm, ObjClass* subclass, ObjClass* superclass)
{
  ASSERT(superclass != NULL, "Must have superclass.");

  subclass->superclass = superclass;

  // Include the superclass in the total number of fields.
  subclass->numFields += superclass->numFields;

  // Inherit methods from its superclass.
  for (int i = 0; i < superclass->methods.count; i++)
  {
    wrenBindMethod(vm, subclass, i, superclass->methods.data[i]);
  }
}

ObjClass* wrenNewClass(WrenVM* vm, ObjClass* superclass, int numFields,
                       ObjString* name)
{
  WREN_PIN(vm, name);

  // Create the metaclass.
  ObjString* metaclassName = wrenStringConcat(vm, name->value, " metaclass");
  WREN_PIN(vm, metaclassName);

  ObjClass* metaclass = wrenNewSingleClass(vm, 0, metaclassName);
  metaclass->obj.classObj = vm->classClass;

  WREN_UNPIN(vm);

  // Make sure the metaclass isn't collected when we allocate the class.
  WREN_PIN(vm, metaclass);

  // Metaclasses always inherit Class and do not parallel the non-metaclass
  // hierarchy.
  wrenBindSuperclass(vm, metaclass, vm->classClass);

  ObjClass* classObj = wrenNewSingleClass(vm, numFields, name);

  // Make sure the class isn't collected while the inherited methods are being
  // bound.
  WREN_PIN(vm, classObj);

  classObj->obj.classObj = metaclass;
  wrenBindSuperclass(vm, classObj, superclass);

  WREN_UNPIN(vm);
  WREN_UNPIN(vm);
  WREN_UNPIN(vm);

  return classObj;
}

void wrenBindMethod(WrenVM* vm, ObjClass* classObj, int symbol, Method method)
{
  // Make sure the buffer is big enough to reach the symbol's index.
  // TODO: Do a single grow instead of a loop.
  Method noMethod;
  noMethod.type = METHOD_NONE;
  while (symbol >= classObj->methods.count)
  {
    wrenMethodBufferWrite(vm, &classObj->methods, noMethod);
  }

  classObj->methods.data[symbol] = method;
}

ObjClosure* wrenNewClosure(WrenVM* vm, ObjFn* fn)
{
  ObjClosure* closure = ALLOCATE_FLEX(vm, ObjClosure,
                                      sizeof(Upvalue*) * fn->numUpvalues);
  initObj(vm, &closure->obj, OBJ_CLOSURE, vm->fnClass);

  closure->fn = fn;

  // Clear the upvalue array. We need to do this in case a GC is triggered
  // after the closure is created but before the upvalue array is populated.
  for (int i = 0; i < fn->numUpvalues; i++) closure->upvalues[i] = NULL;

  return closure;
}

ObjFiber* wrenNewFiber(WrenVM* vm, Obj* fn)
{
  ObjFiber* fiber = ALLOCATE(vm, ObjFiber);
  initObj(vm, &fiber->obj, OBJ_FIBER, vm->fiberClass);

  // Push the stack frame for the function.
  fiber->stackTop = fiber->stack;
  fiber->numFrames = 1;
  fiber->openUpvalues = NULL;
  fiber->caller = NULL;
  fiber->error = NULL;
  fiber->callerIsTrying = false;

  CallFrame* frame = &fiber->frames[0];
  frame->fn = fn;
  frame->stackStart = fiber->stack;
  if (fn->type == OBJ_FN)
  {
    frame->ip = ((ObjFn*)fn)->bytecode;
  }
  else
  {
    frame->ip = ((ObjClosure*)fn)->fn->bytecode;
  }

  return fiber;
}

ObjFn* wrenNewFunction(WrenVM* vm, Value* constants, int numConstants,
                       int numUpvalues, int numParams,
                       uint8_t* bytecode, int bytecodeLength,
                       ObjString* debugSourcePath,
                       const char* debugName, int debugNameLength,
                       int* sourceLines)
{
  // Allocate these before the function in case they trigger a GC which would
  // free the function.
  Value* copiedConstants = NULL;
  if (numConstants > 0)
  {
    copiedConstants = ALLOCATE_ARRAY(vm, Value, numConstants);
    for (int i = 0; i < numConstants; i++)
    {
      copiedConstants[i] = constants[i];
    }
  }

  FnDebug* debug = ALLOCATE(vm, FnDebug);

  debug->sourcePath = debugSourcePath;

  // Copy the function's name.
  debug->name = ALLOCATE_ARRAY(vm, char, debugNameLength + 1);
  strncpy(debug->name, debugName, debugNameLength);
  debug->name[debugNameLength] = '\0';

  debug->sourceLines = sourceLines;

  ObjFn* fn = ALLOCATE(vm, ObjFn);
  initObj(vm, &fn->obj, OBJ_FN, vm->fnClass);

  // TODO: Should eventually copy this instead of taking ownership. When the
  // compiler grows this, its capacity will often exceed the actual used size.
  // Copying to an exact-sized buffer will save a bit of memory. I tried doing
  // this, but it made the "for" benchmark ~15% slower for some unknown reason.
  fn->bytecode = bytecode;
  fn->constants = copiedConstants;
  fn->numUpvalues = numUpvalues;
  fn->numConstants = numConstants;
  fn->numParams = numParams;
  fn->bytecodeLength = bytecodeLength;
  fn->debug = debug;

  return fn;
}

Value wrenNewInstance(WrenVM* vm, ObjClass* classObj)
{
  ObjInstance* instance = ALLOCATE_FLEX(vm, ObjInstance,
                                        classObj->numFields * sizeof(Value));
  initObj(vm, &instance->obj, OBJ_INSTANCE, classObj);

  // Initialize fields to null.
  for (int i = 0; i < classObj->numFields; i++)
  {
    instance->fields[i] = NULL_VAL;
  }

  return OBJ_VAL(instance);
}

ObjList* wrenNewList(WrenVM* vm, int numElements)
{
  // Allocate this before the list object in case it triggers a GC which would
  // free the list.
  Value* elements = NULL;
  if (numElements > 0)
  {
    elements = ALLOCATE_ARRAY(vm, Value, numElements);
  }

  ObjList* list = ALLOCATE(vm, ObjList);
  initObj(vm, &list->obj, OBJ_LIST, vm->listClass);
  list->capacity = numElements;
  list->count = numElements;
  list->elements = elements;
  return list;
}

// Grows [list] if needed to ensure it can hold [count] elements.
static void ensureListCapacity(WrenVM* vm, ObjList* list, int count)
{
  if (list->capacity >= count) return;

  int capacity = list->capacity * LIST_GROW_FACTOR;
  if (capacity < LIST_MIN_CAPACITY) capacity = LIST_MIN_CAPACITY;

  list->capacity *= 2;
  list->elements = (Value*)wrenReallocate(vm, list->elements,
      list->capacity * sizeof(Value), capacity * sizeof(Value));
  // TODO: Handle allocation failure.
  list->capacity = capacity;
}

void wrenListAdd(WrenVM* vm, ObjList* list, Value value)
{
  if (IS_OBJ(value)) WREN_PIN(vm, AS_OBJ(value));

  ensureListCapacity(vm, list, list->count + 1);

  if (IS_OBJ(value)) WREN_UNPIN(vm);

  list->elements[list->count++] = value;
}

void wrenListInsert(WrenVM* vm, ObjList* list, Value value, int index)
{
  if (IS_OBJ(value)) WREN_PIN(vm, AS_OBJ(value));

  ensureListCapacity(vm, list, list->count + 1);

  if (IS_OBJ(value)) WREN_UNPIN(vm);

  // Shift items down.
  for (int i = list->count; i > index; i--)
  {
    list->elements[i] = list->elements[i - 1];
  }

  list->elements[index] = value;
  list->count++;
}

Value wrenListRemoveAt(WrenVM* vm, ObjList* list, int index)
{
  Value removed = list->elements[index];

  if (IS_OBJ(removed)) WREN_PIN(vm, AS_OBJ(removed));

  // Shift items up.
  for (int i = index; i < list->count - 1; i++)
  {
    list->elements[i] = list->elements[i + 1];
  }

  // If we have too much excess capacity, shrink it.
  if (list->capacity / LIST_GROW_FACTOR >= list->count)
  {
    list->elements = (Value*)wrenReallocate(vm, list->elements,
        sizeof(Value) * list->capacity,
        sizeof(Value) * (list->capacity / LIST_GROW_FACTOR));
    list->capacity /= LIST_GROW_FACTOR;
  }

  if (IS_OBJ(removed)) WREN_UNPIN(vm);

  list->count--;
  return removed;
}

Value wrenNewRange(WrenVM* vm, double from, double to, bool isInclusive)
{
  ObjRange* range = ALLOCATE(vm, ObjRange);
  initObj(vm, &range->obj, OBJ_RANGE, vm->rangeClass);
  range->from = from;
  range->to = to;
  range->isInclusive = isInclusive;

  return OBJ_VAL(range);
}

Value wrenNewString(WrenVM* vm, const char* text, size_t length)
{
  // Allow NULL if the string is empty since byte buffers don't allocate any
  // characters for a zero-length string.
  ASSERT(length == 0 || text != NULL, "Unexpected NULL string.");

  // TODO: Don't allocate a heap string at all for zero-length strings.
  ObjString* string = AS_STRING(wrenNewUninitializedString(vm, length));

  // Copy the string (if given one).
  if (length > 0) strncpy(string->value, text, length);

  string->value[length] = '\0';

  return OBJ_VAL(string);
}

Value wrenNewUninitializedString(WrenVM* vm, size_t length)
{
  ObjString* string = ALLOCATE_FLEX(vm, ObjString, length + 1);
  initObj(vm, &string->obj, OBJ_STRING, vm->stringClass);
  string->length = (int)length;

  return OBJ_VAL(string);
}

ObjString* wrenStringConcat(WrenVM* vm, const char* left, const char* right)
{
  size_t leftLength = strlen(left);
  size_t rightLength = strlen(right);

  Value value = wrenNewUninitializedString(vm, leftLength + rightLength);
  ObjString* string = AS_STRING(value);
  strcpy(string->value, left);
  strcpy(string->value + leftLength, right);
  string->value[leftLength + rightLength] = '\0';

  return string;
}

Upvalue* wrenNewUpvalue(WrenVM* vm, Value* value)
{
  Upvalue* upvalue = ALLOCATE(vm, Upvalue);

  // Upvalues are never used as first-class objects, so don't need a class.
  initObj(vm, &upvalue->obj, OBJ_UPVALUE, NULL);

  upvalue->value = value;
  upvalue->closed = NULL_VAL;
  upvalue->next = NULL;
  return upvalue;
}

// Sets the mark flag on [obj]. Returns true if it was already set so that we
// can avoid recursing into already-processed objects. That ensures we don't
// crash on an object cycle.
static bool setMarkedFlag(Obj* obj)
{
  if (obj->flags & FLAG_MARKED) return true;
  obj->flags |= FLAG_MARKED;
  return false;
}

static void markString(WrenVM* vm, ObjString* string)
{
  if (setMarkedFlag(&string->obj)) return;

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjString) + string->length + 1;
}

static void markClass(WrenVM* vm, ObjClass* classObj)
{
  if (setMarkedFlag(&classObj->obj)) return;

  // The metaclass.
  if (classObj->obj.classObj != NULL) markClass(vm, classObj->obj.classObj);

  // The superclass.
  if (classObj->superclass != NULL) markClass(vm, classObj->superclass);

  // Method function objects.
  for (int i = 0; i < classObj->methods.count; i++)
  {
    if (classObj->methods.data[i].type == METHOD_BLOCK)
    {
      wrenMarkObj(vm, classObj->methods.data[i].fn.obj);
    }
  }

  if (classObj->name != NULL) markString(vm, classObj->name);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjClass);
  vm->bytesAllocated += classObj->methods.capacity * sizeof(Method);
}

static void markFn(WrenVM* vm, ObjFn* fn)
{
  if (setMarkedFlag(&fn->obj)) return;

  // Mark the constants.
  for (int i = 0; i < fn->numConstants; i++)
  {
    wrenMarkValue(vm, fn->constants[i]);
  }

  wrenMarkObj(vm, (Obj*)fn->debug->sourcePath);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjFn);
  vm->bytesAllocated += sizeof(uint8_t) * fn->bytecodeLength;
  vm->bytesAllocated += sizeof(Value) * fn->numConstants;

  // The debug line number buffer.
  vm->bytesAllocated += sizeof(int) * fn->bytecodeLength;

  // TODO: What about the function name?
}

static void markInstance(WrenVM* vm, ObjInstance* instance)
{
  if (setMarkedFlag(&instance->obj)) return;

  markClass(vm, instance->obj.classObj);

  // Mark the fields.
  for (int i = 0; i < instance->obj.classObj->numFields; i++)
  {
    wrenMarkValue(vm, instance->fields[i]);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjInstance);
  vm->bytesAllocated += sizeof(Value) * instance->obj.classObj->numFields;
}

static void markList(WrenVM* vm, ObjList* list)
{
  if (setMarkedFlag(&list->obj)) return;

  // Mark the elements.
  Value* elements = list->elements;
  for (int i = 0; i < list->count; i++)
  {
    wrenMarkValue(vm, elements[i]);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjList);
  if (list->elements != NULL)
  {
    vm->bytesAllocated += sizeof(Value) * list->capacity;
  }
}

static void markUpvalue(WrenVM* vm, Upvalue* upvalue)
{
  // This can happen if a GC is triggered in the middle of initializing the
  // closure.
  if (upvalue == NULL) return;

  if (setMarkedFlag(&upvalue->obj)) return;

  // Mark the closed-over object (in case it is closed).
  wrenMarkValue(vm, upvalue->closed);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(Upvalue);
}

static void markFiber(WrenVM* vm, ObjFiber* fiber)
{
  if (setMarkedFlag(&fiber->obj)) return;

  // Stack functions.
  for (int i = 0; i < fiber->numFrames; i++)
  {
    wrenMarkObj(vm, fiber->frames[i].fn);
  }

  // Stack variables.
  for (Value* slot = fiber->stack; slot < fiber->stackTop; slot++)
  {
    wrenMarkValue(vm, *slot);
  }

  // Open upvalues.
  Upvalue* upvalue = fiber->openUpvalues;
  while (upvalue != NULL)
  {
    markUpvalue(vm, upvalue);
    upvalue = upvalue->next;
  }

  // The caller.
  if (fiber->caller != NULL) markFiber(vm, fiber->caller);

  if (fiber->error != NULL) markString(vm, fiber->error);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjFiber);
  // TODO: Count size of error message buffer.
}

static void markClosure(WrenVM* vm, ObjClosure* closure)
{
  if (setMarkedFlag(&closure->obj)) return;

  // Mark the function.
  markFn(vm, closure->fn);

  // Mark the upvalues.
  for (int i = 0; i < closure->fn->numUpvalues; i++)
  {
    Upvalue* upvalue = closure->upvalues[i];
    markUpvalue(vm, upvalue);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjClosure);
  vm->bytesAllocated += sizeof(Upvalue*) * closure->fn->numUpvalues;
}

void wrenMarkObj(WrenVM* vm, Obj* obj)
{
#if WREN_DEBUG_TRACE_MEMORY
  static int indent = 0;
  indent++;
  for (int i = 0; i < indent; i++) printf("  ");
  printf("mark ");
  wrenPrintValue(OBJ_VAL(obj));
  printf(" @ %p\n", obj);
#endif

  // Traverse the object's fields.
  switch (obj->type)
  {
    case OBJ_CLASS:    markClass(   vm, (ObjClass*)   obj); break;
    case OBJ_CLOSURE:  markClosure( vm, (ObjClosure*) obj); break;
    case OBJ_FIBER:    markFiber(   vm, (ObjFiber*)   obj); break;
    case OBJ_FN:       markFn(      vm, (ObjFn*)      obj); break;
    case OBJ_INSTANCE: markInstance(vm, (ObjInstance*)obj); break;
    case OBJ_LIST:     markList(    vm, (ObjList*)    obj); break;
    case OBJ_RANGE:    setMarkedFlag(obj); break;
    case OBJ_STRING:   markString(  vm, (ObjString*)  obj); break;
    case OBJ_UPVALUE:  markUpvalue( vm, (Upvalue*)    obj); break;
  }

#if WREN_DEBUG_TRACE_MEMORY
  indent--;
#endif
}

void wrenMarkValue(WrenVM* vm, Value value)
{
  if (!IS_OBJ(value)) return;
  wrenMarkObj(vm, AS_OBJ(value));
}

void wrenFreeObj(WrenVM* vm, Obj* obj)
{
#if WREN_DEBUG_TRACE_MEMORY
  printf("free ");
  wrenPrintValue(OBJ_VAL(obj));
  printf(" @ %p\n", obj);
#endif

  switch (obj->type)
  {
    case OBJ_CLASS:
      wrenMethodBufferClear(vm, &((ObjClass*)obj)->methods);
      break;

    case OBJ_FN:
    {
      ObjFn* fn = (ObjFn*)obj;
      wrenReallocate(vm, fn->constants, 0, 0);
      wrenReallocate(vm, fn->bytecode, 0, 0);
      wrenReallocate(vm, fn->debug->name, 0, 0);
      wrenReallocate(vm, fn->debug->sourceLines, 0, 0);
      wrenReallocate(vm, fn->debug, 0, 0);
      break;
    }

    case OBJ_LIST:
      wrenReallocate(vm, ((ObjList*)obj)->elements, 0, 0);
      break;

    case OBJ_STRING:
    case OBJ_CLOSURE:
    case OBJ_FIBER:
    case OBJ_INSTANCE:
    case OBJ_RANGE:
    case OBJ_UPVALUE:
      break;
  }

  wrenReallocate(vm, obj, 0, 0);
}

ObjClass* wrenGetClass(WrenVM* vm, Value value)
{
  return wrenGetClassInline(vm, value);
}

static void printList(ObjList* list)
{
  printf("[");
  for (int i = 0; i < list->count; i++)
  {
    if (i > 0) printf(", ");
    wrenPrintValue(list->elements[i]);
  }
  printf("]");
}

static void printObject(Obj* obj)
{
  switch (obj->type)
  {
    case OBJ_CLASS: printf("[class %p]", obj); break;
    case OBJ_CLOSURE: printf("[closure %p]", obj); break;
    case OBJ_FIBER: printf("[fiber %p]", obj); break;
    case OBJ_FN: printf("[fn %p]", obj); break;
    case OBJ_INSTANCE: printf("[instance %p]", obj); break;
    case OBJ_LIST: printList((ObjList*)obj); break;
    case OBJ_RANGE: printf("[fn %p]", obj); break;
    case OBJ_STRING: printf("%s", ((ObjString*)obj)->value); break;
    case OBJ_UPVALUE: printf("[upvalue %p]", obj); break;
    default: printf("[unknown object]"); break;
  }
}

void wrenPrintValue(Value value)
{
  #if WREN_NAN_TAGGING
  if (IS_NUM(value))
  {
    printf("%.14g", AS_NUM(value));
  }
  else if (IS_OBJ(value))
  {
    printObject(AS_OBJ(value));
  }
  else
  {
    switch (GET_TAG(value))
    {
      case TAG_FALSE: printf("false"); break;
      case TAG_NAN: printf("NaN"); break;
      case TAG_NULL: printf("null"); break;
      case TAG_TRUE: printf("true"); break;
      case TAG_UNDEFINED: UNREACHABLE();
    }
  }
  #else
  switch (value.type)
  {
    case VAL_FALSE: printf("false"); break;
    case VAL_NULL: printf("null"); break;
    case VAL_NUM: printf("%.14g", AS_NUM(value)); break;
    case VAL_TRUE: printf("true"); break;
    case VAL_OBJ: printObject(AS_OBJ(value)); break;
    case VAL_UNDEFINED: UNREACHABLE();
  }
  #endif
}
