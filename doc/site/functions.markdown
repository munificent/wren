^title Functions

Like many languages today, functions in Wren are little bundles of code 
you can store in a variable, or pass as an argument to a method. 

Notice there's a difference between _function_ and _method_.

Since Wren is object-oriented, most of your code will live in methods on
classes, but free-floating functions are still eminently handy. 

Functions are objects like everything else in Wren, instances of the `Fn`
class.

## Creating a function

To create a function, we call `Fn.new`, which takes a block to execute.
To call the function, we use `.call()` on the function instance.

<pre class="snippet">
var sayHello = Fn.new { System.print("hello") }

sayHello.call() //> hello
</pre>

Note that we'll see a shorthand syntax for creating a function below.

## Function parameters

Of course, functions aren't very useful if you can't pass values to them. The
function above takes no arguments. To change that, you can provide a parameter
list surrounded by `|` immediately after the opening brace of the body.

To pass arguments to the function, pass them to the `call` method:

<pre class="snippet">
var sayMessage = Fn.new {|recipient, message|
  System.print("message for %(recipient): %(message)")
}

sayMessage.call("Bob", "Good day!")
</pre>

It's an error to call a function with fewer arguments than its parameter list
expects. If you pass too *many* arguments, the extras are ignored.

## Returning values

The body of a function is a [block](syntax.html#blocks). If it is a single
expression&mdash;more precisely if there is no newline after the `{` or
parameter list&mdash;then the function implicitly returns the value of the
expression.

Otherwise, the body returns `null` by default. You can explicitly return a
value using a `return` statement. In other words, these two functions do the
same thing:

<pre class="snippet">
Fn.new { "return value" }

Fn.new {
  return "return value"
}
</pre>

The return value is handed back to you when using `call`:

<pre class="snippet">
var fn = Fn.new { "some value" }
var result = fn.call()
System.print(result) //> some value
</pre>

## Closures

As you expect, functions are closures&mdash;they can access variables defined
outside of their scope. They will hold onto closed-over variables even after
leaving the scope where the function is defined:

<pre class="snippet">
class Counter {
  static create() {
    var i = 0
    return Fn.new { i = i + 1 }
  }
}
</pre>

Here, the `create` method returns the function created on its second line. That
function references a variable `i` declared outside of the function. Even after
the function is returned from `create`, it is still able to read and assign
to`i`:

<pre class="snippet">
var counter = Counter.create()
System.print(counter.call()) //> 1
System.print(counter.call()) //> 2
System.print(counter.call()) //> 3
</pre>

Variables outside the scope of the closure are captured by reference and not by
copy, so writing to a captured variable will mutate it in the outer scope as 
well. If you're familiar with C++, you can think of it like using [&] when 
creating a lambda.

## Callable classes

Because `Fn` is a class, and responds to `call()`, any class can respond to 
`call()` and be used in place of a function. This is particularly handy when 
the function is passed to a method to be called, like a callback or event.

<pre class="snippet">
class Callable {
  construct new() {}
  call(name, version) {
    System.print("called %(name) with version %(version)")
  }
}

var fn = Callable.new()
fn.call("wren", "0.4.0")
</pre>

## Block arguments

Very frequently, functions are passed to methods to be called. There are 
countless examples of this in Wren, like [list](lists.html) can be filtered
using a method `where` which accepts a function:

<pre class="snippet">
var list = [1, 2, 3, 4, 5]
var filtered = list.where(Fn.new {|value| value > 3 }) 
System.print(filtered.toList) //> [4, 5]
</pre>

This syntax is a bit less fun to read and write, so Wren implements the 
_block argument_ concept. When a function is being passed to a method, 
and is the last argument to the method, it can use a shorter syntax: 
_just the block part_.

Let's use a block argument for `list.where`, it's the last (only) argument:

<pre class="snippet">
var list = [1, 2, 3, 4, 5]
var filtered = list.where {|value| value > 3 } 
System.print(filtered.toList) //> [4, 5]
</pre>

We've seen this before in a previous page using `map` and `where`:

<pre class="snippet">
numbers.map {|n| n * 2 }.where {|n| n < 100 }
</pre>

## Block argument example

Let's look at a complete example, so we can see both ends.

Here's a fictional class for something that will call a function
when a click event is sent to it. It allows us to pass just a 
function and assume the left mouse button, or to pass a button and a function.

<pre class="snippet">
class Clickable {
  construct new() {
    _fn = null
    _button = 0
  }
  
  onClick(fn) {
    _fn = fn
  }

  onClick(button, fn) {
    _button = button
    _fn = fn
  }

  fireEvent(button) {
    if(_fn && button == _button) {
      _fn.call(button)
    }
  }
}
</pre>

Now that we've got the clickable class, let's use it.
We'll start by using the method that accepts just a function
because we're fine with it just being the default left mouse button.

<pre class="snippet">
var link = Clickable.new()

link.onClick {|button|
  System.print("I was clicked by button %(button)")
}

// send a left mouse click
// normally this would happen from elsewhere

link.fireEvent(0)  //> I was clicked by button 0
</pre>

Now let's try with the extra button argument:

<pre class="snippet">
var contextMenu = Clickable.new()

contextMenu.onClick(1) {|button|
  System.print("I was right-clicked")
}

link.fireEvent(0)  //> (nothing happened)
link.fireEvent(1)  //> I was right-clicked
</pre>

Notice that we still pass the other arguments normally, 
it's only the last argument that is special.

**Just a regular function**   

Block arguments are purely syntax sugar for creating a function and passing it
in one little blob of syntax. These two are equivalent:

<pre class="snippet">
onClick(Fn.new { System.print("clicked") })
onClick { System.print("clicked") }
</pre>

And this is just as valid:

<pre class="snippet">
var onEvent = Fn.new {|button|
  System.print("clicked by button %(button)")
}

onClick(onEvent)
onClick(1, onEvent)
</pre>

**Fn.new**   
As you may have noticed by now, `Fn` accepts a block argument for the `Fn.new`.
All the constructor does is return that argument right back to you!


<br><hr>
<a class="right" href="classes.html">Classes &rarr;</a>
<a href="variables.html">&larr; Variables</a>
