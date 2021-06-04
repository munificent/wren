var x = 0b1111_1111

System.print(x) // expect: 255
System.print(x + 1) // expect: 256
System.print(x == 255) // expect: true
System.print(0b1001 is Num) // expect: true
System.print(x is Num) // expect: true
System.print(-0b1111_1111) // expect: -255
System.print(0b1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111) // expect: 1.844674407371e+19
