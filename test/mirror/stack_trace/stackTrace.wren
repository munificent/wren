
import "mirror" for StackTrace

var fiber = Fiber.new {
  Fiber.yield()
}

var stackTrace = StackTrace.new(fiber)
System.print(stackTrace.count) // expect: 1

fiber.call()

stackTrace = StackTrace.new(fiber)
System.print(stackTrace.count) // expect: 1

// Check it also works with `Fiber.current`
stackTrace = StackTrace.new(Fiber.current)
System.print(stackTrace.count) // expect: 2
