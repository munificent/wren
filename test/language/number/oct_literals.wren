var o = 0o377

System.print(o) // expect: 255
System.print(o + 1) // expect: 256
System.print(o == 255) // expect: true
System.print(0o07 is Num) // expect: true
System.print(o is Num) // expect: true
System.print(-0o377) // expect: -255
System.print(0o1234567) // expect: 342391