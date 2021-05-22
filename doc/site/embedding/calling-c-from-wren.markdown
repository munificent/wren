^title Calling C from Wren

When we are ensconced within the world of Wren, the external C world is
"foreign" to us. There are two reasons we might want to bring some foreign
flavor into our VM:

* We want to execute code written in C.
* We want to store raw C data.

Since Wren is object-oriented, behavior lives in methods, so for the former we
have **foreign methods**. Likewise, data lives in objects, so for the latter, we
define **foreign classes**. This page is about the first, foreign methods. The
[next page][] covers foreign classes.

[next page]: /embedding/storing-c-data.html

A foreign method looks to Wren like a regular method. It is defined on a Wren
class, it has a name and signature, and calls to it are dynamically dispatched.
The only difference is that the *body* of the method is written in C.

A foreign method is declared in Wren like so:

<pre class="snippet">
class Math {
  foreign static add(a, b)
}
</pre>

The `foreign` keyword tells Wren that the method `add()` is declared on `Math`,
but implemented in C. Both static and instance methods can be foreign.

## Binding Foreign Methods

When you call a foreign method, Wren needs to figure out which C function to
execute. This process is called *binding*. Binding is performed on-demand by the
VM. When a class that declares a foreign method is executed -- when the `class`
statement itself is evaluated -- the VM asks the host application for the C
function that should be used for the foreign method.

It does this through the `bindForeignMethodFn` callback you give it when you
first [configure the VM][config]. This callback isn't the foreign method itself.
It's the binding function your app uses to *look up* foreign methods.

[config]: configuring-the-vm.html

Its signature is:

<pre class="snippet" data-lang="c">
WrenBindForeignMethodResult bindForeignMethodFn(
    WrenVM* vm,
    const char* module,
    const char* className,
    bool isStatic,
    const char* signature);
</pre>

Every time a foreign method is first declared, the VM invokes this callback. It
passes in the module containing the class declaration, the name of the class
containing the method, the method's signature, and whether or not it's a static
method. In the above example, it would pass something like:

<pre class="snippet" data-lang="c">
bindForeignMethodFn(vm, "main", "Math", true, "add(_,_)");
</pre>

When you configure the VM, you give it a C callback that looks up the
appropriate function for the given foreign method and returns a pointer to it.
Something like:

<pre class="snippet" data-lang="c">
WrenBindForeignMethodResult bindForeignMethod(
    WrenVM* vm,
    const char* module,
    const char* className,
    bool isStatic,
    const char* signature)
{
  WrenBindForeignMethodResult result = {};
  result.executeFn = NULL;  // no function unless we find a match below.

  if (strcmp(module, "main") == 0)
  {
    if (strcmp(className, "Math") == 0)
    {
      if (isStatic && strcmp(signature, "add(_,_)") == 0)
      {
        result.executeFn = mathAdd; // C function for Math.add(_,_).
      }
      // Other foreign methods on Math...
    }
    // Other classes in main...
  }
  // Other modules...
  return result;
}
</pre>

This implementation is pretty tedious, but you get the idea. Feel free to do
something more clever here in your host application.

The important part is that it returns a pointer to a C function to use for that
foreign method. Wren does this binding step *once* when the class definition is
first executed. It then keeps the function pointer you return and associates it
with that method. This way, *calls* to the foreign method are fast.

The `WrenBindForeignMethodResult` structure also has a `userData` member.
We can provide any value we want, and Wren will give us that value back
when calling our foreign-method function.

## Implementing a Foreign Method

All C functions for foreign methods have the same signature:

<pre class="snippet" data-lang="c">
void foreignMethod(WrenVM* vm, void *userData);
</pre>

Arguments passed from Wren are not passed as C arguments, and the method's
return value is not a C return value. Instead -- you guessed it -- we go through
the [slot array][].

[slot array]: /embedding/slots-and-handles.html

When a foreign method is called from Wren, the VM sets up the slot array with
the receiver and arguments to the call. As in calling Wren from C, the receiver
object is in slot zero, and arguments are in consecutive slots after that.

You use the slot API to read those arguments, and then perform whatever work you
want to in C. If you want the foreign method to return a value, place it in slot
zero. Like so:

<pre class="snippet" data-lang="c">
void mathAdd(WrenVM* vm, void *userData)
{
  double a = wrenGetSlotDouble(vm, 1);
  double b = wrenGetSlotDouble(vm, 2);
  wrenSetSlotDouble(vm, 0, a + b);
}
</pre>

While your foreign method is executing, the VM is completely suspended. No other
fibers run until your foreign method returns. You should *not* try to resume the
VM from within a foreign method by calling `wrenCall()` or `wrenInterpret()`.
The VM is not re-entrant.

This covers foreign behavior, but what about foreign *state*? For that, we need
a foreign *class*...

<a class="right" href="storing-c-data.html">Storing C Data &rarr;</a>
<a href="calling-wren-from-c.html">&larr; Calling Wren from C</a>
