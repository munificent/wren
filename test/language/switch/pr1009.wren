// The example from #1009

var doSomethingElse = Fn.new {
  System.print("something else")
}

// Match all cases, but not in the same order they are listed in the switch
var values = [99, 1, 100, "a", 3, Fn.new {}, 7]

for(topic in values) {
  switch(topic) {
    1: System.print("the topic was == 1")
    "a": System.print("the topic was the string 'a'")
    2..5: System.print("the topic was between 2 and 5")
    [7, 9]: System.print("the topic was either 7 or 9")
    Fn: System.print("the topic was a function!")
    {|v| v%2 == 0}: System.print("the topic was even")
    else: {
      System.print("nothing else matched")
      doSomethingElse.call()
    }
  }
}

// expect: nothing else matched
// expect: something else
// expect: the topic was == 1
// expect: the topic was even
// expect: the topic was the string 'a'
// expect: the topic was between 2 and 5
// expect: the topic was a function!
// expect: the topic was either 7 or 9
