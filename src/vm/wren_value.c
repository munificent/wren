#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "wren.h"
#include "wren_value.h"
#include "wren_vm.h"

#if WREN_DEBUG_TRACE_MEMORY
  #include "wren_debug.h"
#endif

// TODO: Tune these.
// The initial (and minimum) capacity of a non-empty list or map object.
#define MIN_CAPACITY 16

// The rate at which a collection's capacity grows when the size exceeds the
// current capacity. The new capacity will be determined by *multiplying* the
// old capacity by this. Growing geometrically is necessary to ensure that
// adding to a collection has O(1) amortized complexity.
#define GROW_FACTOR 2

// The maximum percentage of map entries that can be filled before the map is
// grown. A lower load takes more memory but reduces collisions which makes
// lookup faster.
#define MAP_LOAD_PERCENT 75

// The number of call frames initially allocated when a fiber is created. Making
// this smaller makes fibers use less memory (at first) but spends more time
// reallocating when the call stack grows.
#define INITIAL_CALL_FRAMES 4

DEFINE_BUFFER(Value, Value);
DEFINE_BUFFER(Method, Method);
DEFINE_BUFFER(ForeignMethodUserData, ForeignMethodUserData);

static void initObj(WrenVM* vm, Obj* obj, ObjType type, ObjClass* classObj)
{
  obj->type = type;
  obj->isDark = false;
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
  classObj->attributes = NULL_VAL;

  wrenPushRoot(vm, (Obj*)classObj);
  wrenMethodBufferInit(&classObj->methods);
  wrenForeignMethodUserDataBufferInit(&classObj->foreignMethodUserDatas);
  wrenPopRoot(vm);

  return classObj;
}

void wrenBindSuperclass(WrenVM* vm, ObjClass* subclass, ObjClass* superclass)
{
  ASSERT(superclass != NULL, "Must have superclass.");

  subclass->superclass = superclass;

  // Include the superclass in the total number of fields.
  if (subclass->numFields != -1)
  {
    subclass->numFields += superclass->numFields;
  }
  else
  {
    ASSERT(superclass->numFields == 0,
           "A foreign class cannot inherit from a class with fields.");
  }

  // Inherit methods from its superclass.
  for (int i = 0; i < superclass->methods.count; i++)
  {
    // Grab foreign-method userdata from the superclass, if any
    WrenUserData userData = WREN_USER_DATA_NONE;
    if(superclass->methods.data[i].type == METHOD_FOREIGN) {
      userData = superclass->foreignMethodUserDatas.data[i].userData;
    }

    wrenBindMethod(vm, subclass, i, superclass->methods.data[i], userData);
  }
}

ObjClass* wrenNewClass(WrenVM* vm, ObjClass* superclass, int numFields,
                       ObjString* name)
{
  // Create the metaclass.
  Value metaclassName = wrenStringFormat(vm, "@ metaclass", OBJ_VAL(name));
  wrenPushRoot(vm, AS_OBJ(metaclassName));

  ObjClass* metaclass = wrenNewSingleClass(vm, 0, AS_STRING(metaclassName));
  metaclass->obj.classObj = vm->classClass;

  wrenPopRoot(vm);

  // Make sure the metaclass isn't collected when we allocate the class.
  wrenPushRoot(vm, (Obj*)metaclass);

  // Metaclasses always inherit Class and do not parallel the non-metaclass
  // hierarchy.
  wrenBindSuperclass(vm, metaclass, vm->classClass);

  ObjClass* classObj = wrenNewSingleClass(vm, numFields, name);

  // Make sure the class isn't collected while the inherited methods are being
  // bound.
  wrenPushRoot(vm, (Obj*)classObj);

  classObj->obj.classObj = metaclass;
  wrenBindSuperclass(vm, classObj, superclass);

  wrenPopRoot(vm);
  wrenPopRoot(vm);

  return classObj;
}

void wrenBindMethod(WrenVM* vm, ObjClass* classObj, int symbol, Method method,
                    WrenUserData userData)
{
  // Make sure the buffer is big enough to contain the symbol's index.
  if (symbol >= classObj->methods.count)
  {
    Method noMethod;
    noMethod.type = METHOD_NONE;
    wrenMethodBufferFill(vm, &classObj->methods, noMethod,
                         symbol - classObj->methods.count + 1);
  }

  classObj->methods.data[symbol] = method;

  if(method.type == METHOD_FOREIGN) {
    // Make sure the foreign userdata buffer is big enough.
    ForeignMethodUserData noUserData;
    noUserData.userData = WREN_USER_DATA_NONE;
    wrenForeignMethodUserDataBufferFill(vm, &classObj->foreignMethodUserDatas,
        noUserData, symbol - classObj->foreignMethodUserDatas.count + 1);

    classObj->foreignMethodUserDatas.data[symbol].userData = userData;
  }
}

ObjClosure* wrenNewClosure(WrenVM* vm, ObjFn* fn)
{
  ObjClosure* closure = ALLOCATE_FLEX(vm, ObjClosure,
                                      ObjUpvalue*, fn->numUpvalues);
  initObj(vm, &closure->obj, OBJ_CLOSURE, vm->fnClass);

  closure->fn = fn;

  // Clear the upvalue array. We need to do this in case a GC is triggered
  // after the closure is created but before the upvalue array is populated.
  for (int i = 0; i < fn->numUpvalues; i++) closure->upvalues[i] = NULL;

  return closure;
}

ObjFiber* wrenNewFiber(WrenVM* vm, ObjClosure* closure)
{
  // Allocate the arrays before the fiber in case it triggers a GC.
  CallFrame* frames = ALLOCATE_ARRAY(vm, CallFrame, INITIAL_CALL_FRAMES);
  
  // Add one slot for the unused implicit receiver slot that the compiler
  // assumes all functions have.
  int stackCapacity = closure == NULL
      ? 1
      : wrenPowerOf2Ceil(closure->fn->maxSlots + 1);
  Value* stack = ALLOCATE_ARRAY(vm, Value, stackCapacity);
  
  ObjFiber* fiber = ALLOCATE(vm, ObjFiber);
  initObj(vm, &fiber->obj, OBJ_FIBER, vm->fiberClass);

  fiber->stack = stack;
  fiber->stackTop = fiber->stack;
  fiber->stackCapacity = stackCapacity;

  fiber->frames = frames;
  fiber->frameCapacity = INITIAL_CALL_FRAMES;
  fiber->numFrames = 0;

  fiber->openUpvalues = NULL;
  fiber->caller = NULL;
  fiber->error = NULL_VAL;
  fiber->state = FIBER_OTHER;
  
  if (closure != NULL)
  {
    // Initialize the first call frame.
    wrenAppendCallFrame(vm, fiber, closure, fiber->stack);

    // The first slot always holds the closure.
    fiber->stackTop[0] = OBJ_VAL(closure);
    fiber->stackTop++;
  }
  
  return fiber;
}

void wrenEnsureStack(WrenVM* vm, ObjFiber* fiber, int needed)
{
  if (fiber->stackCapacity >= needed) return;
  
  int capacity = wrenPowerOf2Ceil(needed);
  
  Value* oldStack = fiber->stack;
  fiber->stack = (Value*)wrenReallocate(vm, fiber->stack,
                                        sizeof(Value) * fiber->stackCapacity,
                                        sizeof(Value) * capacity);
  fiber->stackCapacity = capacity;
  
  // If the reallocation moves the stack, then we need to recalculate every
  // pointer that points into the old stack to into the same relative distance
  // in the new stack. We have to be a little careful about how these are
  // calculated because pointer subtraction is only well-defined within a
  // single array, hence the slightly redundant-looking arithmetic below.
  if (fiber->stack != oldStack)
  {
    // Top of the stack.
    if (vm->apiStack >= oldStack && vm->apiStack <= fiber->stackTop)
    {
      vm->apiStack = fiber->stack + (vm->apiStack - oldStack);
    }
    
    // Stack pointer for each call frame.
    for (int i = 0; i < fiber->numFrames; i++)
    {
      CallFrame* frame = &fiber->frames[i];
      frame->stackStart = fiber->stack + (frame->stackStart - oldStack);
    }
    
    // Open upvalues.
    for (ObjUpvalue* upvalue = fiber->openUpvalues;
         upvalue != NULL;
         upvalue = upvalue->next)
    {
      upvalue->value = fiber->stack + (upvalue->value - oldStack);
    }
    
    fiber->stackTop = fiber->stack + (fiber->stackTop - oldStack);
  }
}

ObjForeign* wrenNewForeign(WrenVM* vm, ObjClass* classObj, size_t size)
{
  ObjForeign* object = ALLOCATE_FLEX(vm, ObjForeign, uint8_t, size);
  initObj(vm, &object->obj, OBJ_FOREIGN, classObj);

  // Zero out the bytes.
  memset(object->data, 0, size);
  return object;
}

ObjFn* wrenNewFunction(WrenVM* vm, ObjModule* module, int maxSlots)
{
  FnDebug* debug = ALLOCATE(vm, FnDebug);
  debug->name = NULL;
  wrenIntBufferInit(&debug->sourceLines);

  ObjFn* fn = ALLOCATE(vm, ObjFn);
  initObj(vm, &fn->obj, OBJ_FN, vm->fnClass);
  
  wrenValueBufferInit(&fn->constants);
  wrenByteBufferInit(&fn->code);
  fn->module = module;
  fn->maxSlots = maxSlots;
  fn->numUpvalues = 0;
  fn->arity = 0;
  fn->debug = debug;
  
  return fn;
}

void wrenFunctionBindName(WrenVM* vm, ObjFn* fn, const char* name, int length)
{
  fn->debug->name = ALLOCATE_ARRAY(vm, char, length + 1);
  memcpy(fn->debug->name, name, length);
  fn->debug->name[length] = '\0';
}

Value wrenNewInstance(WrenVM* vm, ObjClass* classObj)
{
  ObjInstance* instance = ALLOCATE_FLEX(vm, ObjInstance,
                                        Value, classObj->numFields);
  initObj(vm, &instance->obj, OBJ_INSTANCE, classObj);

  // Initialize fields to null.
  for (int i = 0; i < classObj->numFields; i++)
  {
    instance->fields[i] = NULL_VAL;
  }

  return OBJ_VAL(instance);
}

ObjList* wrenNewList(WrenVM* vm, uint32_t numElements)
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
  list->elements.capacity = numElements;
  list->elements.count = numElements;
  list->elements.data = elements;
  return list;
}

void wrenListInsert(WrenVM* vm, ObjList* list, Value value, uint32_t index)
{
  if (IS_OBJ(value)) wrenPushRoot(vm, AS_OBJ(value));

  // Add a slot at the end of the list.
  wrenValueBufferWrite(vm, &list->elements, NULL_VAL);

  if (IS_OBJ(value)) wrenPopRoot(vm);

  // Shift the existing elements down.
  for (uint32_t i = list->elements.count - 1; i > index; i--)
  {
    list->elements.data[i] = list->elements.data[i - 1];
  }

  // Store the new element.
  list->elements.data[index] = value;
}

int wrenListIndexOf(WrenVM* vm, ObjList* list, Value value)
{
  int count = list->elements.count;
  for (int i = 0; i < count; i++)
  {
    Value item = list->elements.data[i];
    if(wrenValuesEqual(item, value)) {
      return i;
    }
  }
  return -1;
}

Value wrenListRemoveAt(WrenVM* vm, ObjList* list, uint32_t index)
{
  Value removed = list->elements.data[index];

  if (IS_OBJ(removed)) wrenPushRoot(vm, AS_OBJ(removed));

  // Shift items up.
  for (int i = index; i < list->elements.count - 1; i++)
  {
    list->elements.data[i] = list->elements.data[i + 1];
  }

  // If we have too much excess capacity, shrink it.
  if (list->elements.capacity / GROW_FACTOR >= list->elements.count)
  {
    list->elements.data = (Value*)wrenReallocate(vm, list->elements.data,
        sizeof(Value) * list->elements.capacity,
        sizeof(Value) * (list->elements.capacity / GROW_FACTOR));
    list->elements.capacity /= GROW_FACTOR;
  }

  if (IS_OBJ(removed)) wrenPopRoot(vm);

  list->elements.count--;
  return removed;
}

ObjMap* wrenNewMap(WrenVM* vm)
{
  ObjMap* map = ALLOCATE(vm, ObjMap);
  initObj(vm, &map->obj, OBJ_MAP, vm->mapClass);
  map->capacity = 0;
  map->count = 0;
  map->entries = NULL;
  return map;
}

static inline uint32_t hashBits(uint64_t hash)
{
  // From v8's ComputeLongHash() which in turn cites:
  // Thomas Wang, Integer Hash Functions.
  // http://www.concentric.net/~Ttwang/tech/inthash.htm
  hash = ~hash + (hash << 18);  // hash = (hash << 18) - hash - 1;
  hash = hash ^ (hash >> 31);
  hash = hash * 21;  // hash = (hash + (hash << 2)) + (hash << 4);
  hash = hash ^ (hash >> 11);
  hash = hash + (hash << 6);
  hash = hash ^ (hash >> 22);
  return (uint32_t)(hash & 0x3fffffff);
}

// Generates a hash code for [num].
static inline uint32_t hashNumber(double num)
{
  // Hash the raw bits of the value.
  return hashBits(wrenDoubleToBits(num));
}

// Generates a hash code for [object].
static uint32_t hashObject(Obj* object)
{
  switch (object->type)
  {
    case OBJ_CLASS:
      // Classes just use their name.
      return hashObject((Obj*)((ObjClass*)object)->name);
      
      // Allow bare (non-closure) functions so that we can use a map to find
      // existing constants in a function's constant table. This is only used
      // internally. Since user code never sees a non-closure function, they
      // cannot use them as map keys.
    case OBJ_FN:
    {
      ObjFn* fn = (ObjFn*)object;
      return hashNumber(fn->arity) ^ hashNumber(fn->code.count);
    }

    case OBJ_RANGE:
    {
      ObjRange* range = (ObjRange*)object;
      return hashNumber(range->from) ^ hashNumber(range->to);
    }

    case OBJ_STRING:
      return ((ObjString*)object)->hash;

    default:
      ASSERT(false, "Only immutable objects can be hashed.");
      return 0;
  }
}

// Generates a hash code for [value], which must be one of the built-in
// immutable types: null, bool, class, num, range, or string.
static uint32_t hashValue(Value value)
{
  // TODO: We'll probably want to randomize this at some point.

#if WREN_NAN_TAGGING
  if (IS_OBJ(value)) return hashObject(AS_OBJ(value));

  // Hash the raw bits of the unboxed value.
  return hashBits(value);
#else
  switch (value.type)
  {
    case VAL_FALSE: return 0;
    case VAL_NULL:  return 1;
    case VAL_NUM:   return hashNumber(AS_NUM(value));
    case VAL_TRUE:  return 2;
    case VAL_OBJ:   return hashObject(AS_OBJ(value));
    default:        UNREACHABLE();
  }
  
  return 0;
#endif
}

// Looks for an entry with [key] in an array of [capacity] [entries].
//
// If found, sets [result] to point to it and returns `true`. Otherwise,
// returns `false` and points [result] to the entry where the key/value pair
// should be inserted.
static bool findEntry(MapEntry* entries, uint32_t capacity, Value key,
                      MapEntry** result)
{
  // If there is no entry array (an empty map), we definitely won't find it.
  if (capacity == 0) return false;
  
  // Figure out where to insert it in the table. Use open addressing and
  // basic linear probing.
  uint32_t startIndex = hashValue(key) % capacity;
  uint32_t index = startIndex;
  
  // If we pass a tombstone and don't end up finding the key, its entry will
  // be re-used for the insert.
  MapEntry* tombstone = NULL;
  
  // Walk the probe sequence until we've tried every slot.
  do
  {
    MapEntry* entry = &entries[index];
    
    if (IS_UNDEFINED(entry->key))
    {
      // If we found an empty slot, the key is not in the table. If we found a
      // slot that contains a deleted key, we have to keep looking.
      if (IS_FALSE(entry->value))
      {
        // We found an empty slot, so we've reached the end of the probe
        // sequence without finding the key. If we passed a tombstone, then
        // that's where we should insert the item, otherwise, put it here at
        // the end of the sequence.
        *result = tombstone != NULL ? tombstone : entry;
        return false;
      }
      else
      {
        // We found a tombstone. We need to keep looking in case the key is
        // after it, but we'll use this entry as the insertion point if the
        // key ends up not being found.
        if (tombstone == NULL) tombstone = entry;
      }
    }
    else if (wrenValuesEqual(entry->key, key))
    {
      // We found the key.
      *result = entry;
      return true;
    }
    
    // Try the next slot.
    index = (index + 1) % capacity;
  }
  while (index != startIndex);
  
  // If we get here, the table is full of tombstones. Return the first one we
  // found.
  ASSERT(tombstone != NULL, "Map should have tombstones or empty entries.");
  *result = tombstone;
  return false;
}

// Inserts [key] and [value] in the array of [entries] with the given
// [capacity].
//
// Returns `true` if this is the first time [key] was added to the map.
static bool insertEntry(MapEntry* entries, uint32_t capacity,
                        Value key, Value value)
{
  ASSERT(entries != NULL, "Should ensure capacity before inserting.");
  
  MapEntry* entry;
  if (findEntry(entries, capacity, key, &entry))
  {
    // Already present, so just replace the value.
    entry->value = value;
    return false;
  }
  else
  {
    entry->key = key;
    entry->value = value;
    return true;
  }
}

// Updates [map]'s entry array to [capacity].
static void resizeMap(WrenVM* vm, ObjMap* map, uint32_t capacity)
{
  // Create the new empty hash table.
  MapEntry* entries = ALLOCATE_ARRAY(vm, MapEntry, capacity);
  for (uint32_t i = 0; i < capacity; i++)
  {
    entries[i].key = UNDEFINED_VAL;
    entries[i].value = FALSE_VAL;
  }

  // Re-add the existing entries.
  if (map->capacity > 0)
  {
    for (uint32_t i = 0; i < map->capacity; i++)
    {
      MapEntry* entry = &map->entries[i];
      
      // Don't copy empty entries or tombstones.
      if (IS_UNDEFINED(entry->key)) continue;

      insertEntry(entries, capacity, entry->key, entry->value);
    }
  }

  // Replace the array.
  DEALLOCATE(vm, map->entries);
  map->entries = entries;
  map->capacity = capacity;
}

Value wrenMapGet(ObjMap* map, Value key)
{
  MapEntry* entry;
  if (findEntry(map->entries, map->capacity, key, &entry)) return entry->value;

  return UNDEFINED_VAL;
}

void wrenMapSet(WrenVM* vm, ObjMap* map, Value key, Value value)
{
  // If the map is getting too full, make room first.
  if (map->count + 1 > map->capacity * MAP_LOAD_PERCENT / 100)
  {
    // Figure out the new hash table size.
    uint32_t capacity = map->capacity * GROW_FACTOR;
    if (capacity < MIN_CAPACITY) capacity = MIN_CAPACITY;

    resizeMap(vm, map, capacity);
  }

  if (insertEntry(map->entries, map->capacity, key, value))
  {
    // A new key was added.
    map->count++;
  }
}

void wrenMapClear(WrenVM* vm, ObjMap* map)
{
  DEALLOCATE(vm, map->entries);
  map->entries = NULL;
  map->capacity = 0;
  map->count = 0;
}

Value wrenMapRemoveKey(WrenVM* vm, ObjMap* map, Value key)
{
  MapEntry* entry;
  if (!findEntry(map->entries, map->capacity, key, &entry)) return NULL_VAL;

  // Remove the entry from the map. Set this value to true, which marks it as a
  // deleted slot. When searching for a key, we will stop on empty slots, but
  // continue past deleted slots.
  Value value = entry->value;
  entry->key = UNDEFINED_VAL;
  entry->value = TRUE_VAL;

  if (IS_OBJ(value)) wrenPushRoot(vm, AS_OBJ(value));

  map->count--;

  if (map->count == 0)
  {
    // Removed the last item, so free the array.
    wrenMapClear(vm, map);
  }
  else if (map->capacity > MIN_CAPACITY &&
           map->count < map->capacity / GROW_FACTOR * MAP_LOAD_PERCENT / 100)
  {
    uint32_t capacity = map->capacity / GROW_FACTOR;
    if (capacity < MIN_CAPACITY) capacity = MIN_CAPACITY;

    // The map is getting empty, so shrink the entry array back down.
    // TODO: Should we do this less aggressively than we grow?
    resizeMap(vm, map, capacity);
  }

  if (IS_OBJ(value)) wrenPopRoot(vm);
  return value;
}

ObjModule* wrenNewModule(WrenVM* vm, ObjString* name)
{
  ObjModule* module = ALLOCATE(vm, ObjModule);

  // Modules are never used as first-class objects, so don't need a class.
  initObj(vm, (Obj*)module, OBJ_MODULE, NULL);

  wrenPushRoot(vm, (Obj*)module);

  wrenSymbolTableInit(&module->variableNames);
  wrenValueBufferInit(&module->variables);

  module->name = name;

  wrenPopRoot(vm);
  return module;
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

// Creates a new string object with a null-terminated buffer large enough to
// hold a string of [length] but does not fill in the bytes.
//
// The caller is expected to fill in the buffer and then calculate the string's
// hash.
static ObjString* allocateString(WrenVM* vm, size_t length)
{
  ObjString* string = ALLOCATE_FLEX(vm, ObjString, char, length + 1);
  initObj(vm, &string->obj, OBJ_STRING, vm->stringClass);
  string->length = (int)length;
  string->value[length] = '\0';

  return string;
}

// Calculates and stores the hash code for [string].
static void hashString(ObjString* string)
{
  // FNV-1a hash. See: http://www.isthe.com/chongo/tech/comp/fnv/
  uint32_t hash = 2166136261u;

  // This is O(n) on the length of the string, but we only call this when a new
  // string is created. Since the creation is also O(n) (to copy/initialize all
  // the bytes), we allow this here.
  for (uint32_t i = 0; i < string->length; i++)
  {
    hash ^= string->value[i];
    hash *= 16777619;
  }

  string->hash = hash;
}

Value wrenNewString(WrenVM* vm, const char* text)
{
  return wrenNewStringLength(vm, text, strlen(text));
}

Value wrenNewStringLength(WrenVM* vm, const char* text, size_t length)
{
  // Allow NULL if the string is empty since byte buffers don't allocate any
  // characters for a zero-length string.
  ASSERT(length == 0 || text != NULL, "Unexpected NULL string.");
  
  ObjString* string = allocateString(vm, length);
  
  // Copy the string (if given one).
  if (length > 0 && text != NULL) memcpy(string->value, text, length);
  
  hashString(string);
  return OBJ_VAL(string);
}


Value wrenNewStringFromRange(WrenVM* vm, ObjString* source, int start,
                             uint32_t count, int step)
{
  uint8_t* from = (uint8_t*)source->value;
  int length = 0;
  for (uint32_t i = 0; i < count; i++)
  {
    length += wrenUtf8DecodeNumBytes(from[start + i * step]);
  }

  ObjString* result = allocateString(vm, length);
  result->value[length] = '\0';

  uint8_t* to = (uint8_t*)result->value;
  for (uint32_t i = 0; i < count; i++)
  {
    int index = start + i * step;
    int codePoint = wrenUtf8Decode(from + index, source->length - index);

    if (codePoint != -1)
    {
      to += wrenUtf8Encode(codePoint, to);
    }
  }

  hashString(result);
  return OBJ_VAL(result);
}

Value wrenNumToString(WrenVM* vm, double value)
{
  // Edge case: If the value is NaN or infinity, different versions of libc
  // produce different outputs (some will format it signed and some won't). To
  // get reliable output, handle it ourselves.
  if (isnan(value)) return CONST_STRING(vm, "nan");
  if (isinf(value))
  {
    if (value > 0.0)
    {
      return CONST_STRING(vm, "infinity");
    }
    else
    {
      return CONST_STRING(vm, "-infinity");
    }
  }

  // This is large enough to hold any double converted to a string using
  // "%.14g". Example:
  //
  //     -1.12345678901234e-1022
  //
  // So we have:
  //
  // + 1 char for sign
  // + 1 char for digit
  // + 1 char for "."
  // + 14 chars for decimal digits
  // + 1 char for "e"
  // + 1 char for "-" or "+"
  // + 4 chars for exponent
  // + 1 char for "\0"
  // = 24
  char buffer[24];
  int length = sprintf(buffer, "%.14g", value);
  return wrenNewStringLength(vm, buffer, length);
}

Value wrenStringFromCodePoint(WrenVM* vm, int value)
{
  int length = wrenUtf8EncodeNumBytes(value);
  ASSERT(length != 0, "Value out of range.");

  ObjString* string = allocateString(vm, length);

  wrenUtf8Encode(value, (uint8_t*)string->value);
  hashString(string);

  return OBJ_VAL(string);
}

Value wrenStringFromByte(WrenVM *vm, uint8_t value)
{
  int length = 1;
  ObjString* string = allocateString(vm, length);
  string->value[0] = value;
  hashString(string);
  return OBJ_VAL(string);
}

Value wrenStringFormat(WrenVM* vm, const char* format, ...)
{
  va_list argList;

  // Calculate the length of the result string. Do this up front so we can
  // create the final string with a single allocation.
  va_start(argList, format);
  size_t totalLength = 0;
  for (const char* c = format; *c != '\0'; c++)
  {
    switch (*c)
    {
      case '$':
        totalLength += strlen(va_arg(argList, const char*));
        break;

      case '@':
        totalLength += AS_STRING(va_arg(argList, Value))->length;
        break;

      default:
        // Any other character is interpreted literally.
        totalLength++;
    }
  }
  va_end(argList);

  // Concatenate the string.
  ObjString* result = allocateString(vm, totalLength);

  va_start(argList, format);
  char* start = result->value;
  for (const char* c = format; *c != '\0'; c++)
  {
    switch (*c)
    {
      case '$':
      {
        const char* string = va_arg(argList, const char*);
        size_t length = strlen(string);
        memcpy(start, string, length);
        start += length;
        break;
      }

      case '@':
      {
        ObjString* string = AS_STRING(va_arg(argList, Value));
        memcpy(start, string->value, string->length);
        start += string->length;
        break;
      }

      default:
        // Any other character is interpreted literally.
        *start++ = *c;
    }
  }
  va_end(argList);

  hashString(result);

  return OBJ_VAL(result);
}

Value wrenStringCodePointAt(WrenVM* vm, ObjString* string, uint32_t index)
{
  ASSERT(index < string->length, "Index out of bounds.");

  int codePoint = wrenUtf8Decode((uint8_t*)string->value + index,
                                 string->length - index);
  if (codePoint == -1)
  {
    // If it isn't a valid UTF-8 sequence, treat it as a single raw byte.
    char bytes[2];
    bytes[0] = string->value[index];
    bytes[1] = '\0';
    return wrenNewStringLength(vm, bytes, 1);
  }

  return wrenStringFromCodePoint(vm, codePoint);
}

// Uses the Boyer-Moore-Horspool string matching algorithm.
uint32_t wrenStringFind(ObjString* haystack, ObjString* needle, uint32_t start)
{
  // Edge case: An empty needle is always found.
  if (needle->length == 0) return start;

  // If the needle goes past the haystack it won't be found.
  if (start + needle->length > haystack->length) return UINT32_MAX;

  // If the startIndex is too far it also won't be found.
  if (start >= haystack->length) return UINT32_MAX;

  // Pre-calculate the shift table. For each character (8-bit value), we
  // determine how far the search window can be advanced if that character is
  // the last character in the haystack where we are searching for the needle
  // and the needle doesn't match there.
  uint32_t shift[UINT8_MAX];
  uint32_t needleEnd = needle->length - 1;

  // By default, we assume the character is not the needle at all. In that case
  // case, if a match fails on that character, we can advance one whole needle
  // width since.
  for (uint32_t index = 0; index < UINT8_MAX; index++)
  {
    shift[index] = needle->length;
  }

  // Then, for every character in the needle, determine how far it is from the
  // end. If a match fails on that character, we can advance the window such
  // that it the last character in it lines up with the last place we could
  // find it in the needle.
  for (uint32_t index = 0; index < needleEnd; index++)
  {
    char c = needle->value[index];
    shift[(uint8_t)c] = needleEnd - index;
  }

  // Slide the needle across the haystack, looking for the first match or
  // stopping if the needle goes off the end.
  char lastChar = needle->value[needleEnd];
  uint32_t range = haystack->length - needle->length;

  for (uint32_t index = start; index <= range; )
  {
    // Compare the last character in the haystack's window to the last character
    // in the needle. If it matches, see if the whole needle matches.
    char c = haystack->value[index + needleEnd];
    if (lastChar == c &&
        memcmp(haystack->value + index, needle->value, needleEnd) == 0)
    {
      // Found a match.
      return index;
    }

    // Otherwise, slide the needle forward.
    index += shift[(uint8_t)c];
  }

  // Not found.
  return UINT32_MAX;
}

ObjUpvalue* wrenNewUpvalue(WrenVM* vm, Value* value)
{
  ObjUpvalue* upvalue = ALLOCATE(vm, ObjUpvalue);

  // Upvalues are never used as first-class objects, so don't need a class.
  initObj(vm, &upvalue->obj, OBJ_UPVALUE, NULL);

  upvalue->value = value;
  upvalue->closed = NULL_VAL;
  upvalue->next = NULL;
  return upvalue;
}

void wrenGrayObj(WrenVM* vm, Obj* obj)
{
  if (obj == NULL) return;

  // Stop if the object is already darkened so we don't get stuck in a cycle.
  if (obj->isDark) return;

  // It's been reached.
  obj->isDark = true;

  // Add it to the gray list so it can be recursively explored for
  // more marks later.
  if (vm->grayCount >= vm->grayCapacity)
  {
    vm->grayCapacity = vm->grayCount * 2;
    vm->gray = (Obj**)vm->config.reallocateFn(vm->gray,
                                              vm->grayCapacity * sizeof(Obj*),
                                              vm->config.userData);
  }

  vm->gray[vm->grayCount++] = obj;
}

void wrenGrayValue(WrenVM* vm, Value value)
{
  if (!IS_OBJ(value)) return;
  wrenGrayObj(vm, AS_OBJ(value));
}

void wrenGrayBuffer(WrenVM* vm, ValueBuffer* buffer)
{
  for (int i = 0; i < buffer->count; i++)
  {
    wrenGrayValue(vm, buffer->data[i]);
  }
}

static void blackenClass(WrenVM* vm, ObjClass* classObj)
{
  // The metaclass.
  wrenGrayObj(vm, (Obj*)classObj->obj.classObj);

  // The superclass.
  wrenGrayObj(vm, (Obj*)classObj->superclass);

  // Method function objects.
  for (int i = 0; i < classObj->methods.count; i++)
  {
    if (classObj->methods.data[i].type == METHOD_BLOCK)
    {
      wrenGrayObj(vm, (Obj*)classObj->methods.data[i].as.closure);
    }
  }

  wrenGrayObj(vm, (Obj*)classObj->name);

  if(!IS_NULL(classObj->attributes)) wrenGrayObj(vm, AS_OBJ(classObj->attributes));

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjClass);
  vm->bytesAllocated += classObj->methods.capacity * sizeof(Method);
}

static void blackenClosure(WrenVM* vm, ObjClosure* closure)
{
  // Mark the function.
  wrenGrayObj(vm, (Obj*)closure->fn);

  // Mark the upvalues.
  for (int i = 0; i < closure->fn->numUpvalues; i++)
  {
    wrenGrayObj(vm, (Obj*)closure->upvalues[i]);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjClosure);
  vm->bytesAllocated += sizeof(ObjUpvalue*) * closure->fn->numUpvalues;
}

static void blackenFiber(WrenVM* vm, ObjFiber* fiber)
{
  // Stack functions.
  for (int i = 0; i < fiber->numFrames; i++)
  {
    wrenGrayObj(vm, (Obj*)fiber->frames[i].closure);
  }

  // Stack variables.
  for (Value* slot = fiber->stack; slot < fiber->stackTop; slot++)
  {
    wrenGrayValue(vm, *slot);
  }

  // Open upvalues.
  ObjUpvalue* upvalue = fiber->openUpvalues;
  while (upvalue != NULL)
  {
    wrenGrayObj(vm, (Obj*)upvalue);
    upvalue = upvalue->next;
  }

  // The caller.
  wrenGrayObj(vm, (Obj*)fiber->caller);
  wrenGrayValue(vm, fiber->error);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjFiber);
  vm->bytesAllocated += fiber->frameCapacity * sizeof(CallFrame);
  vm->bytesAllocated += fiber->stackCapacity * sizeof(Value);
}

static void blackenFn(WrenVM* vm, ObjFn* fn)
{
  // Mark the constants.
  wrenGrayBuffer(vm, &fn->constants);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjFn);
  vm->bytesAllocated += sizeof(uint8_t) * fn->code.capacity;
  vm->bytesAllocated += sizeof(Value) * fn->constants.capacity;
  
  // The debug line number buffer.
  vm->bytesAllocated += sizeof(int) * fn->code.capacity;
  // TODO: What about the function name?
}

static void blackenForeign(WrenVM* vm, ObjForeign* foreign)
{
  // TODO: Keep track of how much memory the foreign object uses. We can store
  // this in each foreign object, but it will balloon the size. We may not want
  // that much overhead. One option would be to let the foreign class register
  // a C function that returns a size for the object. That way the VM doesn't
  // always have to explicitly store it.
}

static void blackenInstance(WrenVM* vm, ObjInstance* instance)
{
  wrenGrayObj(vm, (Obj*)instance->obj.classObj);

  // Mark the fields.
  for (int i = 0; i < instance->obj.classObj->numFields; i++)
  {
    wrenGrayValue(vm, instance->fields[i]);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjInstance);
  vm->bytesAllocated += sizeof(Value) * instance->obj.classObj->numFields;
}

static void blackenList(WrenVM* vm, ObjList* list)
{
  // Mark the elements.
  wrenGrayBuffer(vm, &list->elements);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjList);
  vm->bytesAllocated += sizeof(Value) * list->elements.capacity;
}

static void blackenMap(WrenVM* vm, ObjMap* map)
{
  // Mark the entries.
  for (uint32_t i = 0; i < map->capacity; i++)
  {
    MapEntry* entry = &map->entries[i];
    if (IS_UNDEFINED(entry->key)) continue;

    wrenGrayValue(vm, entry->key);
    wrenGrayValue(vm, entry->value);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjMap);
  vm->bytesAllocated += sizeof(MapEntry) * map->capacity;
}

static void blackenModule(WrenVM* vm, ObjModule* module)
{
  // Top-level variables.
  for (int i = 0; i < module->variables.count; i++)
  {
    wrenGrayValue(vm, module->variables.data[i]);
  }

  wrenBlackenSymbolTable(vm, &module->variableNames);

  wrenGrayObj(vm, (Obj*)module->name);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjModule);
}

static void blackenRange(WrenVM* vm, ObjRange* range)
{
  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjRange);
}

static void blackenString(WrenVM* vm, ObjString* string)
{
  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjString) + string->length + 1;
}

static void blackenUpvalue(WrenVM* vm, ObjUpvalue* upvalue)
{
  // Mark the closed-over object (in case it is closed).
  wrenGrayValue(vm, upvalue->closed);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjUpvalue);
}

static void blackenObject(WrenVM* vm, Obj* obj)
{
#if WREN_DEBUG_TRACE_MEMORY
  printf("mark ");
  wrenDumpValue(OBJ_VAL(obj));
  printf(" @ %p\n", obj);
#endif

  // Traverse the object's fields.
  switch (obj->type)
  {
    case OBJ_CLASS:    blackenClass(   vm, (ObjClass*)   obj); break;
    case OBJ_CLOSURE:  blackenClosure( vm, (ObjClosure*) obj); break;
    case OBJ_FIBER:    blackenFiber(   vm, (ObjFiber*)   obj); break;
    case OBJ_FN:       blackenFn(      vm, (ObjFn*)      obj); break;
    case OBJ_FOREIGN:  blackenForeign( vm, (ObjForeign*) obj); break;
    case OBJ_INSTANCE: blackenInstance(vm, (ObjInstance*)obj); break;
    case OBJ_LIST:     blackenList(    vm, (ObjList*)    obj); break;
    case OBJ_MAP:      blackenMap(     vm, (ObjMap*)     obj); break;
    case OBJ_MODULE:   blackenModule(  vm, (ObjModule*)  obj); break;
    case OBJ_RANGE:    blackenRange(   vm, (ObjRange*)   obj); break;
    case OBJ_STRING:   blackenString(  vm, (ObjString*)  obj); break;
    case OBJ_UPVALUE:  blackenUpvalue( vm, (ObjUpvalue*) obj); break;
  }
}

void wrenBlackenObjects(WrenVM* vm)
{
  while (vm->grayCount > 0)
  {
    // Pop an item from the gray stack.
    Obj* obj = vm->gray[--vm->grayCount];
    blackenObject(vm, obj);
  }
}

void wrenFreeObj(WrenVM* vm, Obj* obj)
{
#if WREN_DEBUG_TRACE_MEMORY
  printf("free ");
  wrenDumpValue(OBJ_VAL(obj));
  printf(" @ %p\n", obj);
#endif

  switch (obj->type)
  {
    case OBJ_CLASS:
      wrenMethodBufferClear(vm, &((ObjClass*)obj)->methods);
      wrenForeignMethodUserDataBufferClear(vm, &((ObjClass*)obj)->foreignMethodUserDatas);
      break;

    case OBJ_FIBER:
    {
      ObjFiber* fiber = (ObjFiber*)obj;
      DEALLOCATE(vm, fiber->frames);
      DEALLOCATE(vm, fiber->stack);
      break;
    }
      
    case OBJ_FN:
    {
      ObjFn* fn = (ObjFn*)obj;
      wrenValueBufferClear(vm, &fn->constants);
      wrenByteBufferClear(vm, &fn->code);
      wrenIntBufferClear(vm, &fn->debug->sourceLines);
      DEALLOCATE(vm, fn->debug->name);
      DEALLOCATE(vm, fn->debug);
      break;
    }

    case OBJ_FOREIGN:
      wrenFinalizeForeign(vm, (ObjForeign*)obj);
      break;

    case OBJ_LIST:
      wrenValueBufferClear(vm, &((ObjList*)obj)->elements);
      break;

    case OBJ_MAP:
      DEALLOCATE(vm, ((ObjMap*)obj)->entries);
      break;

    case OBJ_MODULE:
      wrenSymbolTableClear(vm, &((ObjModule*)obj)->variableNames);
      wrenValueBufferClear(vm, &((ObjModule*)obj)->variables);
      break;

    case OBJ_CLOSURE:
    case OBJ_INSTANCE:
    case OBJ_RANGE:
    case OBJ_STRING:
    case OBJ_UPVALUE:
      break;
  }

  DEALLOCATE(vm, obj);
}

ObjClass* wrenGetClass(WrenVM* vm, Value value)
{
  return wrenGetClassInline(vm, value);
}

bool wrenValuesEqual(Value a, Value b)
{
  if (wrenValuesSame(a, b)) return true;

  // If we get here, it's only possible for two heap-allocated immutable objects
  // to be equal.
  if (!IS_OBJ(a) || !IS_OBJ(b)) return false;

  Obj* aObj = AS_OBJ(a);
  Obj* bObj = AS_OBJ(b);

  // Must be the same type.
  if (aObj->type != bObj->type) return false;

  switch (aObj->type)
  {
    case OBJ_RANGE:
    {
      ObjRange* aRange = (ObjRange*)aObj;
      ObjRange* bRange = (ObjRange*)bObj;
      return aRange->from == bRange->from &&
             aRange->to == bRange->to &&
             aRange->isInclusive == bRange->isInclusive;
    }

    case OBJ_STRING:
    {
      ObjString* aString = (ObjString*)aObj;
      ObjString* bString = (ObjString*)bObj;
      return aString->hash == bString->hash &&
      wrenStringEqualsCString(aString, bString->value, bString->length);
    }

    default:
      // All other types are only equal if they are same, which they aren't if
      // we get here.
      return false;
  }
}
