#ifndef wren_vm_h
#define wren_vm_h

#include "wren_common.h"
#include "wren_compiler.h"
#include "wren_value.h"
#include "wren_utils.h"

// The maximum number of temporary objects that can be made visible to the GC
// at one time.
#define WREN_MAX_TEMP_ROOTS 5

typedef enum
{
  // Load the constant at index [arg].
  CODE_CONSTANT,

  // Push null onto the stack.
  CODE_NULL,

  // Push false onto the stack.
  CODE_FALSE,

  // Push true onto the stack.
  CODE_TRUE,

  // Pushes the value in the given local slot.
  CODE_LOAD_LOCAL_0,
  CODE_LOAD_LOCAL_1,
  CODE_LOAD_LOCAL_2,
  CODE_LOAD_LOCAL_3,
  CODE_LOAD_LOCAL_4,
  CODE_LOAD_LOCAL_5,
  CODE_LOAD_LOCAL_6,
  CODE_LOAD_LOCAL_7,
  CODE_LOAD_LOCAL_8,

  // Note: The compiler assumes the following _STORE instructions always
  // immediately follow their corresponding _LOAD ones.

  // Pushes the value in local slot [arg].
  CODE_LOAD_LOCAL,

  // Stores the top of stack in local slot [arg]. Does not pop it.
  CODE_STORE_LOCAL,

  // Pushes the value in upvalue [arg].
  CODE_LOAD_UPVALUE,

  // Stores the top of stack in upvalue [arg]. Does not pop it.
  CODE_STORE_UPVALUE,

  // Pushes the value of the top-level variable in slot [arg].
  CODE_LOAD_MODULE_VAR,

  // Stores the top of stack in top-level variable slot [arg]. Does not pop it.
  CODE_STORE_MODULE_VAR,

  // Pushes the value of the field in slot [arg] of the receiver of the current
  // function. This is used for regular field accesses on "this" directly in
  // methods. This instruction is faster than the more general CODE_LOAD_FIELD
  // instruction.
  CODE_LOAD_FIELD_THIS,

  // Stores the top of the stack in field slot [arg] in the receiver of the
  // current value. Does not pop the value. This instruction is faster than the
  // more general CODE_LOAD_FIELD instruction.
  CODE_STORE_FIELD_THIS,

  // Pops an instance and pushes the value of the field in slot [arg] of it.
  CODE_LOAD_FIELD,

  // Pops an instance and stores the subsequent top of stack in field slot
  // [arg] in it. Does not pop the value.
  CODE_STORE_FIELD,

  // Pop and discard the top of stack.
  CODE_POP,

  // Push a copy of the value currently on the top of the stack.
  CODE_DUP,

  // Invoke the method with symbol [arg]. The number indicates the number of
  // arguments (not including the receiver).
  CODE_CALL_0,
  CODE_CALL_1,
  CODE_CALL_2,
  CODE_CALL_3,
  CODE_CALL_4,
  CODE_CALL_5,
  CODE_CALL_6,
  CODE_CALL_7,
  CODE_CALL_8,
  CODE_CALL_9,
  CODE_CALL_10,
  CODE_CALL_11,
  CODE_CALL_12,
  CODE_CALL_13,
  CODE_CALL_14,
  CODE_CALL_15,
  CODE_CALL_16,

  // Invoke a superclass method with symbol [arg]. The number indicates the
  // number of arguments (not including the receiver).
  CODE_SUPER_0,
  CODE_SUPER_1,
  CODE_SUPER_2,
  CODE_SUPER_3,
  CODE_SUPER_4,
  CODE_SUPER_5,
  CODE_SUPER_6,
  CODE_SUPER_7,
  CODE_SUPER_8,
  CODE_SUPER_9,
  CODE_SUPER_10,
  CODE_SUPER_11,
  CODE_SUPER_12,
  CODE_SUPER_13,
  CODE_SUPER_14,
  CODE_SUPER_15,
  CODE_SUPER_16,

  // Jump the instruction pointer [arg] forward.
  CODE_JUMP,

  // Jump the instruction pointer [arg] backward. Pop and discard the top of
  // the stack.
  CODE_LOOP,

  // Pop and if not truthy then jump the instruction pointer [arg] forward.
  CODE_JUMP_IF,

  // If the top of the stack is false, jump [arg] forward. Otherwise, pop and
  // continue.
  CODE_AND,

  // If the top of the stack is non-false, jump [arg] forward. Otherwise, pop
  // and continue.
  CODE_OR,

  // Pop [a] then [b] and push true if [b] is an instance of [a].
  CODE_IS,

  // Close the upvalue for the local on the top of the stack, then pop it.
  CODE_CLOSE_UPVALUE,

  // Exit from the current function and return the value on the top of the
  // stack.
  CODE_RETURN,

  // Creates a closure for the function stored at [arg] in the constant table.
  //
  // Following the function argument is a number of arguments, two for each
  // upvalue. The first is true if the variable being captured is a local (as
  // opposed to an upvalue), and the second is the index of the local or
  // upvalue being captured.
  //
  // Pushes the created closure.
  CODE_CLOSURE,

  // Creates a class. Top of stack is the superclass, or `null` if the class
  // inherits Object. Below that is a string for the name of the class. Byte
  // [arg] is the number of fields in the class.
  CODE_CLASS,

  // Define a method for symbol [arg]. The class receiving the method is popped
  // off the stack, then the function defining the body is popped.
  CODE_METHOD_INSTANCE,

  // Define a method for symbol [arg]. The class whose metaclass will receive
  // the method is popped off the stack, then the function defining the body is
  // popped.
  CODE_METHOD_STATIC,

  // Load the module whose name is stored in string constant [arg]. Pushes
  // NULL onto the stack. If the module has already been loaded, does nothing
  // else. Otherwise, it creates a fiber to run the desired module and switches
  // to that. When that fiber is done, the current one is resumed.
  CODE_LOAD_MODULE,

  // Reads a top-level variable from another module. [arg1] is a string
  // constant for the name of the module, and [arg2] is a string constant for
  // the variable name. Pushes the variable if found, or generates a runtime
  // error otherwise.
  CODE_IMPORT_VARIABLE,

  // This pseudo-instruction indicates the end of the bytecode. It should
  // always be preceded by a `CODE_RETURN`, so is never actually executed.
  CODE_END
} Code;

struct WrenMethod
{
  // The fiber that invokes the method. Its stack is pre-populated with the
  // receiver for the method, and it contains a single callframe whose function
  // is the bytecode stub to invoke the method.
  ObjFiber* fiber;

  WrenMethod* prev;
  WrenMethod* next;
};

struct WrenVM
{
  ObjClass* boolClass;
  ObjClass* classClass;
  ObjClass* fiberClass;
  ObjClass* fnClass;
  ObjClass* listClass;
  ObjClass* mapClass;
  ObjClass* nullClass;
  ObjClass* numClass;
  ObjClass* objectClass;
  ObjClass* rangeClass;
  ObjClass* stringClass;

  // The fiber that is currently running.
  ObjFiber* fiber;

  // The loaded modules. Each key is an ObjString (except for the main module,
  // whose key is null) for the module's name and the value is the ObjModule
  // for the module.
  ObjMap* modules;

  // Memory management data:

  // The externally-provided function used to allocate memory.
  WrenReallocateFn reallocate;

  // The number of bytes that are known to be currently allocated. Includes all
  // memory that was proven live after the last GC, as well as any new bytes
  // that were allocated since then. Does *not* include bytes for objects that
  // were freed since the last GC.
  size_t bytesAllocated;

  // The number of total allocated bytes that will trigger the next GC.
  size_t nextGC;

  // The minimum value for [nextGC] when recalculated after a collection.
  size_t minNextGC;

  // The scale factor used to calculate [nextGC] from the current number of in
  // use bytes, as a percent. For example, 150 here means that nextGC will be
  // 50% larger than the current number of in-use bytes.
  int heapScalePercent;

  // The first object in the linked list of all currently allocated objects.
  Obj* first;

  // The list of temporary roots. This is for temporary or new objects that are
  // not otherwise reachable but should not be collected.
  //
  // They are organized as a stack of pointers stored in this array. This
  // implies that temporary roots need to have stack semantics: only the most
  // recently pushed object can be released.
  Obj* tempRoots[WREN_MAX_TEMP_ROOTS];

  int numTempRoots;

  // Foreign function data:

  // During a foreign function call, this will point to the first argument (the
  // receiver) of the call on the fiber's stack.
  Value* foreignCallSlot;

  // Pointer to the first node in the linked list of active method handles or
  // NULL if there are no handles.
  WrenMethod* methodHandles;

  // During a foreign function call, this will contain the number of arguments
  // to the function.
  int foreignCallNumArgs;

  // The function used to load modules.
  WrenLoadModuleFn loadModule;

  // Compiler and debugger data:

  // The compiler that is currently compiling code. This is used so that heap
  // allocated objects used by the compiler can be found if a GC is kicked off
  // in the middle of a compile.
  Compiler* compiler;

  // There is a single global symbol table for all method names on all classes.
  // Method calls are dispatched directly by index in this table.
  SymbolTable methodNames;
};

// A generic allocation function that handles all explicit memory management.
// It's used like so:
//
// - To allocate new memory, [memory] is NULL and [oldSize] is zero. It should
//   return the allocated memory or NULL on failure.
//
// - To attempt to grow an existing allocation, [memory] is the memory,
//   [oldSize] is its previous size, and [newSize] is the desired size.
//   It should return [memory] if it was able to grow it in place, or a new
//   pointer if it had to move it.
//
// - To shrink memory, [memory], [oldSize], and [newSize] are the same as above
//   but it will always return [memory].
//
// - To free memory, [memory] will be the memory to free and [newSize] and
//   [oldSize] will be zero. It should return NULL.
void* wrenReallocate(WrenVM* vm, void* memory, size_t oldSize, size_t newSize);

// Imports the module with [name].
//
// If the module has already been imported (or is already in the middle of
// being imported, in the case of a circular import), returns true. Otherwise,
// returns a new fiber that will execute the module's code. That fiber should
// be called before any variables are loaded from the module.
//
// If the module could not be found, returns false.
Value wrenImportModule(WrenVM* vm, const char* name);

// Returns the value of the module-level variable named [name] in the main
// module.
Value wrenFindVariable(WrenVM* vm, const char* name);

// Adds a new implicitly declared top-level variable named [name] to [module].
//
// If [module] is `NULL`, uses the main module.
//
// Does not check to see if a variable with that name is already declared or
// defined. Returns the symbol for the new variable or -2 if there are too many
// variables defined.
int wrenDeclareVariable(WrenVM* vm, ObjModule* module, const char* name,
                        size_t length);

// Adds a new top-level variable named [name] to [module].
//
// If [module] is `NULL`, uses the main module.
//
// Returns the symbol for the new variable, -1 if a variable with the given name
// is already defined, or -2 if there are too many variables defined.
int wrenDefineVariable(WrenVM* vm, ObjModule* module, const char* name,
                       size_t length, Value value);

// Sets the current Compiler being run to [compiler].
void wrenSetCompiler(WrenVM* vm, Compiler* compiler);

// Mark [obj] as a GC root so that it doesn't get collected.
void wrenPushRoot(WrenVM* vm, Obj* obj);

// Remove the most recently pushed temporary root.
void wrenPopRoot(WrenVM* vm);

// Returns the class of [value].
//
// Defined here instead of in wren_value.h because it's critical that this be
// inlined. That means it must be defined in the header, but the wren_value.h
// header doesn't have a full definitely of WrenVM yet.
static inline ObjClass* wrenGetClassInline(WrenVM* vm, Value value)
{
  if (IS_NUM(value)) return vm->numClass;
  if (IS_OBJ(value)) return AS_OBJ(value)->classObj;

#if WREN_NAN_TAGGING
  switch (GET_TAG(value))
  {
    case TAG_FALSE: return vm->boolClass; break;
    case TAG_NAN: return vm->numClass; break;
    case TAG_NULL: return vm->nullClass; break;
    case TAG_TRUE: return vm->boolClass; break;
    case TAG_UNDEFINED: UNREACHABLE();
  }
#else
  switch (value.type)
  {
    case VAL_FALSE: return vm->boolClass;
    case VAL_NULL: return vm->nullClass;
    case VAL_NUM: return vm->numClass;
    case VAL_TRUE: return vm->boolClass;
    case VAL_OBJ: return AS_OBJ(value)->classObj;
    case VAL_UNDEFINED: UNREACHABLE();
  }
#endif

  UNREACHABLE();
  return NULL;
}

#endif
