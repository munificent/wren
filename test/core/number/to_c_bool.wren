System.print(0.toCBool) // expect: false
System.print((-0).toCBool) // expect: false

System.print(1.toCBool) // expect: true
System.print((-123.45).toCBool) // expect: true

System.print(Num.infinity.toCBool) // expect: true
System.print(Num.nan.toCBool) // expect: true
