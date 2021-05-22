// Map
var m = { "a": 1, "b": 2, "c": 3 }
System.print("a" ~~ m)        // expect: true
System.print("b" ~~ m)        // expect: true
System.print("c" ~~ m)        // expect: true
System.print("d" ~~ m)        // expect: false
System.print(1 ~~ m)          // expect: false
System.print(2 ~~ m)          // expect: false
System.print(3 ~~ m)          // expect: false
System.print(4 ~~ m)          // expect: false
System.print(null ~~ m)       // expect: false

System.print("a" !~ m)        // expect: false
System.print("b" !~ m)        // expect: false
System.print("c" !~ m)        // expect: false
System.print("d" !~ m)        // expect: true
System.print(1 !~ m)          // expect: true
System.print(2 !~ m)          // expect: true
System.print(3 !~ m)          // expect: true
System.print(4 !~ m)          // expect: true
System.print(null !~ m)       // expect: true

// Sequence: List
var l = [1, "banana", true]
System.print(1 ~~ l)          // expect: true
System.print("banana" ~~ l)   // expect: true
System.print(true ~~ l)       // expect: true
System.print(2 ~~ l)          // expect: false
System.print("banan" ~~ l)    // expect: false
System.print("anana" ~~ l)    // expect: false
System.print("" ~~ l)         // expect: false
System.print(false ~~ l)      // expect: false

System.print(1 !~ l)          // expect: false
System.print("banana" !~ l)   // expect: false
System.print(true !~ l)       // expect: false
System.print(2 !~ l)          // expect: true
System.print("banan" !~ l)    // expect: true
System.print("anana" !~ l)    // expect: true
System.print("" !~ l)         // expect: true
System.print(false !~ l)      // expect: true

// Sequence: Range
System.print(1 ~~ 1..1)         // expect: true
System.print(1 ~~ 1...1)        // expect: false
System.print(-1 ~~ -5..5)       // expect: true
System.print(Num.nan ~~ -5..5)  // expect: false
System.print("a" ~~ -5..5)      // expect: false

System.print(1 !~ 1..1)         // expect: false
System.print(1 !~ 1...1)        // expect: true
System.print(-1 !~ -5..5)       // expect: false
System.print(Num.nan !~ -5..5)  // expect: true
System.print("a" !~ -5..5)      // expect: true
