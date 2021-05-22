
import "mirror" for Mirror, ClassMirror


class Foo {
  construct new() { }
}

var mirror = Mirror.reflect(Foo)

System.print(mirror is ClassMirror)   // expect: true
System.print(mirror.reflectee == Foo) // expect: true
