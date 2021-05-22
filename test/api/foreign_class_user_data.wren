// Apple and Banana are arbitrary class names
foreign class Apple {
  construct new() {}
  foreign instanceUserData
}

foreign class Banana {
  construct new() {}
  foreign instanceUserData
  foreign static finalizeResult
}

var apple = Apple.new()
System.print(apple.instanceUserData) // expect: 12345

var banana = Banana.new()
System.print(banana.instanceUserData) // expect: 8

System.print(Banana.finalizeResult)  // expect: 404
banana = null
System.gc()
System.print(Banana.finalizeResult)  // expect: 57005
