^title Bool Class

Boolean [values][]. There are two instances, `true` and `false`.

[values]: ../../values.html

## Methods

### **!** operator

Returns the logical complement of the value.

<pre class="snippet">
System.print(!true) //> false
System.print(!false) //> true
</pre>

### toString

The string representation of the value, either `"true"` or `"false"`.

### **toCNum**

Converts the value to a Num using the C99 language's notion of truth i.e. false corresponds to `0` and true to `1`.

Note that this differs from Wren's notion of truth where every number (including ±0) is considered to be true