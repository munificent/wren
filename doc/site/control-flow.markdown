^title Control Flow

Control flow is used to determine which chunks of code are executed and how many
times. *Branching* statements and expressions decide whether or not to execute
some code and *looping* ones execute something more than once.

## Truth

All control flow is based on *deciding* whether or not to do something. This
decision depends on some expression's value. We take the entire universe of
possible objects and divide them into two buckets: some we consider "true" and
the rest are "false". If the expression results in a value in the true bucket,
we do one thing. Otherwise, we do something else.

Obviously, the boolean `true` is in the "true" bucket and `false` is in
"false", but what about values of other types? The choice is ultimately
arbitrary, and different languages have different rules. Wren's rules follow
Ruby:

  * The boolean value `false` is false.
  * The null value `null` is false.
  * Everything else is true.

This means `0`, empty strings, and empty collections are all considered "true"
values.

## If statements

The simplest branching statement, `if` lets you conditionally skip a chunk of
code. It looks like this:

<pre class="snippet">
if (ready) System.print("go!")
</pre>

That evaluates the parenthesized expression after `if`. If it's true, then the
statement after the condition is evaluated. Otherwise it is skipped. Instead of
a statement, you can have a [block](syntax.html#blocks):

<pre class="snippet">
if (ready) {
  System.print("getSet")
  System.print("go!")
}
</pre>

You may also provide an `else` branch. It will be executed if the condition is
false:

<pre class="snippet">
if (ready) System.print("go!") else System.print("not ready!")
</pre>

And, of course, it can take a block too:

<pre class="snippet">
if (ready) {
  System.print("go!")
} else {
  System.print("not ready!")
}
</pre>

## Logical operators

Unlike most other [operators][] in Wren which are just a special syntax for
[method calls][], the `&&` and `||` operators are special. This is because they
only conditionally evaluate the right operand&mdash;they short-circuit.

[operators]: method-calls.html#operators
[method calls]: method-calls.html

A `&&` ("logical and") expression evaluates the left-hand argument. If it's
false, it returns that value. Otherwise it evaluates and returns the right-hand
argument.

<pre class="snippet">
System.print(false && 1)  //> false
System.print(1 && 2)      //> 2
</pre>

A `||` ("logical or") expression is reversed. If the left-hand argument is
*true*, it's returned, otherwise the right-hand argument is evaluated and
returned:

<pre class="snippet">
System.print(false || 1)  //> 1
System.print(1 || 2)      //> 1
</pre>

## The conditional operator `?:`

Also known as the "ternary" operator since it takes three arguments, Wren has
the little "if statement in the form of an expression" you know and love from C
and similar languages.

<pre class="snippet">
System.print(1 != 2 ? "math is sane" : "math is not sane!")
</pre>

It takes a condition expression, followed by `?`, followed by a then
expression, a `:`, then an else expression. Just like `if`, it evaluates the
condition. If true, it evaluates and returns the then expression. Otherwise
it does the else expression.

## While statements

It's hard to write a useful program without executing some chunk of code
repeatedly. To do that, you use looping statements. There are two in Wren, and
they should be familiar if you've used other imperative languages.

The simplest, a `while` statement executes a chunk of code as long as a
condition continues to hold. For example:

<pre class="snippet">
// Hailstone sequence.
var n = 27
while (n != 1) {
  if (n % 2 == 0) {
    n = n / 2
  } else {
    n = 3 * n + 1
  }
}
</pre>

This evaluates the expression `n != 1`. If it is true, then it executes the
following body. After that, it loops back to the top, and evaluates the
condition again. It keeps doing this as long as the condition evaluates to
something true.

The condition for a while loop can be any expression, and must be surrounded by
parentheses. The body of the loop is usually a curly block but can also be a
single statement:

<pre class="snippet">
var n = 27
while (n != 1) if (n % 2 == 0) n = n / 2 else n = 3 * n + 1
</pre>

## For statements

While statements are useful when you want to loop indefinitely or according to
some complex condition. But in most cases, you're looping through
a [list](lists.html), a series of numbers, or some other "sequence" object.
That's what `for` is, uh, for. It looks like this:

<pre class="snippet">
for (beatle in ["george", "john", "paul", "ringo"]) {
  System.print(beatle)
}
</pre>

A `for` loop has three components:

1. A *variable name* to bind. In the example, that's `beatle`. Wren will create
   a new variable with that name whose scope is the body of the loop.

2. A *sequence expression*. This determines what you're looping over. It gets
   evaluated *once* before the body of the loop. In this case, it's a list
   literal, but it can be any expression.

3. A *body*. This is a curly block or a single statement. It gets executed once
   for each iteration of the loop.

## Break statements

Sometimes, right in the middle of a loop body, you decide you want to bail out
and stop. To do that, you can use a `break` statement. It's just the `break`
keyword all by itself. That immediately exits out of the nearest enclosing
`while` or `for` loop.

<pre class="snippet">
for (i in [1, 2, 3, 4]) {
  System.print(i)           //> 1
  if (i == 3) break         //> 2
}                           //> 3
</pre>

## Continue statements

During the execution of a loop body, you might decide that you want to skip the 
rest of this iteration and move on to the next one. You can use a `continue` 
statement to do that. It's just the `continue` keyword all by itself. Execution
will immediately jump to the beginning of the next loop iteration (and check the
loop conditions).

<pre class="snippet">
for (i in [1, 2, 3, 4]) {
  if (i == 2) continue      //> 1
  System.print(i)           //> 3
}                           //> 4
</pre>

## Numeric ranges

Lists are one common use for `for` loops, but sometimes you want to walk over a
sequence of numbers, or loop a number of times. For that, you can create a
[range](values.html#ranges), like so:

<pre class="snippet">
for (i in 1..100) {
  System.print(i)
}
</pre>

This loops over the numbers from 1 to 100, including 100 itself. If you want to
leave off the last value, use three dots instead of two:

<pre class="snippet">
for (i in 1...100) {
  System.print(i)
}
</pre>

This looks like some special "range" syntax in the `for` loop, but it's actually
just a pair of operators. The `..` and `...` syntax are infix "range" operators.
Like [other operators][operators], they are special syntax for a regular method
call. The number type implements them and returns a [range object][] that knows
how to iterate over a series of numbers.

[range object]: values.html#ranges

## The iterator protocol

Lists and ranges cover the two most common kinds of loops, but you should also
be able to define your own sequences. To enable that, the semantics of `for`
are defined in terms of an "iterator protocol". The loop itself doesn't know
anything about lists or ranges, it just knows how to call two particular
methods on the object that resulted from evaluating the sequence expression.

When you write a loop like this:

<pre class="snippet">
for (i in 1..100) {
  System.print(i)
}
</pre>

Wren sees it something like this:

<pre class="snippet">
var iter_ = null
var seq_ = 1..100
while (iter_ = seq_.iterate(iter_)) {
  var i = seq_.iteratorValue(iter_)
  System.print(i)
}
</pre>

First, Wren evaluates the sequence expression and stores it in a hidden
variable (written `seq_` in the example but in reality it doesn't have a name
you can use). It also creates a hidden "iterator" variable and initializes it
to `null`.

Each iteration, it calls `iterate()` on the sequence, passing in the current
iterator value. (In the first iteration, it passes in `null`.) The sequence's
job is to take that iterator and advance it to the next element in the
sequence. (Or, in the case where the iterator is `null`, to advance it to the
*first* element). It then returns either the new iterator, or `false` to
indicate that there are no more elements.

If a falsy value (`false` or `null`) is returned, Wren exits out of the loop
and we're done. If anything else is returned, that means that we have advanced
to a new valid element. To get that, Wren then calls `iteratorValue()` on the
sequence and passes in the iterator value that it just got from calling
`iterate()`. The sequence uses that to look up and return the appropriate element.

The built-in [List](lists.html) and [Range](values.html#ranges) types implement
`iterate()` and `iteratorValue()` to walk over their respective sequences. You
can implement the same methods in your classes to make your own types iterable.

<br><hr>
<a class="right" href="variables.html">Variables &rarr;</a>
<a href="method-calls.html">&larr; Method Calls</a>
