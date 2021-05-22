var fiber = Fiber.new {
  "s".unknown
}

System.print(fiber.stackTrace) // expect: null
System.print(fiber.try()) // expect: String does not implement 'unknown'.
System.print(fiber.stackTrace) // expect: at new(_) block argument:([MODULE]) line 2\n
