// Ordered range.
System.print((2..5).contains(1)) // expect: false
System.print((2..5).contains(2)) // expect: true
System.print((2..5).contains(5)) // expect: true
System.print((2..5).contains(6)) // expect: false

// Backwards range.
System.print((5..2).contains(1)) // expect: false
System.print((5..2).contains(2)) // expect: true
System.print((5..2).contains(5)) // expect: true
System.print((5..2).contains(6)) // expect: false

// Exclusive ordered range.
System.print((2...5).contains(1)) // expect: false
System.print((2...5).contains(2)) // expect: true
System.print((2...5).contains(5)) // expect: false
System.print((2...5).contains(6)) // expect: false

// Exclusive backwards range.
System.print((5...2).contains(1)) // expect: false
System.print((5...2).contains(2)) // expect: false
System.print((5...2).contains(5)) // expect: true
System.print((5...2).contains(6)) // expect: false

// Not numbers
System.print((0..0).contains("")) // expect: false
System.print((0..0).contains(null)) // expect: false
