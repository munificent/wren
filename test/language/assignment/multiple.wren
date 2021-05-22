
class Foo {
  static four_values {
      return 1, 2, 3, 4
  }

  static one_value {
      return four_values
  }
}

Fn.new {
  var v1, v2, v3, v4, v5, v6, v7, v8 = 1, 2, 1+2, 4, 3+2, 6, 8-1, 4+4
  System.print(v1.toString) // expect: 1
  System.print(v2.toString) // expect: 2
  System.print(v3.toString) // expect: 3
  System.print(v4.toString) // expect: 4
  System.print(v5.toString) // expect: 5
  System.print(v6.toString) // expect: 6
  System.print(v7.toString) // expect: 7
  System.print(v8.toString) // expect: 8

  var a1, a2, a3, a4, a5 = Foo.four_values, 5
  System.print(a1.toString) // expect: 1
  System.print(a2.toString) // expect: 2
  System.print(a3.toString) // expect: 3
  System.print(a4.toString) // expect: 4
  System.print(a5.toString) // expect: 5

  // too many values will leave extra elements on stack
  var x1, x2, x3, x4 = Foo.four_values, 5
  System.print(x1.toString) // expect: 1
  System.print(x2.toString) // expect: 2
  System.print(x3.toString) // expect: 3
  System.print(x4.toString) // expect: 4

  // stack position is now out of sync
  var y1 = 1
  System.print(y1.toString) // expect: 5

  // still out of sync here, but will reset on function return

}.call()

Fn.new {
  // too few values will lead to undefined values
  var z1, z2, z3, z4 = 1, 2, 3
  System.print(z1.toString) // expect: 1
  System.print(z2.toString) // expect: 2
  System.print(z3.toString) // expect: 3
  //System.print(z4.toString) // undefined
}.call()

// module variables
var v1, v2, v3, v4, v5 = 5, 4, 3, 2, 1
System.print(v1.toString) // expect: 5
System.print(v2.toString) // expect: 4
System.print(v3.toString) // expect: 3
System.print(v4.toString) // expect: 2
System.print(v5.toString) // expect: 1

var a1, a2, a3, a4, a5 = Foo.four_values, 5
System.print(a1.toString) // expect: 1
System.print(a2.toString) // expect: 2
System.print(a3.toString) // expect: 3
System.print(a4.toString) // expect: 4
System.print(a5.toString) // expect: 5

// chained returns will not forward multiple returns
var o1 = Foo.one_value
System.print(o1.toString) // expect: 4

// too many values will lead to incorrect stack pop
var x1, x2, x3, x4 = Foo.four_values, 5
System.print(x1.toString) // expect: 2
System.print(x2.toString) // expect: 3
System.print(x3.toString) // expect: 4
System.print(x4.toString) // expect: 5

// too few values will lead to incorrect stack pop
var z1, z2, z3, z4 = 1, 2, 3
//System.print(z1.toString) // undefined
System.print(z2.toString) // expect: 1
System.print(z3.toString) // expect: 2
System.print(z4.toString) // expect: 3
