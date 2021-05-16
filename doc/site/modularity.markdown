^title Modularity

Once you start writing programs that are more than little toys, you quickly run
into two problems:

1. You want to break them down into multiple smaller files to make it easier to
   find your way around them.

2. You want to reuse pieces of them across different programs.

To address those, Wren has a simple module system. A file containing Wren code
defines a *module*. A module can use the code defined in another module by
*importing* it. You can break big programs into smaller modules that you
import, and you can reuse code by having multiple programs share the use of a
single module.

Wren does not have a single global scope. Instead, each module has its own
top-level scope independent of all other modules. This means, for example, that
two modules can define a top-level variable with the same name without causing
a name collision. Each module is, well, modular.

## Importing, briefly

When you run Wren and give it a file name to execute, the contents of that file
define the "main" module that execution starts at. To load and execute other
modules, you use an import statement:

<pre class="snippet">
import "beverages" for Coffee, Tea
</pre>

This finds a module named "beverages" and executes its source code. Then, it
looks up two top-level variables, `Coffee` and `Tea` in *that* module and
creates new variables in *this* module with their values.

This statement can appear anywhere a variable declaration is allowed, even
inside blocks:

<pre class="snippet">
if (thirsty) {
  import "beverages" for Coffee, Tea
}
</pre>

If you need to import a variable under a different name, you can use 
`import "..." for Name as OtherName`. This looks up the top-level variable
`Name` in *that* module, but declares a variable called `OtherName` in *this* module
with its value.

<pre class="snippet">
import "liquids" for Water //Water is now taken
import "beverages" for Coffee, Water as H2O, Tea
// var water = H2O.new()
</pre>

If you want to load a module, but not bind any variables from it, you can omit
the `for` clause:

<pre class="snippet">
import "some_imperative_code"
</pre>

That's the basic idea. Now let's break it down into each of the steps it
performs:

1. Locate the source code for the module.
2. Execute the imported module's code.
3. Bind new variables in the importing module to values defined in the imported
   module.

We'll go through each step:

## Locating a module

The first thing you need to do to import a module is actually *find* the code
for it. The import specifies a *name*&mdash;some arbitrary string that is used
to uniquely identify the module. The embedding application controls how that
string is used to locate a blob of source code.

When the host application creates a new Wren VM, it provides a module loader
function:

<pre class="snippet" data-lang="c">
WrenConfiguration config;
config.loadModuleFn = loadModule;

// Other configuration...

WrenVM* vm = wrenNewVM(&config);
</pre>

That function has this signature:

<pre class="snippet" data-lang="c">
WrenLoadModuleResult WrenLoadModuleFn(WrenVM* vm, const char* name);
</pre>

Whenever a module is imported, the VM calls this and passes it the name of the
module. The embedder is expected to return the source code contents of the
module in a `WrenLoadModuleResult`. When you embed Wren in your app, you can handle
this however you want: reach out to the file system, look inside resources bundled
into your app, whatever.

You can return the source field as `NULL` from this function to indicate that a module
couldn't be found. When you do this, Wren will report it as a runtime error.

### The command-line loader

The [Wren CLI command-line tool](getting-started.html#using-the-wren-cli) has a very simple
lookup process. It appends the module name and ".wren" to the directory where
the main module was loaded and looks for that file. So, let's say you run:

    $ wren code/my_program.wren

And that main module has:

<pre class="snippet">
import "some/module"
</pre>

Then the command-line VM will try to find `/code/some/module.wren`. By
convention, forward slashes should be used as path separators, even on Windows,
to help ensure your scripts are platform-independent. (Forward slashes are a
valid separator on Windows, but backslashes are not valid on other OSes.)

## Executing the module

Once we have the source code for a module, we need to run it. First, the VM
takes the [fiber][] that is executing the `import` statement in the importing
module and pauses it.

[fiber]: concurrency.html

Then it creates a new module object&mdash;a new fresh top-level scope,
basically&mdash;and a new fiber. It executes the new module's code in that
fiber and scope. The module can run whatever imperative code it wants and
define whatever top-level variables it wants.

When the module's code is done being executed and its fiber completes, the
suspended fiber for the importing module is resumed. This suspending and
resuming is recursive. So, if "a" imports "b" which imports "c", both "a" and
"b" will be suspended while "c" is running. When "c" is done, "b" is resumed.
Then, when "b" completes, "a" is resumed.

Think of it like traversing the tree of imports, one node at a time. At any
given point in time, only one module's code is running.

## Binding variables

Once the module is done executing, the last step is to actually *import* some
data from it. Any module can define "top-level" [variables](variables.html).
These are simply variables declared outside of any
[method](classes.html#methods) or [function](functions.html).

These are visible to anything inside the module, but they can also be
*exported* and used by other modules. When Wren executes an import like:

<pre class="snippet">
import "beverages" for Coffee, Tea
</pre>

First it runs the "beverages" module. Then it goes through each of the variable
names in the `for` clause. For each one, it looks for a top-level variable with
that name in the imported module. If a variable with that name can't be found
in the imported module, it's a runtime error.

Otherwise, it gets the current value of the variable and defines a new variable
in the importing module with the same name and value. It's worth noting that
the importing module gets its *own* variable whose value is a snapshot of the
value of the imported variable at the time it was imported. If either module
later assigns to that variable, the other won't see it. It's not a "live"
connection.

In practice, most top-level variables are only assigned once anyway, so this
rarely makes a difference.

## Shared imports

Earlier, I described a program's set of modules as a tree. Of course, it's only
a *tree* of modules if there are no *shared imports*. But consider a program
like:

<pre class="snippet">
// main.wren
import "a"
import "b"

// a.wren
import "shared"

// b.wren
import "shared"

// shared.wren
System.print("Shared!")
</pre>

Here, "a" and "b" both want to use "shared". If "shared" defines some top-level
state, we only want a single copy of that in memory. To handle this, a module's
code is only executed the *first* time it is loaded. After that, importing the
module again just looks up the previously loaded module.

Internally, Wren maintains a map of every module it has previously loaded. When
a module is imported, Wren looks for it in that map first before it calls out
to the embedder for its source.

In other words, in that list of steps above, there's an implicit zeroth step:
"See if we already loaded the module and reuse it if we did". That means the
above program only prints "Shared!" once.

## Cyclic imports

You can even have cycles in your imports, provided you're a bit careful with
them. The loading process, in detail, is:

1. See if we have already created a module with the given name.
2. If so, use it.
3. Otherwise, create a new module with the name and store it in the module
   registry.
4. Create a fiber for it and execute its code.

Note the order of the last two steps. When a module is loaded, it is added to
the registry *before* it is executed. This means if an import for that same
module is reached while the module itself or one of its imports is executing,
it will be found in the registry and the cycle is short-circuited.

For example:

<pre class="snippet">
// main.wren
import "a"

// a.wren
System.print("start a")
import "b"
System.print("end a")

// b.wren
System.print("start b")
import "a"
System.print("end b")
</pre>

This program runs successfully and prints:

    start a
    start b
    end b
    end a

Where you have to be careful is binding variables. Consider:

<pre class="snippet">
// main.wren
import "a"

// a.wren
import "b" for B
var A = "a variable"

// b.wren
import "a" for A
var B = "b variable"
</pre>

The import of "a" in b.wren will fail here. If you trace the execution, you
get:

1. Execute `import "a"` in "main.wren". That suspends "main.wren".
2. Execute `import "b"` in "a.wren". That suspends "a.wren".
3. Execute `import "a"` in "b.wren". Since "a" is already in the module map,
   this does *not* suspend it.

Instead, we look for a variable named `A` in that module. But it hasn't been
defined yet since "a.wren" is still sitting on the `import "b" for B` line
before the declaration. To get this to work, you would need to move the
variable declaration above the import:

<pre class="snippet">
// main.wren
import "a"

// a.wren
var A = "a variable"
import "b" for B

// b.wren
import "a" for A
var B = "b variable"
</pre>

Now when we run it, we get:

1. Execute `import "a"` in "main.wren". That suspends "main.wren".
2. Define `A` in "a.wren".
2. Execute `import "b"` in "a.wren". That suspends "a.wren".
3. Execute `import "a"` in "b.wren". Since "a" is already in the module map,
   this does *not* suspend it. It looks up `A`, which has already been defined,
   and binds it.
4. Define `B` in "b.wren".
5. Complete "b.wren".
6. Look up `B` in "b.wren" and bind it in "a.wren".
7. Resume "a.wren".

This sounds super hairy, but that's because cyclic dependencies are hairy in
general. The key point here is that Wren *can* handle them in the rare cases
where you need them.

## Exiting a module early

Although the `return` statement is normally used to exit from a [method](classes.html#methods) or a [function](functions.html), it can also be used from a module's top-level code to exit the module. For example, if the script consists of a single module, this code would exit the module (and therefore the script) early:

<pre class="snippet">
for (i in 1..2) {
  if (i == 2) return
  System.print(i)  //>  prints 1 but not 2
}
System.print(3)    //>  not reached
</pre>

Although it is not invalid to return a value, it can only be accessed using meta-programming - reading the file contents and compiling them with the Meta.compile method. Otherwise it would simply be discarded.

<br><hr>
<a href="error-handling.html">&larr; Error Handling</a>
