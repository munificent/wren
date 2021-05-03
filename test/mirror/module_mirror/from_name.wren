
import "mirror" for ModuleMirror

var moduleMirror

// Check `core` module handling
moduleMirror = ModuleMirror.fromName("core")

System.print(moduleMirror is ModuleMirror ) // expect: true
System.print(moduleMirror.name ) // expect: core
