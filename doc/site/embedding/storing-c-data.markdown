^title Storing C Data

An embedded language often needs to work with native data. You may want a
pointer to some memory managed in the C heap, or maybe you want to store a chunk
of data more efficiently than Wren's dynamism allows. You may want a Wren object
that represents a native resource like a file handle or database connection.

For those cases, you can define a **foreign class**, a chimera whose state is
half Wren and half C. It is a real Wren class with a name, constructor, and
methods. You can define methods on it written in Wren, or [foreign methods][]
written in C. It produces real Wren objects that you can pass around, do `is`
checks on, etc. But it also wraps a blob of raw memory that is opaque to Wren
but accessible from C.

[foreign methods]: calling-c-from-wren.html

## Defining a Foreign Class

You define one like so:

<pre class="snippet">
foreign class Point {
  // ...
}
</pre>

The `foreign` keyword tells Wren to loop in the host application when it
constructs instances of the class. The host tells Wren how many bytes of extra
memory the foreign instance should contain and in return, Wren gives the host
the opportunity to initialize that data.

To talk to the host app, Wren needs a C function it can call when it constructs
an instance of the foreign class. This function is found through a binding
process similar to [how foreign methods are bound][bind]. When you [configure
the VM][], you set the `bindForeignClassFn` field in WrenConfiguration to point
to a C callback you define. Its signature must be:

[bind]: calling-c-from-wren.html#binding-foreign-methods
[configure the vm]: configuring-the-vm.html

<pre class="snippet" data-lang="c">
WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className);
</pre>

Wren invokes this callback once when a foreign class declaration is executed.
Wren passes in the name of the module containing the foreign class, and the name
of the class being declared. The host's responsibility is to return one of these
structs:

<pre class="snippet" data-lang="c">
typedef struct
{
  WrenForeignMethodFn allocate;
  WrenFinalizerFn finalize;
} WrenForeignClassMethods;
</pre>

It's a pair of function pointers. The first, `allocate`, is called by Wren
whenever an instance of the foreign class is created. (We'll get to the optional
`finalize` callback later.) The allocation callback has the same signature as a
foreign method:

<pre class="snippet" data-lang="c">
void allocate(WrenVM* vm);
</pre>

## Initializing an Instance

When you create an instance of a foreign class by calling one its
[constructors][], Wren invokes the `allocate` callback you gave it when binding
the foreign class. Your primary responsibility in that callback is to tell Wren
how many bytes of raw memory you need. You do that by calling:

[constructors]: ../classes.html#constructors

<pre class="snippet" data-lang="c">
void* wrenSetSlotNewForeign(WrenVM* vm,
    int slot, int classSlot, size_t size);
</pre>

Like other [slot manipulation functions][slot], it both reads from and writes to
the slot array. It has a few parameters to make it more general purpose since it
can also be used in other foreign methods:

[slot]: slots-and-handles.html

* The `slot` parameter is the destination slot where the new foreign object
  should be placed. When you're calling this in a foreign class's allocate
  callback, this should be 0.

* The `classSlot` parameter is the slot where the foreign class being
  constructed can be found. When the VM calls an allocate callback for a
  foreign class, the class itself is already in slot 0, so you'll pass 0 for
  this too.

* Finally, the `size` parameter is the interesting one. Here, you pass in the
  number of extra raw bytes of data you want the foreign instance to store.
  This is the memory you get to play with from C.

So, for example, if you wanted to create a foreign instance that contains eight
bytes of C data, you'd call:

<pre class="snippet" data-lang="c">
void* data = wrenSetSlotNewForeign(vm, 0, 0, 8);
</pre>

The value returned by `wrenSetSlotNewForeign()` is the raw pointer to the
requested bytes. You can cast that to whatever C type makes sense (as long as it
fits within the requested number of bytes) and initialize it as you see fit.

Any parameters passed to the constructor are also available in subsequent slots
in the slot array. That way you can initialize the foreign data based on values
passed to the constructor from Wren.

After the allocate callback returns, the class's constructor in Wren is run and
execution proceeds like normal. From here on out, within Wren, it appears you
have a normal instance of a class. It just happens to have some extra bytes
hiding inside it that can be accessed from foreign methods.

## Accessing Foreign Data

Typically, the way you make use of the data stored in an instance of a foreign
class is through other foreign methods. Those are usually defined on the same
foreign class, but can be defined on other classes as well. Wren doesn't care.

Once you have a foreign instance in a slot, you can access the raw bytes it
stores by calling:

<pre class="snippet" data-lang="c">
void* wrenGetSlotForeign(WrenVM* vm, int slot);
</pre>

You pass in the slot index containing the foreign object and it gives you back a
pointer to the raw memory the object wraps. As usual, the C API doesn't do any
type or bounds checking, so it's on you to make sure the object in that slot
actually *is* an instance of a foreign class and contains as much memory as you
access.

Given that void pointer, you can now freely read and modify the data it points
to. They're your bits, Wren just holds them for you.

## Freeing Resources

If your foreign instances are just holding memory and you're OK with Wren's
garbage collector managing the lifetime of that memory, then you're done. Wren
will keep the bytes around as long as there is still a reference to them. When
the instance is no longer reachable, eventually the garbage collector will do
its thing and free the memory.

But, often, your foreign data refers to some resource whose lifetime needs to
be explicitly managed. For example, if you have a foreign object that wraps an
open file handle, you need to ensure that handle doesn't get left open when the
GC frees the foreign instance.

Of course, you can (and usually should) add a method on your foreign class, like
`close()` so the user can explicitly release the resource managed by the object.
But if they forget to do that and the object is no longer reachable, you want to
make sure the resource isn't leaked.

To that end, you can also provide a *finalizer* function when binding the
foreign class. That's the other callback in the WrenForeignClassMethods struct.
If you provide that callback, then Wren will invoke it when an instance of your
foreign class is about to be freed by the garbage collector. This gives you one
last chance to clean up the object's resources.

Because this is called during the middle of a garbage collection, you do not
have unfettered access to the VM. It's not like a normal foreign method where
you can monkey around with slots and other stuff. Doing that while the GC is
running could leave Wren in a weird state.

Instead, the finalize callback's signature is only:

<pre class="snippet" data-lang="c">
void finalize(void* data);
</pre>

Wren gives you the pointer to your foreign function's memory, and that's it. The
*only* thing you should do inside a finalizer is release any external resources
referenced by that memory.

## A Full Example

That's a lot to take in, so let's walk through a full example of a foreign class
with a finalizer and a couple of methods. We'll do a File class that wraps the
C standard file API.

In Wren, the class we want looks like this:

<pre class="snippet">
foreign class File {
  construct create(path) {}

  foreign write(text)
  foreign close()
}
</pre>

So you can create a new file given a path. Once you have one, you can write to
it and then explicitly close it if you want. We also need to make sure the file
gets closed if the user forgets to and the GC cleans up the object.

### Setting up the VM

Over in the host, first we'll set up the VM:

<pre class="snippet" data-lang="c">
#include "wren.h"

int main(int argc, const char* argv[])
{
  WrenConfiguration config;
  wrenInitConfiguration(&config);

  config.bindForeignClassFn = bindForeignClass;
  config.bindForeignMethodFn = bindForeignMethod;

  WrenVM* vm = wrenNewVM(&config);
  wrenInterpret(vm, "my_module", "some code...");

  return 0;
}
</pre>

### Binding the foreign class

We give the VM two callbacks. The first is for wiring up the foreign class
itself:

<pre class="snippet" data-lang="c">
WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className)
{
  WrenForeignClassMethods methods;

  if (strcmp(className, "File") == 0)
  {
    methods.allocate = fileAllocate;
    methods.finalize = fileFinalize;
  }
  else
  {
    // Unknown class.
    methods.allocate = NULL;
    methods.finalize = NULL;
  }

  return methods;
}
</pre>

When our binding callback is invoked for the File class, we return the allocate
and finalize functions the VM should call. Allocation looks like:

<pre class="snippet" data-lang="c">
#include &lt;stdio.h>
#include "wren.h"

void fileAllocate(WrenVM* vm)
{
  FILE** file = (FILE**)wrenSetSlotNewForeign(vm,
      0, 0, sizeof(FILE*));
  const char* path = wrenGetSlotString(vm, 1);
  *file = fopen(path, "w");
}
</pre>

First we create the instance by calling `wrenSetSlotNewForeign()`. We tell it to
add enough extra bytes to store a `FILE*` in it, which is C's representation of
a file handle. We're given back a pointer to the bytes. Since the file handle is
itself a pointer, we end up with a double indirection, hence the `FILE**`. In
most cases, you'll just have a single `*`.

We also pull the file path from the slot array. Then we tell C to create a new
file at that path. That gives us back a new file handle -- a `FILE*` -- and we
store that back into the foreign instance using `*file`. Now we have a foreign
object that wraps an open file handle.

The finalizer simply casts the foreign instance's data back to the proper type
and closes the file:

<pre class="snippet" data-lang="c">
void fileFinalize(void* data)
{
  closeFile((FILE**) data);
}
</pre>

It uses this little utility function:

<pre class="snippet" data-lang="c">
static void closeFile(FILE** file)
{
  // Already closed.
  if (*file == NULL) return;

  fclose(*file);
  *file = NULL;
}
</pre>

This closes the file (if it's not already closed) and also nulls out the file
handle so that we don't try to use the file after it's been closed.

### Binding the foreign methods

That's the foreign *class* part. Now we have a couple of foreign *methods* to
handle. The host tells the VM how to find them by giving Wren a pointer to this
function:

<pre class="snippet" data-lang="c">
WrenBindForeignMethodResult bindForeignMethod(WrenVM* vm, const char* module,
    const char* className, bool isStatic, const char* signature)
{
  WrenBindForeignMethodResult result = {};
  result.executeFn = NULL;  // no function unless we find a match below.

  if (strcmp(className, "File") == 0)
  {
    if (!isStatic && strcmp(signature, "write(_)") == 0)
    {
      result.executeFn = fileWrite;
    }

    if (!isStatic && strcmp(signature, "close()") == 0)
    {
      result.executeFn = fileClose;
    }
  }

  return result;
}
</pre>

When Wren calls this, we look at the class and method name to figure out which
method it's binding, and then return a pointer to the appropriate function.
That pointer is wrapped in a `WrenBindForeignMethodResult`:

<pre class="snippet" data-lang="c">
typedef struct WrenBindForeignMethodResult {
  WrenForeignMethodFn executeFn;
  void* userData;
} WrenBindForeignMethodResult;
</pre>

Along with the function, we can give Wren a `userData` value it will pass to
the foreign method.  We don't do that here, but the space is available if we
need it.

The foreign method for writing to the file is:

<pre class="snippet" data-lang="c">
void fileWrite(WrenVM* vm, void* userData)
{
  FILE** file = (FILE**)wrenGetSlotForeign(vm, 0);

  // Make sure the file is still open.
  if (*file == NULL)
  {
    wrenSetSlotString(vm, 0, "Cannot write to a closed file.");
    wrenAbortFiber(vm, 0);
    return;
  }

  const char* text = wrenGetSlotString(vm, 1);
  fwrite(text, sizeof(char), strlen(text), *file);
}
</pre>

We use `wrenGetSlotForeign()` to pull the foreign data out of the slot array.
Since this method is called on the file itself, the foreign object is in slot
zero. We take the resulting pointer and cast it to a pointer of the proper type.
Again, because our foreign data is *itself* a pointer, we get a pointer to a
pointer.

We do a little sanity checking to make sure the user isn't writing to a file
they already closed. If not, we call `fwrite()` to write to the file.

The other method is `close()` to let them explicitly close the file:

<pre class="snippet" data-lang="c">
void fileClose(WrenVM* vm, void *userData)
{
  FILE** file = (FILE**)wrenGetSlotForeign(vm, 0);
  closeFile(file);
}
</pre>

It uses the same helper we defined above. And that's it, a complete foreign
class with a finalizer and a couple of foreign methods. In Wren, you can use it
like so:

<pre class="snippet">
var file = File.create("some/path.txt")
file.write("some text")
file.close()
</pre>

Pretty neat, right? The resulting class looks and feels like a normal Wren
class, but it has the functionality and much of the performance of native C
code.

<a class="right" href="configuring-the-vm.html">Configuring the VM &rarr;</a>
<a href="calling-c-from-wren.html">&larr; Calling C from Wren</a>
