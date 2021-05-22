class Test {
  construct new() {}
  ~~(needle) { 42 }
  !~(needle) { "nope" }
}

var test = Test.new()

System.print(1 ~~ test) // expect: 42
System.print(1 !~ test) // expect: nope
