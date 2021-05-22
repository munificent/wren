// Fiber
var fi1 = Fiber.new {}
var fi2 = Fiber.new {}
System.print(fi1 ~~ fi1) // expect: true
System.print(fi2 ~~ fi2) // expect: true
System.print(fi1 ~~ fi2) // expect: false
System.print(fi2 ~~ fi1) // expect: false

System.print(fi1 !~ fi1) // expect: false
System.print(fi2 !~ fi2) // expect: false
System.print(fi1 !~ fi2) // expect: true
System.print(fi2 !~ fi1) // expect: true

// Fn
var fn1 = Fn.new { 42 }
var fn2 = Fn.new { |x| x+1 }
System.print(fn1 ~~ fn1)      // expect: 42
System.print(1 ~~ fn1)        // expect: 42
System.print(null ~~ fn1)     // expect: 42
System.print("str" ~~ fn1)    // expect: 42
System.print(Num ~~ fn1)      // expect: 42

System.print(1 ~~ fn2)        // expect: 2
System.print(-1 ~~ fn2)       // expect: 0
var fiber = Fiber.new { "x" ~~ fn2 }
var error = fiber.try()
System.print(error) // expect: Right operand must be a string.

var returnTrue = Fn.new { true }
var returnFalse = Fn.new { false }
System.print(1 ~~ returnTrue)   // expect: true
System.print(1 ~~ returnFalse)  // expect: false

System.print(1 !~ returnTrue)   // expect: false
System.print(1 !~ returnFalse)  // expect: true

// System
System.print(System ~~ System)  // expect: true
System.print(true ~~ System)    // expect: false
System.print(1 ~~ System)       // expect: false
System.print("a" ~~ System)     // expect: false
System.print(fi1 ~~ System)     // expect: false
System.print(fn1 ~~ System)     // expect: false

System.print(System !~ System)  // expect: false
System.print(true !~ System)    // expect: true
System.print(1 !~ System)       // expect: true
System.print("a" !~ System)     // expect: true
System.print(fi1 !~ System)     // expect: true
System.print(fn1 !~ System)     // expect: true

// Note: not testing Meta or Random specifically, since they might not be
// compiled into Wren.

// Object
class Test {
  construct new() {}
  // inherits Object.~~(_)
}
var t1 = Test.new()
var t2 = Test.new()

System.print(t1 ~~ t1) // expect: true
System.print(t2 ~~ t2) // expect: true
System.print(t1 ~~ t2) // expect: false
System.print(t2 ~~ t1) // expect: false

System.print(t1 !~ t1) // expect: false
System.print(t2 !~ t2) // expect: false
System.print(t1 !~ t2) // expect: true
System.print(t2 !~ t1) // expect: true

class TestChild is Test {
  construct new() {}
}
var tc = TestChild.new()

// Class: values against classes
System.print(t1 ~~ Test)  // expect: true
System.print(t1 ~~ Num)   // expect: false

System.print(t1 ~~ TestChild) // expect: false
System.print(tc ~~ TestChild) // expect: true
System.print(tc ~~ Test)      // expect: true

System.print(fi1 ~~ Fiber)        // expect: true
System.print(fn1 ~~ Fn)           // expect: true
System.print(fn1 ~~ Fiber)        // expect: false
System.print(fi1 ~~ Fn)           // expect: false
System.print("a" ~~ String)       // expect: true
System.print("a" ~~ Num)          // expect: false
System.print(1 ~~ Num)            // expect: true
System.print(Num.nan ~~ Num)      // expect: true
System.print(Num.infinity ~~ Num) // expect: true

System.print(t1 !~ Test)  // expect: false
System.print(t1 !~ Num)   // expect: true

System.print(t1 !~ TestChild) // expect: true
System.print(tc !~ TestChild) // expect: false
System.print(tc !~ Test)      // expect: false

System.print(fi1 !~ Fiber)        // expect: false
System.print(fn1 !~ Fn)           // expect: false
System.print(fn1 !~ Fiber)        // expect: true
System.print(fi1 !~ Fn)           // expect: true
System.print("a" !~ String)       // expect: false
System.print("a" !~ Num)          // expect: true
System.print(1 !~ Num)            // expect: false
System.print(Num.nan !~ Num)      // expect: false
System.print(Num.infinity !~ Num) // expect: false

// Class: classes against classes
System.print(Test ~~ Test)        // expect: true
System.print(Test ~~ Object)      // expect: true
System.print(TestChild ~~ Test)   // expect: true
System.print(TestChild ~~ Object) // expect: true
System.print(Test ~~ TestChild)   // expect: false
System.print(Test ~~ String)      // expect: false
System.print(String ~~ Test)      // expect: false

System.print(Test !~ Test)        // expect: false
System.print(Test !~ Object)      // expect: false
System.print(TestChild !~ Test)   // expect: false
System.print(TestChild !~ Object) // expect: false
System.print(Test !~ TestChild)   // expect: true
System.print(Test !~ String)      // expect: true
System.print(String !~ Test)      // expect: true
