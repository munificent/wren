System.print(123)     // expect: 123
System.print(987_654)  // expect: 987654
System.print(0)       // expect: 0
System.print(-0)      // expect: -0

System.print(123.456) // expect: 123.456
System.print(-0.001)  // expect: -0.001

// TODO: Literals at and beyond numeric limits.
