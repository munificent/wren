var bytes = "\0\00\000\0000\033\377\089".bytes

System.print(bytes[0]) // expect: 0
System.print(bytes[1]) // expect: 0
System.print(bytes[2]) // expect: 0
System.print(bytes[3]) // expect: 0
// "0".
System.print(bytes[4]) // expect: 48
System.print(bytes[5]) // expect: 27
System.print(bytes[6]) // expect: 255
System.print(bytes[7]) // expect: 0
// "8".
System.print(bytes[8]) // expect: 56
// "9".
System.print(bytes[9]) // expect: 57
