System.print(0.bitwiseShift(0)) // expect: 0
System.print(1.bitwiseShift(0)) // expect: 1

System.print(0.bitwiseShift(1)) // expect: 0
System.print(1.bitwiseShift(1)) // expect: 0
System.print(0xaaaaaaaa.bitwiseShift(1)) // expect: 1431655765
System.print(0xf0f0f0f0.bitwiseShift(1)) // expect: 2021161080

// Max u32 value.
System.print(0xffffffff.bitwiseShift(1)) // expect: 2147483647
System.print(0xffffffff.bitwiseShift(32)) // expect: 0

System.print(0.bitwiseShift(-1)) // expect: 0
System.print(1.bitwiseShift(-1)) // expect: 2
System.print(0xaaaaaaaa.bitwiseShift(-1)) // expect: 1431655764
System.print(0xf0f0f0f0.bitwiseShift(-1)) // expect: 3789677024

// Max u32 value.
System.print(0xffffffff.bitwiseShift(-1)) // expect: 4294967294
System.print(0xffffffff.bitwiseShift(-32)) // expect: 0

// TODO: Floating-point numbers.
// TODO: Numbers that don't fit in u32.
