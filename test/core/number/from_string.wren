System.print(Num.fromString("123") == 123) // expect: true
System.print(Num.fromString("-123") == -123) // expect: true
System.print(Num.fromString("-0") == -0) // expect: true
System.print(Num.fromString("12.34") == 12.34) // expect: true
System.print(Num.fromString("-0.0001") == -0.0001) // expect: true
System.print(Num.fromString(" 12 ") == 12) // expect: true

// Test some non-number literals and ensure they return null.
System.print(Num.fromString("test1") == null) // expect: true
System.print(Num.fromString("") == null) // expect: true
System.print(Num.fromString("prefix1.2") == null) // expect: true
System.print(Num.fromString("1.2suffix") == null) // expect: true

// TODO: Parse hex and scientific numbers.
System.print(Num.fromString("0xDEAD_beef") == 0xdead_beef) // expect: true
System.print(Num.fromString("0o0123_4567") == 0o0123_4567) // expect: true
System.print(Num.fromString("0b1010_0101") == 0b1010_0101) // expect: true
System.print(Num.fromString("25500e-2") == 255) // expect: true
System.print(Num.fromString("2.55e2") == 2.55e2) // expect: true
System.print(Num.fromString("0x") == null) // expect: true
System.print(Num.fromString("0b") == null) // expect: true
System.print(Num.fromString("0o") == null) // expect: true
