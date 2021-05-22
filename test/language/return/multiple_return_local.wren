class Foo {
  construct new() {}

  method {
    return "one", "two", "three"
    System.print("bad")
  }

  // single line returns
  values { 1, 2, 3, 4, 5 }
  str_values { "one", "two", "three" }

  local_values() {
    var v1, v2, v3 = method
    System.print(v1) // expect: one
    System.print(v2) // expect: two
    System.print(v3) // expect: three
  }

  local_single_return() {
    var s1, s2, s3, s4, s5 = values
    System.print(s1.toString) // expect: 1
    System.print(s2.toString) // expect: 2
    System.print(s3.toString) // expect: 3
    System.print(s4.toString) // expect: 4
    System.print(s5.toString) // expect: 5
  }

  // This will try to call "print" on object "two" with "three" as argument
  // since the receiver is implicitly looked up from the number of arguments in
  // the OPCODE.
  //
  // The stack looks like this:
  // ... [System] ["one"] ["two"] ["three"]
  //
  local_method_arguments_overflow_bad() {
    System.print(str_values)    // expect runtime error: String does not implement 'print(_)'.
  }
}

Foo.new().local_values()
Foo.new().local_single_return()
Foo.new().local_method_arguments_overflow_bad()
