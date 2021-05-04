
import "mirror" for Mirror

{
  // Class without attributes
  class Foo {}

  var attr = Mirror.reflect(Foo).attributes
  System.print(attr == null)                  // expect: true
}

{
  // Class with attributes
  #!key
  class Foo {}

  var attr = Mirror.reflect(Foo).attributes
  System.print(attr != null)                  // expect: true

  var nullGroup = attr[null]
  System.print(nullGroup != null)             // expect: true
  System.print(nullGroup.count)               // expect: 1
  System.print(nullGroup.containsKey("key"))  // expect: true

  var keyItems = nullGroup["key"]
  System.print(keyItems != null)              // expect: true
  System.print(keyItems is List)              // expect: true
  System.print(keyItems.count)                // expect: 1
  System.print(keyItems[0])                   // expect: null
}
