^title Meta Class

This class contains static methods to list a module's top-level variables and to compile Wren expressions and source code into closures (i.e. [functions](functions.html)) at runtime.

It must be imported from the [meta](meta.html) module:

<pre class="snippet">
    import "meta" for Meta
</pre>

## Static Methods

### Meta.**getModuleVariables**(module)

Returns a list of all module level variables defined or visible in `module`.

This includes any variables explicitly imported from other modules or implicitly imported from the built-in modules. For example if we create this module:

<pre class="snippet">
/* module.wren */
var M = 1
</pre>

and then import it into this module:

<pre class="snippet">
/* get_mod_vars.wren */
import "meta" for Meta
import "./module" for M

var v = 42

var f = Fn.new {
  var g = 2
}

class C {}

System.print(Meta.getModuleVariables("./get_mod_vars"))

var w = 43 // still returned even though defined later
</pre>

the output when the latter module is run is:

<pre class="snippet">
[Object, Class, Object metaclass, Bool, Fiber, Fn, Null, Num, Sequence, MapSequence, SkipSequence, TakeSequence, WhereSequence, List, String, StringByteSequence, StringCodePointSequence, Map, MapKeySequence, MapValueSequence, MapEntry, Range, System, Meta, M, v, f, C, w]
</pre>

Notice that `g` is not included in this list as it is a local variable rather than a module variable.

It is a runtime error if `module` is not a string or cannot be found.

### Meta.**eval**(source)

Compiles Wren source code into a closure and then executes the closure automatically.

It is a runtime error if `source` is not a string.

It is also an error if the source code cannot be compiled though the compilation errors themselves are not printed.

For example:

<pre class="snippet">
import "meta" for Meta

var a = 2
var b = 3
var source = """
  var c = a * b
  System.print(c)
"""
Meta.eval(source)  //> 6
</pre>

### Meta.**compileExpression**(expression)

Compiles a Wren expression into a closure and then returns the closure. It does not execute it.

The closure returns the value of the expression.

It is a runtime error if `expression` is not a string.

Prints any compilation errors - in which event the closure will be null - but does not throw an error.

For example:

<pre class="snippet">
import "meta" for Meta

var d = 4
var e = 5
var expression = "d * e"
var closure = Meta.compileExpression(expression)
System.print(closure.call()) //> 20
</pre>

### Meta.**compile**(source)

Compiles Wren source code into a closure and then returns the closure. It does not execute it.

It is a runtime error if `source` is not a string.

Prints any compilation errors - in which event the closure will be null - but does not throw an error.

For example:

<pre class="snippet">
import "meta" for Meta

/* Enum creates an enum with any number of read-only static members.
   Members are assigned in order an initial integer value (often 0), incremented by 1 each time.
   The enum has:
   1. static property getters for each member,
   2. a static 'startsFrom' property, and
   3. a static 'members' property which returns a list of its members as strings.
*/
class Enum {
  // Creates a class for the Enum (with an underscore after the name to avoid duplicate definition)
  // and returns a reference to it.
  static create(name, members, startsFrom) {
    if (name.type != String || name == "") Fiber.abort("Name must be a non-empty string.")
    if (members.isEmpty) Fiber.abort("An enum must have at least one member.")
    if (startsFrom.type != Num || !startsFrom.isInteger) {
      Fiber.abort("Must start from an integer.")
    }
    name = name +  "_"
    var s = "class %(name) {\n"
    for (i in 0...members.count) {
      var m = members[i]
      s = s + "  static %(m) { %(i + startsFrom) }\n"
    }
    var mems = members.map { |m| "\"%(m)\"" }.join(", ")
    s = s + "  static startsFrom { %(startsFrom) }\n"
    s = s + "  static members { [%(mems)] }\n}\n"
    s = s + "return %(name)"
    return Meta.compile(s).call()
  }
}

var Fruits = Enum.create("Fruits", ["orange", "apple", "banana", "lemon"], 0)
System.print(Fruits.banana)     //> 2
System.print(Fruits.startsFrom) //> 0
System.print(Fruits.members)    //> [orange, apple, banana, lemon]
</pre>
