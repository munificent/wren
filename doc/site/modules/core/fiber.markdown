^title Fiber Class

A lightweight coroutine. [Here][fibers] is a gentle introduction.

[fibers]: ../../concurrency.html

## Static Methods

### Fiber.**abort**(value)

Raises a runtime error with the provided value.

<pre class="snippet">
Fiber.abort("Something bad happened.")
Fiber.abort(CustomError.new())
Fiber.abort(32)
</pre>

If the value is `null`, does nothing.

### Fiber.**current**

The currently executing fiber.

### Fiber.**new**(function)

Creates a new fiber that executes `function` in a separate coroutine when the
fiber is run. Does not immediately start running the fiber.

<pre class="snippet">
var fiber = Fiber.new {
  System.print("I won't get printed")
}
</pre>

`function` must be a function (an actual [Fn][] instance, not just an object
with a `call()` method) and it may only take zero or one parameters.

[fn]: fn.html

### Fiber.**suspend**()

Pauses the current fiber, and stops the interpreter. Control returns to the
host application.

Typically, you store a reference to the fiber using `Fiber.current` before
calling this. The fiber can be resumed later by calling or transferring to that
reference. If there are no references to it, it is eventually garbage collected.

Much like `yield()`, returns the value passed to `call()` or `transfer()` when
the fiber is resumed.

### Fiber.**yield**()

Pauses the current fiber and transfers control to the parent fiber. "Parent"
here means the last fiber that was started using `call` and not `transfer`.

<pre class="snippet">
var fiber = Fiber.new {
  System.print("Before yield")
  Fiber.yield()
  System.print("After yield")
}

fiber.call()                //> Before yield
System.print("After call")  //> After call
fiber.call()                //> After yield
</pre>

When resumed, the parent fiber's `call()` method returns `null`.

If a yielded fiber is resumed by calling `call()` or `transfer()` with an
argument, `yield()` returns that value.

<pre class="snippet">
var fiber = Fiber.new {
  System.print(Fiber.yield()) //> value
}

fiber.call()        // Run until the first yield.
fiber.call("value") // Resume the fiber.
</pre>

If it was resumed by calling `call()` or `transfer()` with no argument, it
returns `null`.

If there is no parent fiber to return to, this exits the interpreter. This can
be useful to pause execution until the host application wants to resume it
later.

<pre class="snippet">
Fiber.yield()
System.print("this does not get reached")
</pre>

### Fiber.**yield**(value)

Similar to `Fiber.yield` but provides a value to return to the parent fiber's
`call`.

<pre class="snippet">
var fiber = Fiber.new {
  Fiber.yield("value")
}

System.print(fiber.call()) //> value
</pre>

## Methods

### **call**()

Starts or resumes the fiber if it is in a paused state. Equivalent to:

<pre class="snippet">
fiber.call(null)
</pre>

### **call**(value)

Start or resumes the fiber if it is in a paused state. If the fiber is being
started for the first time, and its function takes a parameter, `value` is
passed to it.

<pre class="snippet">
var fiber = Fiber.new {|param|
  System.print(param) //> begin
}

fiber.call("begin")
</pre>

If the fiber is being resumed, `value` becomes the returned value of the fiber's
call to `yield`.

<pre class="snippet">
var fiber = Fiber.new {
  System.print(Fiber.yield()) //> resume
}

fiber.call()
fiber.call("resume")
</pre>

### **error**

The error value that was passed when aborting the fiber, or `null` if the
fiber has not been aborted. Wren internal error values will always be
Strings.

<pre class="snippet">
var fiber = Fiber.new {
  123.badMethod
}

fiber.try()
System.print(fiber.error) //> Num does not implement method 'badMethod'.
</pre>

### **isDone**

Whether the fiber's main function has completed and the fiber can no longer be
run. This returns `false` if the fiber is currently running or has yielded.

### **try**()
Tries to run the fiber. If a runtime error occurs
in the called fiber, the error is captured and is returned as a string.

<pre class="snippet">
var fiber = Fiber.new {
  123.badMethod
}

var error = fiber.try()
System.print("Caught error: " + error)
</pre>

If the called fiber raises an error, it can no longer be used.

### **try**(value)
Tries to run the fiber. If a runtime error occurs
in the called fiber, the error is captured and is returned as a string.
If the fiber is being
started for the first time, and its function takes a parameter, `value` is
passed to it.

<pre class="snippet">
var fiber = Fiber.new {|value|
  value.badMethod
}

var error = fiber.try("just a string")
System.print("Caught error: " + error)
</pre>

If the called fiber raises an error, it can no longer be used.

### **transfer**()

Pauses execution of the current running fiber, and transfers control to this fiber.

[Read more][transfers] about the difference between `call` and `transfer`.
Unlike `call`, `transfer` doesn't track the origin of the transfer.

[transfers]: ../../concurrency.html#transferring-control

<pre class="snippet">
// keep hold of the fiber we start in
var main = Fiber.current

// create a new fiber, note it doesn't execute yet!
var fiber = Fiber.new {
  System.print("inside 'fiber'") //> #2: from #1
  main.transfer()                //> #3: go back to 'main'
}

fiber.transfer()      //> #1: print "inside 'fiber'" via #2
                      //> this fiber is now paused by #1

System.print("main")  //> #4: prints "main", unpaused by #3
</pre>

### **transfer**(value)

Pauses execution of the current running fiber, and transfers control to this fiber.

Similar to `transfer`, but a value can be passed between the fibers.

<pre class="snippet">
// keep hold of the fiber we start in
var main = Fiber.current

// create a new fiber, note it doesn't execute yet
// also note that we're accepting a 'value' parameter
var fiber = Fiber.new {|value|
  System.print("in 'fiber' = %(value)")   //> #2: in 'fiber' = 5
  var result = main.transfer("hello?")    //> #3: send to 'message'
  System.print("end 'fiber' = %(result)") //> #6: end 'fiber' = 32
}

var message = fiber.transfer(5)   //> #1: send to 'value'
System.print("... %(message)")    //> #4: ... hello?
fiber.transfer(32)                //> #5: send to 'result'
</pre>

### **transferError**(error)

Transfer to this fiber, but set this fiber into an error state.
The `fiber.error` value will be populated with the value in `error`.

<pre class="snippet">
var A = Fiber.new {
  System.print("transferred to A")     //> #4
  B.transferError("error!")            //> #5
}

var B = Fiber.new {
  System.print("started B")            //> #2
  A.transfer()                         //> #3
  System.print("should not get here")
}

B.try()                   //> #1
System.print(B.error)     //> #6: prints "error!" from #5

// B fiber can no longer be used

B.call()                  //> #7: Cannot call an aborted fiber.
</pre>
