
class MyNumRange {
  construct new(count) {
    _count = count
  }

  count { _count }

  [index] { index }
}

var r0  = MyNumRange.new(0)
var r1  = MyNumRange.new(1)
var r2  = MyNumRange.new(2)
var r3  = MyNumRange.new(3)
var r4  = MyNumRange.new(4)
var r5  = MyNumRange.new(5)
var r6  = MyNumRange.new(6)
var r7  = MyNumRange.new(7)
var r8  = MyNumRange.new(8)
var r9  = MyNumRange.new(9)
var r10 = MyNumRange.new(10)
var r11 = MyNumRange.new(11)
var r12 = MyNumRange.new(12)
var r13 = MyNumRange.new(13)
var r14 = MyNumRange.new(14)
var r15 = MyNumRange.new(15)
var r16 = MyNumRange.new(16)
var r17 = MyNumRange.new(17)

var fn0  = Fn.new { System.print([]) }
var fn1  = Fn.new {|a| System.print([a]) }
var fn2  = Fn.new {|a, b| System.print([a, b]) }
var fn3  = Fn.new {|a, b, c| System.print([a, b, c]) }
var fn4  = Fn.new {|a, b, c, d| System.print([a, b, c, d]) }
var fn5  = Fn.new {|a, b, c, d, e| System.print([a, b, c, d, e]) }
var fn6  = Fn.new {|a, b, c, d, e, f| System.print([a, b, c, d, e, f]) }
var fn7  = Fn.new {|a, b, c, d, e, f, g| System.print([a, b, c, d, e, f, g]) }
var fn8  = Fn.new {|a, b, c, d, e, f, g, h| System.print([a, b, c, d, e, f, g, h]) }
var fn9  = Fn.new {|a, b, c, d, e, f, g, h, i| System.print([a, b, c, d, e, f, g, h, i]) }
var fn10 = Fn.new {|a, b, c, d, e, f, g, h, i, j| System.print([a, b, c, d, e, f, g, h, i, j]) }
var fn11 = Fn.new {|a, b, c, d, e, f, g, h, i, j, k| System.print([a, b, c, d, e, f, g, h, i, j, k]) }
var fn12 = Fn.new {|a, b, c, d, e, f, g, h, i, j, k, l| System.print([a, b, c, d, e, f, g, h, i, j, k, l]) }
var fn13 = Fn.new {|a, b, c, d, e, f, g, h, i, j, k, l, m| System.print([a, b, c, d, e, f, g, h, i, j, k, l, m]) }
var fn14 = Fn.new {|a, b, c, d, e, f, g, h, i, j, k, l, m, n| System.print([a, b, c, d, e, f, g, h, i, j, k, l, m, n]) }
var fn15 = Fn.new {|a, b, c, d, e, f, g, h, i, j, k, l, m, n, o| System.print([a, b, c, d, e, f, g, h, i, j, k, l, m, n, o]) }
var fn16 = Fn.new {|a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p| System.print([a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p]) }


fn0.callAll(r0 ) // expect: []
fn0.callAll(r1 ) // expect: []
fn0.callAll(r2 ) // expect: []
fn0.callAll(r3 ) // expect: []
fn0.callAll(r4 ) // expect: []
fn0.callAll(r5 ) // expect: []
fn0.callAll(r6 ) // expect: []
fn0.callAll(r7 ) // expect: []
fn0.callAll(r8 ) // expect: []
fn0.callAll(r9 ) // expect: []
fn0.callAll(r10) // expect: []
fn0.callAll(r11) // expect: []
fn0.callAll(r12) // expect: []
fn0.callAll(r13) // expect: []
fn0.callAll(r14) // expect: []
fn0.callAll(r15) // expect: []
fn0.callAll(r16) // expect: []
fn0.callAll(r17) // expect: []

fn1.callAll(r1 ) // expect: [0]
fn1.callAll(r2 ) // expect: [0]
fn1.callAll(r3 ) // expect: [0]
fn1.callAll(r4 ) // expect: [0]
fn1.callAll(r5 ) // expect: [0]
fn1.callAll(r6 ) // expect: [0]
fn1.callAll(r7 ) // expect: [0]
fn1.callAll(r8 ) // expect: [0]
fn1.callAll(r9 ) // expect: [0]
fn1.callAll(r10) // expect: [0]
fn1.callAll(r11) // expect: [0]
fn1.callAll(r12) // expect: [0]
fn1.callAll(r13) // expect: [0]
fn1.callAll(r14) // expect: [0]
fn1.callAll(r15) // expect: [0]
fn1.callAll(r16) // expect: [0]
fn1.callAll(r17) // expect: [0]

fn2.callAll(r2 ) // expect: [0, 1]
fn2.callAll(r3 ) // expect: [0, 1]
fn2.callAll(r4 ) // expect: [0, 1]
fn2.callAll(r5 ) // expect: [0, 1]
fn2.callAll(r6 ) // expect: [0, 1]
fn2.callAll(r7 ) // expect: [0, 1]
fn2.callAll(r8 ) // expect: [0, 1]
fn2.callAll(r9 ) // expect: [0, 1]
fn2.callAll(r10) // expect: [0, 1]
fn2.callAll(r11) // expect: [0, 1]
fn2.callAll(r12) // expect: [0, 1]
fn2.callAll(r13) // expect: [0, 1]
fn2.callAll(r14) // expect: [0, 1]
fn2.callAll(r15) // expect: [0, 1]
fn2.callAll(r16) // expect: [0, 1]
fn2.callAll(r17) // expect: [0, 1]

fn3.callAll(r3 ) // expect: [0, 1, 2]
fn3.callAll(r4 ) // expect: [0, 1, 2]
fn3.callAll(r5 ) // expect: [0, 1, 2]
fn3.callAll(r6 ) // expect: [0, 1, 2]
fn3.callAll(r7 ) // expect: [0, 1, 2]
fn3.callAll(r8 ) // expect: [0, 1, 2]
fn3.callAll(r9 ) // expect: [0, 1, 2]
fn3.callAll(r10) // expect: [0, 1, 2]
fn3.callAll(r11) // expect: [0, 1, 2]
fn3.callAll(r12) // expect: [0, 1, 2]
fn3.callAll(r13) // expect: [0, 1, 2]
fn3.callAll(r14) // expect: [0, 1, 2]
fn3.callAll(r15) // expect: [0, 1, 2]
fn3.callAll(r16) // expect: [0, 1, 2]
fn3.callAll(r17) // expect: [0, 1, 2]

fn4.callAll(r4 ) // expect: [0, 1, 2, 3]
fn4.callAll(r5 ) // expect: [0, 1, 2, 3]
fn4.callAll(r6 ) // expect: [0, 1, 2, 3]
fn4.callAll(r7 ) // expect: [0, 1, 2, 3]
fn4.callAll(r8 ) // expect: [0, 1, 2, 3]
fn4.callAll(r9 ) // expect: [0, 1, 2, 3]
fn4.callAll(r10) // expect: [0, 1, 2, 3]
fn4.callAll(r11) // expect: [0, 1, 2, 3]
fn4.callAll(r12) // expect: [0, 1, 2, 3]
fn4.callAll(r13) // expect: [0, 1, 2, 3]
fn4.callAll(r14) // expect: [0, 1, 2, 3]
fn4.callAll(r15) // expect: [0, 1, 2, 3]
fn4.callAll(r16) // expect: [0, 1, 2, 3]
fn4.callAll(r17) // expect: [0, 1, 2, 3]

fn5.callAll(r5 ) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r6 ) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r7 ) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r8 ) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r9 ) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r10) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r11) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r12) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r13) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r14) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r15) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r16) // expect: [0, 1, 2, 3, 4]
fn5.callAll(r17) // expect: [0, 1, 2, 3, 4]

fn6.callAll(r6 ) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r7 ) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r8 ) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r9 ) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r10) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r11) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r12) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r13) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r14) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r15) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r16) // expect: [0, 1, 2, 3, 4, 5]
fn6.callAll(r17) // expect: [0, 1, 2, 3, 4, 5]

fn7.callAll(r7 ) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r8 ) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r9 ) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r10) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r11) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r12) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6]
fn7.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6]

fn8.callAll(r8 ) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r9 ) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r10) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r11) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r12) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7]
fn8.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7]

fn9.callAll(r9 ) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r10) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r11) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r12) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]
fn9.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8]

fn10.callAll(r10) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r11) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r12) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
fn10.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

fn11.callAll(r11) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
fn11.callAll(r12) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
fn11.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
fn11.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
fn11.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
fn11.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
fn11.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

fn12.callAll(r12) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
fn12.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
fn12.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
fn12.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
fn12.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
fn12.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]

fn13.callAll(r13) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
fn13.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
fn13.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
fn13.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
fn13.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]

fn14.callAll(r14) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]
fn14.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]
fn14.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]
fn14.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]

fn15.callAll(r15) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
fn15.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
fn15.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]

fn16.callAll(r16) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
fn16.callAll(r17) // expect: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
