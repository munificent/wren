var b = 0b11111111

System.print(b) // expect: 255
System.print(b + 1) // expect: 256
System.print(b == 255) // expect: true
System.print(0b11111111 is Num) // expect: true
System.print(b is Num) // expect: true
System.print(-0b11111111) // expect: -255
System.print(0b11111111111111111111111111111111) // expect: 4294967295