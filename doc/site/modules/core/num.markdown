^title Num Class

## Static Methods

### Num.**fromString**(value[, base])

Attempts to parse `value` as a decimal literal and return it as an instance of
`Num`. Optionally, `base` can be set to use a specific radix between `2` 
and `36` (or `0` to choose radix automatically). If the number cannot be 
parsed `null` will be returned.

It is a runtime error if `value` is not a string, `base` is set and is not an 
integer, `base` is above `36`, or `base` is lower than `2` and not `0`.

### Num.**infinity**

The value of &infin;.

### Num.**nan**

One value representing a NaN.

Provides a default NaN number suitable for the vm internal values.

### Num.**pi**

The value of &pi;.

### Num.**tau**

The value of &tau;. This is equivalent to ```2 * Num.pi```.

### Num.**largest**

The largest representable numeric value.

### Num.**smallest**

The smallest positive representable numeric value.

### Num.**maxSafeInteger**

The largest integer that Wren can safely represent. It's a constant value of `9007199254740991`.

 This is relevant because Wren uses double precision [floating-point format](https://en.wikipedia.org/wiki/IEEE_floating_point)
 for numbers, which can only safely represent integers between <code>-(2<sup>53</sup> - 1)</code> and <code>2<sup>53</sup> - 1</code>.

### Num.**minSafeInteger**

The smallest integer Wren can safely represent. It's a constant value of `-9007199254740991`. 

## Methods

### **abs**

The absolute value of the number.

<pre class="snippet">
System.print( (-123).abs ) //> 123
</pre>

### **acos**

The arc cosine of the number.

### **asin**

The arc sine of the number.

### **atan**

The arc tangent of the number.

### **atan**(x)

The arc tangent of the number when divided by `x`, using the signs of the two
numbers to determine the quadrant of the result.

### **cbrt**

The cube root of the number.

### **ceil**

Rounds the number up to the nearest integer.

<pre class="snippet">
System.print(1.5.ceil)    //> 2
System.print((-3.2).ceil) //> -3
</pre>

### **cos**

The cosine of the number.

### **floor**

Rounds the number down to the nearest integer.

<pre class="snippet">
System.print(1.5.floor)    //> 1
System.print((-3.2).floor) //> -4
</pre>

### **fraction**

The fractional part of a number i.e. the part after any decimal point.

The returned value has the same sign as `this`.

<pre class="snippet">
System.print(1.5.fraction)    //> 0.5
System.print((-3.2).fraction) //> -0.2
</pre>

### **isInfinity**

Whether the number is positive or negative infinity or not.

<pre class="snippet">
System.print(99999.isInfinity)  //> false
System.print((1/0).isInfinity)  //> true
</pre>

### **isInteger**

Whether the number is an integer or has some fractional component.

<pre class="snippet">
System.print(2.isInteger)   //> true
System.print(2.3.isInteger) //> false
</pre>

### **isNan**

Whether the number is [not a number](http://en.wikipedia.org/wiki/NaN). This is
`false` for normal number values and infinities, and `true` for the result of
`0/0`, the square root of a negative number, etc.

### **log**

The natural logarithm of the number. Returns `nan` if the base is negative.

### **log2**

The binary (base-2) logarithm of the number. Returns `nan` if the base is negative.

### **exp**

The exponential `e` (Euler’s number) raised to the number. This: `eⁿ`. 

### **min**(other)

Returns the minimum value when comparing this number and `other`.

### **max**(other)

Returns the maximum value when comparing this number and `other`.

### **clamp**(min, max)

Clamps a number into the range of `min` and `max`. If this number is less than min, 
`min` is returned. If bigger than `max`, `max` is returned. Otherwise, the number 
itself is returned.

### **pow**(power)

Raises this number (the base) to `power`. Returns `nan` if the base is negative.

### **round**

Rounds the number to the nearest integer.

<pre class="snippet">
System.print(1.5.round)    //> 2
System.print((-3.2).round) //> -3
System.print((-3.7).round) //> -4
</pre>

### **sign**

The sign of the number, expressed as a -1, 1 or 0, for negative and positive numbers, and zero.

### **sin**

The sine of the number.

### **sqrt**

The square root of the number. Returns `nan` if the number is negative.

### **tan**

The tangent of the number.

### **toString**

The string representation of the number.

### **truncate**

Rounds the number to the nearest integer towards zero.

It is therefore equivalent to `floor` if the number is non-negative or `ceil` if it is negative.

<pre class="snippet">
System.print(1.5.truncate)    //> 1
System.print((-3.2).truncate) //> -3
</pre>

### **-** operator

Negates the number.

<pre class="snippet">
var a = 123
System.print(-a) //> -123
</pre>

### **-**(other), **+**(other), **/**(other), **\***(other) operators

The usual arithmetic operators you know and love. All of them do 64-bit
floating point arithmetic. It is a runtime error if the right-hand operand is
not a number. Wren doesn't roll with implicit conversions.

### **%**(denominator) operator

Also known as mod or modulus.   
The floating-point remainder of this number divided by `denominator`. 

The returned value has the same sign as `this` (internally calls `fmod` from C).

It is a runtime error if `denominator` is not a number.

### **&lt;**(other), **&gt;**(other), **&lt;=**(other), **&gt;=**(other) operators

Compares this and `other`, returning `true` or `false` based on how the numbers
are ordered. It is a runtime error if `other` is not a number.

### **~** operator

Performs *bitwise* negation on the number. The number is first converted to a
32-bit unsigned value, which will truncate any floating point value. The bits
of the result of that are then negated, yielding the result.

### **&**(other) operator

Performs bitwise and on the number. Both numbers are first converted to 32-bit
unsigned values. The result is then a 32-bit unsigned number where each bit is
`true` only where the corresponding bits of both inputs were `true`.

It is a runtime error if `other` is not a number.

### **|**(other) operator

Performs bitwise or on the number. Both numbers are first converted to 32-bit
unsigned values. The result is then a 32-bit unsigned number where each bit is
`true` only where the corresponding bits of one or both inputs were `true`.

It is a runtime error if `other` is not a number.

### **^**(other) operator

Performs bitwise exclusive or on the number. Both numbers are first converted to 32-bit unsigned values. The result is then a 32-bit unsigned number where each bit is `true` only where the corresponding bits of one (but not both) inputs were `true`. Each bit is therefore `false` if the corresponding bits of both inputs were either both `true` or both `false`.

It is a runtime error if `other` is not a number.

### **&lt;&lt;**(other) operator

Performs a bitwise left shift on the number. Internally, both numbers are first converted to 32-bit unsigned values and C's left shift operator is then applied to them.

It is a runtime error if `other` is not a number.

### **&gt;&gt;**(other) operator

Performs a bitwise right shift on the number. Internally, both numbers are first converted to 32-bit unsigned values and C's right shift operator is then applied to them.

It is a runtime error if `other` is not a number.

### **..**(other) operator

Creates a [Range](range.html) representing a consecutive range of numbers
from the beginning number to the ending number.

<pre class="snippet">
var range = 1.2..3.4
System.print(range.min)         //> 1.2
System.print(range.max)         //> 3.4
System.print(range.isInclusive) //> true
</pre>

### **...**(other) operator

Creates a [Range](range.html) representing a consecutive range of numbers
from the beginning number to the ending number not including the ending number.

<pre class="snippet">
var range = 1.2...3.4
System.print(range.min)         //> 1.2
System.print(range.max)         //> 3.4
System.print(range.isInclusive) //> false
</pre>
