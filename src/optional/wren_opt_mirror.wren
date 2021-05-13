
// FIXME: Add cache.

class Mirror {
  static reflect(reflectee) {
    var mirror = ObjectMirror
    if (reflectee is Class) mirror = ClassMirror
    if (reflectee is Fiber) mirror = FiberMirror

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

class FiberMirror is ObjectMirror {
  foreign static methodAt_(reflectee, stackTraceIndex)
  foreign static lineAt_(reflectee, stackTraceIndex)
  foreign static stackFramesCount_(reflectee)

  construct new_(reflectee) {
    super(reflectee)
  }

  lineAt(stackTraceIndex)   { FiberMirror.lineAt_(reflectee, stackTraceIndex) }
  methodAt(stackTraceIndex) { FiberMirror.methodAt_(reflectee, stackTraceIndex) }
  stackFramesCount          { FiberMirror.stackFramesCount_(reflectee) }

  stackTrace {
    var reflectee = this.reflectee
    var stackFramesCount = FiberMirror.stackFramesCount_(reflectee)
    return StackTrace.new_(reflectee, stackFramesCount)
  }
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

//  arity { MethodMirror.arity_(_method) }
//  maxSlots { MethodMirror.maxSlots_(_method) }
//  numUpvalues { MethodMirror.maxSlots_(_numUpvalues) }
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

class StackTrace is Sequence {
  construct new_(fiber, stackFramesCount) {
    _fiber = fiber
    _stackTrace = []
    for (i in 0...stackFramesCount) {
      _stackTrace.add(StackTraceFrame.new_(fiber, i))
    }
  }

  static new(fiber) {
    var stackFramesCount = FiberMirror.stackFramesCount_(fiber)

    return new_(fiber, stackFramesCount)
  }

  iterate(iterator) { _stackTrace.iterate(iterator) }
  iteratorValue(iterator) { _stackTrace.iteratorValue(iterator) }

  toString { _stackTrace.join("\n") }
}

class StackTraceFrame {
  construct new_(fiber, stackFramesIndex) {
    _line = FiberMirror.lineAt_(fiber, stackFramesIndex)
    _methodMirror = MethodMirror.new_(FiberMirror.methodAt_(fiber, stackFramesIndex))
  }

  line { _line }
  methodMirror { _methodMirror }

  toString { "at %( _methodMirror.moduleMirror.name ): %( _methodMirror.signature ) line %( _line )" }
}
