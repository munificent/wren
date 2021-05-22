System.print(123)     // expect: 123
System.print(987654)  // expect: 987654
System.print(0)       // expect: 0
System.print(-0)      // expect: -0

System.print(123.456) // expect: 123.456
System.print(-0.001)  // expect: -0.001

System.print(0.000000000000000000000000000000000000783475) // expect: 7.83475e-37

// TODO: Literals at and beyond numeric limits.
