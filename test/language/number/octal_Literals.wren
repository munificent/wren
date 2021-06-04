var x = 0o377

System.print(x) // expect: 255
System.print(x + 1) // expect: 256
System.print(x == 255) // expect: true
System.print(0o011 is Num) // expect: true
System.print(x is Num) // expect: true
System.print(-0o377) // expect: -255
System.print(0o0123_4567) // expect: 342391
