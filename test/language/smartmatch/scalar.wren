// Bool
System.print(true ~~ true)    // expect: true
System.print(true ~~ false)   // expect: false
System.print(false ~~ true)   // expect: false
System.print(false ~~ false)  // expect: true

System.print(true !~ true)    // expect: false
System.print(true !~ false)   // expect: true
System.print(false !~ true)   // expect: true
System.print(false !~ false)  // expect: false

// Null
System.print(null ~~ null)        // expect: true
System.print(1 ~~ null)           // expect: false
System.print("not null" ~~ null)  // expect: false

System.print(null !~ null)        // expect: false
System.print(1 !~ null)           // expect: true
System.print("not null" !~ null)  // expect: true

// Num
System.print(1 ~~ 1)    // expect: true
System.print(1 ~~ 1.0)  // expect: true
System.print(0 ~~ 0)    // expect: true
System.print(-0 ~~ 0)   // expect: true
System.print(0 ~~ -0)   // expect: true
System.print(1 ~~ 2)    // expect: false

System.print(1 !~ 1)    // expect: false
System.print(1 !~ 1.0)  // expect: false
System.print(0 !~ 0)    // expect: false
System.print(-0 !~ 0)   // expect: false
System.print(0 !~ -0)   // expect: false
System.print(1 !~ 2)    // expect: true

// String
System.print("" ~~ "")      // expect: true
System.print("\n" ~~ "\n")  // expect: true
System.print("a" ~~ "a")    // expect: true
System.print("a" ~~ "a ")   // expect: false
System.print("a" ~~ "bar")  // expect: false
System.print("bar" ~~ "a")  // expect: false

System.print("" !~ "")      // expect: false
System.print("\n" !~ "\n")  // expect: false
System.print("a" !~ "a")    // expect: false
System.print("a" !~ "a ")   // expect: true
System.print("a" !~ "bar")  // expect: true
System.print("bar" !~ "a")  // expect: true

// StringPart (substring match)
System.print("b" ~~ "bar".part)       // expect: true
System.print("a" ~~ "bar".part)       // expect: true
System.print("r" ~~ "bar".part)       // expect: true
System.print("x" ~~ "bar".part)       // expect: false
System.print("ba" ~~ "bar".part)      // expect: true
System.print("ar" ~~ "bar".part)      // expect: true
System.print("bar" ~~ "bar".part)     // expect: true
System.print("xbar" ~~ "bar".part)    // expect: false
System.print("barx" ~~ "bar".part)    // expect: false

System.print("b" !~ "bar".part)       // expect: false
System.print("a" !~ "bar".part)       // expect: false
System.print("r" !~ "bar".part)       // expect: false
System.print("x" !~ "bar".part)       // expect: true
System.print("ba" !~ "bar".part)      // expect: false
System.print("ar" !~ "bar".part)      // expect: false
System.print("bar" !~ "bar".part)     // expect: false
System.print("xbar" !~ "bar".part)    // expect: true
System.print("barx" !~ "bar".part)    // expect: true

System.print("bar" ~~ "a".part)       // expect: false
System.print("bar" !~ "a".part)       // expect: true
