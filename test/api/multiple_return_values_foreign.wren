
class MultipleReturnValuesForeign {
  foreign static values()

  static local_values() {
    var v1, v2, v3, v4 = values()
    System.print(v1.toString) // expect: 1
    System.print(v2.toString) // expect: 2
    System.print(v3.toString) // expect: 3
    System.print(v4.toString) // expect: 4
  }
}

MultipleReturnValuesForeign.local_values()

var v1, v2, v3, v4 = MultipleReturnValuesForeign.values()
System.print(v1.toString) // expect: 1
System.print(v2.toString) // expect: 2
System.print(v3.toString) // expect: 3
System.print(v4.toString) // expect: 4
