^title Lists

A list is a compound object that holds a collection of elements identified by integer index. You can create a list by placing a sequence of comma-separated expressions inside square brackets:

    :::wren
    [1, "banana", true]

Here, we've created a list of three elements. Notice that the elements don't have to be the same type.

## Accessing Elements

You can access an element from a list by calling the [subscript operator](method-calls.html#subscript-operators) on it with the index of the element you want. Like most languages, indexes start at zero:

    :::wren
    var hirsute = ["sideburns", "porkchops", "'stache"]
    hirsute[0] // "sideburns".
    hirsute[1] // "porkchops".

Negative indices counts backwards from the end:

    :::wren
    hirsute[-1] // "'stache".
    hirsute[-2] // "porkchops".

It's a runtime error to pass an index outside of the bounds of the list. If you don't know what those bounds are, you can find out using count:

    :::wren
    hirsute.count // 3.

## Adding elements

Lists are *mutable*, meaning their contents can be changed. You can swap out an existing element in the list using the subscript setter:

    :::wren
    hirsute[1] = "muttonchops"
    IO.print(hirsute[1]) // muttonchops.

It's an error to set an element that's out of bounds. To grow a list, you can use `add` to append a single item to the end:

    :::wren
    hirsute.add("goatee")
    IO.print(hirsute.count) // 4.

You can insert a new element at a specific position using `insert`:

    :::wren
    hirsute.insert("soul patch", 2)

The first argument is the value to insert, and the second is the index to insert it at. All elements following the inserted one will be pushed down to make room for it.

It's valid to "insert" after the last element in the list, but only *right* after it. Like other methods, you can use a negative index to count from the back. Doing so counts back from the size of the list *after* it's grown by one:

    :::wren
    var letters = ["a", "b", "c"]
    letters.insert("d", 3)   // OK: inserts at end.
    IO.print(letters)        // ["a", "b", "c", "d"]
    letters.insert("e", -2)  // Counts back from size after insert.
    IO.print(letters)        // ["a", "b", "c", "e", "d"]

## Removing elements

The opposite of `insert` is `removeAt`. It removes a single element from a given position in the list. All following items are shifted up to fill in the gap:

    var letters = ["a", "b", "c"]
    letters.removeAt(1)
    IO.print(letters) // ["a", "c"]

If you want to remove everything from the list, you can clear it:

    hirsute.clear
    IO.print(hirsute) // []

**TODO: Ranges, iteration, etc.**
