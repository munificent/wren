
import "mirror" for ObjectMirror

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

System.print(ObjectMirror.canInvoke(Foo, "foo"))    // expect: true
System.print(ObjectMirror.canInvoke(Foo, "foo()"))  // expect: true
System.print(ObjectMirror.canInvoke(Foo, "foo(_)")) // expect: true

System.print(ObjectMirror.canInvoke(Foo, "baz"))    // expect: false
System.print(ObjectMirror.canInvoke(Foo, "baz()"))  // expect: false
System.print(ObjectMirror.canInvoke(Foo, "baz(_)")) // expect: false

// Test instance
var foo = Foo.new()

System.print(ObjectMirror.canInvoke(foo, "bar"))    // expect: true
System.print(ObjectMirror.canInvoke(foo, "bar()"))  // expect: true
System.print(ObjectMirror.canInvoke(foo, "bar(_)")) // expect: true

System.print(ObjectMirror.canInvoke(foo, "baz"))    // expect: false
System.print(ObjectMirror.canInvoke(foo, "baz()"))  // expect: false
System.print(ObjectMirror.canInvoke(foo, "baz(_)")) // expect: false
