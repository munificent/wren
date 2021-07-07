// Ordered range.
System.print((2..5).skip(1).toList) // expect: [3, 4, 5]
System.print((3..3).skip(0).toList) // expect: [3]
System.print((0..3).skip(8).toList) // expect: []
System.print((-5..3).skip(1).toList) // expect: [-4, -3, -2, -1, 0, 1, 2, 3]
System.print((-5..-2).skip(9).toList) // expect: []
System.print((0.1..10).skip(4).toList) // expect: [4.1, 5.1, 6.1, 7.1, 8.1, 9.1]
System.print((0.1..5.1).skip(100).toList) // expect: []

// Backwards range.
System.print((5..2).skip(1).toList) // expect: [4, 3, 2]

// Exclusive ordered range.
System.print((2...5).skip(2).toList) // expect: [4]

// Exclusive backwards range.
System.print((5...2).skip(2).toList) // expect: [3]
