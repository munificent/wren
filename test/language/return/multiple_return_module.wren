class Foo {
  construct new() {}

  method {
    return "one", "two", "three"
    System.print("bad")
  }
}

var v1, v2, v3 = Foo.new().method
System.print(v1) // expect: one
System.print(v2) // expect: two
System.print(v3) // expect: three
