
import "mirror" for Mirror

class Foo {
  static foo {}
  static foo() {}
  static foo(a) {}

  construct new() { }

  bar {}
  bar() {}
  bar(a) {}
}

// Test class
var FooMirror = Mirror.reflect(Foo)

System.print(FooMirror.canInvoke("foo"))    // expect: true
System.print(FooMirror.canInvoke("foo()"))  // expect: true
System.print(FooMirror.canInvoke("foo(_)")) // expect: true

System.print(FooMirror.canInvoke("baz"))    // expect: false
System.print(FooMirror.canInvoke("baz()"))  // expect: false
System.print(FooMirror.canInvoke("baz(_)")) // expect: false

// Test instance
var fooMirror = Mirror.reflect(Foo.new())

System.print(fooMirror.canInvoke("bar"))    // expect: true
System.print(fooMirror.canInvoke("bar()"))  // expect: true
System.print(fooMirror.canInvoke("bar(_)")) // expect: true

System.print(fooMirror.canInvoke("baz"))    // expect: false
System.print(fooMirror.canInvoke("baz()"))  // expect: false
System.print(fooMirror.canInvoke("baz(_)")) // expect: false
