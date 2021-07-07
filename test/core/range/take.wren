// Ordered range.
System.print((2..5).take(0).toList) // expect: []
System.print((3..3).take(1).toList) // expect: [3]
System.print((0..3).take(8).toList) // expect: [0, 1, 2, 3]
System.print((-5..3).take(1).toList) // expect: [-5]
System.print((-5..-2).take(0).toList) // expect: []
System.print((0.1..10).take(4).toList) // expect: [0.1, 1.1, 2.1, 3.1]
System.print((0.1..5.1).take(100).toList) // expect: [0.1, 1.1, 2.1, 3.1, 4.1, 5.1]

// Backwards range.
System.print((5..2).take(1).toList) // expect: [5]

// Exclusive ordered range.
System.print((2...5).take(3).toList) // expect: [2, 3, 4]

// Exclusive backwards range.
System.print((5...2).take(3).toList) // expect: [5, 4, 3]
