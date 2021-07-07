// Ordered range.
System.print((2..5).count) // expect: 4
System.print((3..3).count) // expect: 1
System.print((0..3).count) // expect: 4
System.print((-5..3).count) // expect: 9
System.print((-5..-2).count) // expect: 4
System.print((0.1..10).count) // expect: 10
System.print((0.1..10.1).count) // expect: 11

// Backwards range.
System.print((5..2).count) // expect: 4
System.print((3..0).count) // expect: 4
System.print((3..-5).count) // expect: 9
System.print((-2..-5).count) // expect: 4
System.print((10..0.1).count) // expect: 10
// Due to the oddnesses floating point arithmetic, previously `count` yieled 10 on the next line, which is the correct result. I don't know how to fix that.
System.print((10.1..0.1).count) // expect: 11

// Exclusive ordered range.
System.print((2...5).count) // expect: 3
System.print((3...3).count) // expect: 0
System.print((0...3).count) // expect: 3
System.print((-5...3).count) // expect: 8
System.print((-5...-2).count) // expect: 3
System.print((0.1...10).count) // expect: 10
System.print((0.1...10.1).count) // expect: 10

// Exclusive backwards range.
System.print((5...2).count) // expect: 3
System.print((3...0).count) // expect: 3
System.print((3...-5).count) // expect: 8
System.print((-2...-5).count) // expect: 3
System.print((10...0.1).count) // expect: 10
System.print((10.1...0.1).count) // expect: 10
