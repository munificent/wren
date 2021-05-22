// This defines the bytecode instructions used by the VM. It does so by invoking
// an OPCODE() macro which is expected to be defined at the point that this is
// included. (See: http://en.wikipedia.org/wiki/X_Macro for more.)
//
// The first argument is the name of the opcode. The second is its "stack
// effect" -- the amount that the op code changes the size of the stack. A
// stack effect of 1 means it pushes a value and the stack grows one larger.
// -2 means it pops two values, etc.
//
// Note that the order of instructions here affects the order of the dispatch
// table in the VM's interpreter loop. That in turn affects caching which
// affects overall performance. Take care to run benchmarks if you change the
// order here.

// Load the constant at index [arg].
OPCODE(CONSTANT, 1)

// Push null onto the stack.
OPCODE(NULL, 1)

// Push false onto the stack.
OPCODE(FALSE, 1)

// Push true onto the stack.
OPCODE(TRUE, 1)

// Pushes the value in the given local slot.
OPCODE(LOAD_LOCAL_0, 1)
OPCODE(LOAD_LOCAL_1, 1)
OPCODE(LOAD_LOCAL_2, 1)
OPCODE(LOAD_LOCAL_3, 1)
OPCODE(LOAD_LOCAL_4, 1)
OPCODE(LOAD_LOCAL_5, 1)
OPCODE(LOAD_LOCAL_6, 1)
OPCODE(LOAD_LOCAL_7, 1)
OPCODE(LOAD_LOCAL_8, 1)

// Note: The compiler assumes the following _STORE instructions always
// immediately follow their corresponding _LOAD ones.

// Pushes the value in local slot [arg].
OPCODE(LOAD_LOCAL, 1)

// Stores the top of stack in local slot [arg]. Does not pop it.
OPCODE(STORE_LOCAL, 0)

// Pushes the value in upvalue [arg].
OPCODE(LOAD_UPVALUE, 1)

// Stores the top of stack in upvalue [arg]. Does not pop it.
OPCODE(STORE_UPVALUE, 0)

// Pushes the value of the top-level variable in slot [arg].
OPCODE(LOAD_MODULE_VAR, 1)

// Stores the top of stack in top-level variable slot [arg]. Does not pop it.
OPCODE(STORE_MODULE_VAR, 0)

// Pushes the value of the field in slot [arg] of the receiver of the current
// function. This is used for regular field accesses on "this" directly in
// methods. This instruction is faster than the more general CODE_LOAD_FIELD
// instruction.
OPCODE(LOAD_FIELD_THIS, 1)

// Stores the top of the stack in field slot [arg] in the receiver of the
// current value. Does not pop the value. This instruction is faster than the
// more general CODE_LOAD_FIELD instruction.
OPCODE(STORE_FIELD_THIS, 0)

// Pops an instance and pushes the value of the field in slot [arg] of it.
OPCODE(LOAD_FIELD, 0)

// Pops an instance and stores the subsequent top of stack in field slot
// [arg] in it. Does not pop the value.
OPCODE(STORE_FIELD, -1)

// Pop and discard the top of stack.
OPCODE(POP, -1)

// Invoke the method with symbol [arg]. The number indicates the number of
// arguments (not including the receiver).
OPCODE(CALL_0, 0)
OPCODE(CALL_1, -1)
OPCODE(CALL_2, -2)
OPCODE(CALL_3, -3)
OPCODE(CALL_4, -4)
OPCODE(CALL_5, -5)
OPCODE(CALL_6, -6)
OPCODE(CALL_7, -7)
OPCODE(CALL_8, -8)
OPCODE(CALL_9, -9)
OPCODE(CALL_10, -10)
OPCODE(CALL_11, -11)
OPCODE(CALL_12, -12)
OPCODE(CALL_13, -13)
OPCODE(CALL_14, -14)
OPCODE(CALL_15, -15)
OPCODE(CALL_16, -16)

// Invoke a superclass method with symbol [arg]. The number indicates the
// number of arguments (not including the receiver).
OPCODE(SUPER_0, 0)
OPCODE(SUPER_1, -1)
OPCODE(SUPER_2, -2)
OPCODE(SUPER_3, -3)
OPCODE(SUPER_4, -4)
OPCODE(SUPER_5, -5)
OPCODE(SUPER_6, -6)
OPCODE(SUPER_7, -7)
OPCODE(SUPER_8, -8)
OPCODE(SUPER_9, -9)
OPCODE(SUPER_10, -10)
OPCODE(SUPER_11, -11)
OPCODE(SUPER_12, -12)
OPCODE(SUPER_13, -13)
OPCODE(SUPER_14, -14)
OPCODE(SUPER_15, -15)
OPCODE(SUPER_16, -16)

// Jump the instruction pointer [arg] forward.
OPCODE(JUMP, 0)

// Jump the instruction pointer [arg] backward.
OPCODE(LOOP, 0)

// Pop and if not truthy then jump the instruction pointer [arg] forward.
OPCODE(JUMP_IF, -1)

// If the top of the stack is false, jump [arg] forward. Otherwise, pop and
// continue.
OPCODE(AND, -1)

// If the top of the stack is non-false, jump [arg] forward. Otherwise, pop
// and continue.
OPCODE(OR, -1)

// Close the upvalue for the local on the top of the stack, then pop it.
OPCODE(CLOSE_UPVALUE, -1)

// Exit from the current function and return the value on the top of the
// stack.
OPCODE(RETURN, 0)

// Exit from the current function and return several values on the top of
// the stack.
OPCODE(RETURN_MULTIPLE, 0)

// Creates a closure for the function stored at [arg] in the constant table.
//
// Following the function argument is a number of arguments, two for each
// upvalue. The first is true if the variable being captured is a local (as
// opposed to an upvalue), and the second is the index of the local or
// upvalue being captured.
//
// Pushes the created closure.
OPCODE(CLOSURE, 1)

// Creates a new instance of a class.
//
// Assumes the class object is in slot zero, and replaces it with the new
// uninitialized instance of that class. This opcode is only emitted by the
// compiler-generated constructor metaclass methods.
OPCODE(CONSTRUCT, 0)

// Creates a new instance of a foreign class.
//
// Assumes the class object is in slot zero, and replaces it with the new
// uninitialized instance of that class. This opcode is only emitted by the
// compiler-generated constructor metaclass methods.
OPCODE(FOREIGN_CONSTRUCT, 0)

// Creates a class. Top of stack is the superclass. Below that is a string for
// the name of the class. Byte [arg] is the number of fields in the class.
OPCODE(CLASS, -1)

// Ends a class. 
// Atm the stack contains the class and the ClassAttributes (or null).
OPCODE(END_CLASS, -2)

// Creates a foreign class. Top of stack is the superclass. Below that is a
// string for the name of the class.
OPCODE(FOREIGN_CLASS, -1)

// Define a method for symbol [arg]. The class receiving the method is popped
// off the stack, then the function defining the body is popped.
//
// If a foreign method is being defined, the "function" will be a string
// identifying the foreign method. Otherwise, it will be a function or
// closure.
OPCODE(METHOD_INSTANCE, -2)

// Define a method for symbol [arg]. The class whose metaclass will receive
// the method is popped off the stack, then the function defining the body is
// popped.
//
// If a foreign method is being defined, the "function" will be a string
// identifying the foreign method. Otherwise, it will be a function or
// closure.
OPCODE(METHOD_STATIC, -2)

// This is executed at the end of the module's body. Pushes NULL onto the stack
// as the "return value" of the import statement and stores the module as the
// most recently imported one.
OPCODE(END_MODULE, 1)

// Import a module whose name is the string stored at [arg] in the constant
// table.
//
// Pushes null onto the stack so that the fiber for the imported module can
// replace that with a dummy value when it returns. (Fibers always return a
// value when resuming a caller.)
OPCODE(IMPORT_MODULE, 1)

// Import a variable from the most recently imported module. The name of the
// variable to import is at [arg] in the constant table. Pushes the loaded
// variable's value.
OPCODE(IMPORT_VARIABLE, 1)

// This pseudo-instruction indicates the end of the bytecode. It should
// always be preceded by a `CODE_RETURN`, so is never actually executed.
OPCODE(END, 0)
