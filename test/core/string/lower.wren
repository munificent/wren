System.print("abc def ghi".lower == "abc def ghi") // expect: true
System.print("ABC DEF GHI".lower == "abc def ghi") // expect: true
System.print("AbC dEf GhI".lower == "abc def ghi") // expect: true

// Empty string
System.print("".lower == "") // expect: true

// Type
System.print("ABC DEF GHI".lower is String) // expect: true

// ISO 8859-1
System.print("ÇÂÑEd".lower == "çâñed") // expect: true

// UTF-8
System.print("søMéஃTHînGsøméஃ".lower == "søméஃthîngsøméஃ") // expect: true
