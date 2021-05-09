var outer = 42

switch(42) {
  {|v| v == outer }: System.print("yes")  // expect: yes
}

// Switch statements that refer to upvalues
class SwitchMaker {

  static makeSwitch(test) {
    return Fn.new {|topic|
      switch(topic) {
        test: System.print("yes")
        else: System.print("no")
      }
    }
  }

  static makeNestedSwitch(test) {
    return Fn.new {|topic|
      switch(topic) {
        {|v| v == test }: System.print("yes")
        else: System.print("no")
      }
    }
  }
}

var sw = SwitchMaker.makeSwitch(1337)
sw.call(1337)   // expect: yes
sw.call("x")    // expect: no
sw.call(null)   // expect: no

sw = SwitchMaker.makeNestedSwitch(65535)
sw.call(65535)  // expect: yes
sw.call("x")    // expect: no
sw.call(null)   // expect: no
