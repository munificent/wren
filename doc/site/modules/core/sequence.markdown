^title Sequence Class

An abstract base class for any iterable object. Any class that implements the
core [iterator protocol][] can extend this to get a number of helpful methods.

[iterator protocol]: ../../control-flow.html#the-iterator-protocol

## Methods

### **all**(predicate)

Tests whether all the elements in the sequence pass the `predicate`.

Iterates over the sequence, passing each element to the function `predicate`.
If it returns something [false](../control-flow.html#truth), stops iterating
and returns the value. Otherwise, returns `true`.

<pre class="snippet">
System.print([1, 2, 3].all {|n| n > 2}) //> false
System.print([1, 2, 3].all {|n| n < 4}) //> true
</pre>

### **any**(predicate)

Tests whether any element in the sequence passes the `predicate`.

Iterates over the sequence, passing each element to the function `predicate`.
If it returns something [true][], stops iterating and
returns that value. Otherwise, returns `false`.

[true]: ../../control-flow.html#truth

<pre class="snippet">
System.print([1, 2, 3].any {|n| n < 1}) //> false
System.print([1, 2, 3].any {|n| n > 2}) //> true
</pre>

### **contains**(element)

Returns whether the sequence contains any element equal to the given element.

### **count**

The number of elements in the sequence.

Unless a more efficient override is available, this will iterate over the
sequence in order to determine how many elements it contains.

### **count**(predicate)

Returns the number of elements in the sequence that pass the `predicate`.

Iterates over the sequence, passing each element to the function `predicate`
and counting the number of times the returned value evaluates to `true`.

<pre class="snippet">
System.print([1, 2, 3].count {|n| n > 2}) //> 1
System.print([1, 2, 3].count {|n| n < 4}) //> 3
</pre>

### **each**(function)

Iterates over the sequence, passing each element to the given `function`.

<pre class="snippet">
["one", "two", "three"].each {|word| System.print(word) }
</pre>

### **find**(predicate), **find**(it, predicate)

Returns an iterator on the sequence that pass the function `predicate`,
starting from the begining of the sequence or `it` if provided.

It is a runtime error if `it` is not a valid iterator value on the sequence.

### **isEmpty**

Returns whether the sequence contains any elements.

This can be more efficient that `count == 0` because this does not iterate over
the entire sequence.

### **join**(separator)

Converts every element in the sequence to a string and then joins the results
together into a single string, each separated by `separator`.

It is a runtime error if `separator` is not a string.

### **join**()

Converts every element in the sequence to a string and then joins the results
together into a single string.

### **map**(transformation)

Creates a new sequence that applies the `transformation` to each element in the
original sequence while it is iterated.

<pre class="snippet">
var doubles = [1, 2, 3].map {|n| n * 2 }
for (n in doubles) {
  System.print(n) //> 2
                  //> 4
                  //> 6
}
</pre>

The returned sequence is *lazy*. It only applies the mapping when you iterate
over the sequence, and it does so by holding a reference to the original
sequence.

This means you can use `map(_)` for things like infinite sequences or sequences
that have side effects when you iterate over them. But it also means that
changes to the original sequence will be reflected in the mapped sequence.

To force eager evaluation, just call `.toList` on the result.

<pre class="snippet">
var numbers = [1, 2, 3]
var doubles = numbers.map {|n| n * 2 }.toList
numbers.add(4)
System.print(doubles) //> [2, 4, 6]
</pre>

### **reduce**(function)

Reduces the sequence down to a single value. `function` is a function that
takes two arguments, the accumulator and sequence item and returns the new
accumulator value. The accumulator is initialized from the first item in the
sequence. Then, the function is invoked on each remaining item in the sequence,
iteratively updating the accumulator.

It is a runtime error to call this on an empty sequence.

### **reduce**(seed, function)

Similar to above, but uses `seed` for the initial value of the accumulator. If
the sequence is empty, returns `seed`.

### **skip**(count)

Creates a new sequence that skips the first `count` elements of the original
sequence.

The returned sequence is *lazy*. The first `count` elements are only skipped
once you start to iterate the returned sequence. Changes to the original
sequence will be reflected in the filtered sequence.

### **take**(count)

Creates a new sequence that iterates only the first `count` elements of the
original sequence.

The returned sequence is *lazy*. Changes to the original sequence will be
reflected in the filtered sequence.

### **toList**

Creates a [list][] containing all the elements in the sequence.

[list]: list.html

<pre class="snippet">
System.print((1..3).toList)  //> [1, 2, 3]
</pre>

If the sequence is already a list, this creates a copy of it.

### **where**(predicate)

Creates a new sequence containing only the elements from the original sequence
that pass the `predicate`.

During iteration, each element in the original sequence is passed to the
function `predicate`. If it returns `false`, the element is skipped.

<pre class="snippet">
var odds = (1..6).where {|n| n % 2 == 1 }
for (n in odds) {
    System.print(n) //> 1
                    //> 3
                    //> 5
}
</pre>

The returned sequence is *lazy*. It only applies the filtering when you iterate
over the sequence, and it does so by holding a reference to the original
sequence.

This means you can use `where(_)` for things like infinite sequences or
sequences that have side effects when you iterate over them. But it also means
that changes to the original sequence will be reflected in the filtered
sequence.

To force eager evaluation, just call `.toList` on the result.

<pre class="snippet">
var numbers = [1, 2, 3, 4, 5, 6]
var odds = numbers.where {|n| n % 2 == 1 }.toList
numbers.add(7)
System.print(odds) //> [1, 3, 5]
</pre>