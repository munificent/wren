// test foreign-method user data.
// Test in a number of different combinations of foreign and Wren methods,
// to make sure the userdata is staying lined up with the correct method.

class JustForeign {
  foreign static fortyTwo
}

class ForeignFirst {
  foreign static oneThreeThreeSeven
  static oneThreeThreeEight {
    return 1338
  }
}

class ForeignSecond {
  static ninetyNine {
    return 99
  }
  foreign static oneZeroOne
}

class Mixed {
  static oneZeroTwo {
    return 102
  }
  foreign static oneZeroThree
  static oneZeroFour {
    return 104
  }
  foreign static oneZeroFive
}

System.print(JustForeign.fortyTwo) // expect: 42
System.print(ForeignFirst.oneThreeThreeSeven) // expect: 1337
System.print(ForeignFirst.oneThreeThreeEight) // expect: 1338
System.print(ForeignSecond.ninetyNine) // expect: 99
System.print(ForeignSecond.oneZeroOne) // expect: 101
System.print(Mixed.oneZeroTwo) // expect: 102
System.print(Mixed.oneZeroThree) // expect: 103
System.print(Mixed.oneZeroFour) // expect: 104
System.print(Mixed.oneZeroFive) // expect: 105
