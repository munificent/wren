
// FIXME: Add cache.

class Mirror {
  static reflect(reflectee) {
    var mirror = ObjectMirror
    if (reflectee is Class) mirror = ClassMirror

    return mirror.new_(reflectee)
  }
}

class ObjectMirror is Mirror {
  foreign static canInvoke(reflectee, methodName)

  construct new_(reflectee) {
    _reflectee = reflectee
  }

  classMirror {
    if (_classMirror == null) _classMirror = Mirror.reflect(_reflectee.type)
    return _classMirror
  }

  moduleMirror { classMirror.moduleMirror }

  reflectee { _reflectee }

  canInvoke(signature) { classMirror.hasMethod(signature) }
}

class ClassMirror is ObjectMirror {
  foreign static hasMethod(reflectee, signature)
  foreign static methodNames(reflectee)

  construct new_(reflectee) {
    super(reflectee)
    _moduleMirror = null

    _methods = ClassMirror.methodNames(reflectee)
  }

  moduleMirror { _moduleMirror }

  hasMethod(signature) { ClassMirror.hasMethod(reflectee, signature) }

  methodNames { _methodNames }
  methodMirrors { _methodMirrors }
}

class MethodMirror is Mirror {
  foreign static boundToClass_(method)
  foreign static module_(method)
  foreign static signature_(method)

  construct new_(method) {
    _method = method
  }

  boundToClassMirror { Mirror.reflect(MethodMirror.boundToClass_(_method)) }
  moduleMirror { ModuleMirror.fromModule_(MethodMirror.module_(_method)) }

  signature { MethodMirror.signature_(_method) }
}

class ModuleMirror is Mirror {
  foreign static fromName_(name)
  foreign static name_(reflectee)

  static fromModule_(module) {
    return ModuleMirror.new_(module)
  }

  static fromName(name) {
    var module = fromName_(name)
    if (null == module) Fiber.abort("Unkown module")

    return ModuleMirror.fromModule_(module)
  }

  construct new_(reflectee) {
    _reflectee = reflectee
  }

  name { ModuleMirror.name_(_reflectee) }
}
