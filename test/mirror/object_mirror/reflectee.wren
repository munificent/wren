
import "mirror" for Mirror, ObjectMirror

class Foo {
  construct new() { }
}

var foo = Foo.new()
var mirror = Mirror.reflect(foo)

System.print(mirror is ObjectMirror)  // expect: true
System.print(mirror.reflectee == foo) // expect: true
