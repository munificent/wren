var fiber = Fiber.new {
  System.print("before")
  Fiber.yield()
  System.print("after")
}

System.print(fiber.stackTrace) // expect: null
System.print(fiber.call()) // expect: before
System.print(fiber.stackTrace) // expect: at new(_) block argument:([MODULE]) line 3
System.print(fiber.call()) // expect: after
System.print(fiber.stackTrace) // expect: null

