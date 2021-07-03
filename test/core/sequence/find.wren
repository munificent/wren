
var data = 0...100

System.print(data.find {|value| value == -1 }) // expect: false
System.print(data.iteratorValue(data.find {|value| value == 42 })) // expect: 42
System.print(data.find {|value| value == 100 }) // expect: false

System.print(data.iteratorValue(data.find(data.find {|value| value == 42 }) {|value| value %15 == 0})) // expect: 45
