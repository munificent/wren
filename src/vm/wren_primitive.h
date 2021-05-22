#ifndef wren_primitive_h
#define wren_primitive_h

#include "wren_vm.h"

// Binds a primitive method named [name] (in Wren) implemented using C function
// [fn] to `ObjClass` [cls].
#define PRIMITIVE(cls, name, function)                                         \
    do                                                                         \
    {                                                                          \
      int symbol = wrenSymbolTableEnsure(vm,                                   \
          &vm->methodNames, name, strlen(name));                               \
      Method method;                                                           \
      method.type = METHOD_PRIMITIVE;                                          \
      method.as.primitive = prim_##function;                                   \
      wrenBindMethod(vm, cls, symbol, method, WREN_USER_DATA_NONE);            \
    } while (false)

// Binds a primitive method named [name] (in Wren) implemented using C function
// [fn] to `ObjClass` [cls], but as a FN call.
#define FUNCTION_CALL(cls, name, function)                                     \
    do                                                                         \
    {                                                                          \
      int symbol = wrenSymbolTableEnsure(vm,                                   \
          &vm->methodNames, name, strlen(name));                               \
      Method method;                                                           \
      method.type = METHOD_FUNCTION_CALL;                                      \
      method.as.primitive = prim_##function;                                   \
      wrenBindMethod(vm, cls, symbol, method, WREN_USER_DATA_NONE);            \
    } while (false)

// Defines a primitive method whose C function name is [name]. This abstracts
// the actual type signature of a primitive function and makes it clear which C
// functions are invoked as primitives.
#define DEF_PRIMITIVE(name)                                                    \
    static bool prim_##name(WrenVM* vm, Value* args)

#define RETURN_VAL(value)                                                      \
    do                                                                         \
    {                                                                          \
      args[0] = value;                                                         \
      return true;                                                             \
    } while (false)

#define RETURN_OBJ(obj)     RETURN_VAL(OBJ_VAL(obj))
#define RETURN_BOOL(value)  RETURN_VAL(BOOL_VAL(value))
#define RETURN_FALSE        RETURN_VAL(FALSE_VAL)
#define RETURN_NULL         RETURN_VAL(NULL_VAL)
#define RETURN_NUM(value)   RETURN_VAL(NUM_VAL(value))
#define RETURN_TRUE         RETURN_VAL(TRUE_VAL)

#define RETURN_ERROR(msg)                                                      \
    do                                                                         \
    {                                                                          \
      vm->fiber->error = wrenNewStringLength(vm, msg, sizeof(msg) - 1);        \
      return false;                                                            \
    } while (false)

#define RETURN_ERROR_FMT(...)                                                  \
    do                                                                         \
    {                                                                          \
      vm->fiber->error = wrenStringFormat(vm, __VA_ARGS__);                    \
      return false;                                                            \
    } while (false)

// Validates that the given [arg] is a function. Returns true if it is. If not,
// reports an error and returns false.
bool validateFn(WrenVM* vm, Value arg, const char* argName);

// Validates that the given [arg] is a Num. Returns true if it is. If not,
// reports an error and returns false.
bool validateNum(WrenVM* vm, Value arg, const char* argName);

// Validates that [value] is an integer. Returns true if it is. If not, reports
// an error and returns false.
bool validateIntValue(WrenVM* vm, double value, const char* argName);

// Validates that the given [arg] is an integer. Returns true if it is. If not,
// reports an error and returns false.
bool validateInt(WrenVM* vm, Value arg, const char* argName);

// Validates that [arg] is a valid object for use as a map key. Returns true if
// it is. If not, reports an error and returns false.
bool validateKey(WrenVM* vm, Value arg);

// Validates that the argument at [argIndex] is an integer within `[0, count)`.
// Also allows negative indices which map backwards from the end. Returns the
// valid positive index value. If invalid, reports an error and returns
// `UINT32_MAX`.
uint32_t validateIndex(WrenVM* vm, Value arg, uint32_t count,
                       const char* argName);

// Validates that the given [arg] is a String. Returns true if it is. If not,
// reports an error and returns false.
bool validateString(WrenVM* vm, Value arg, const char* argName);

// Given a [range] and the [length] of the object being operated on, determines
// the series of elements that should be chosen from the underlying object.
// Handles ranges that count backwards from the end as well as negative ranges.
//
// Returns the index from which the range should start or `UINT32_MAX` if the
// range is invalid. After calling, [length] will be updated with the number of
// elements in the resulting sequence. [step] will be direction that the range
// is going: `1` if the range is increasing from the start index or `-1` if the
// range is decreasing.
uint32_t calculateRange(WrenVM* vm, ObjRange* range, uint32_t* length,
                        int* step);

#endif
