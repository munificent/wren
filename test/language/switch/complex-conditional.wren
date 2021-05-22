switch("a") {
  "bar": System.print("yes")
  else System.print("no")             // expect: no
}

switch("a") {
  "bar".part: System.print("yes")     // expect: yes
}

switch("a") {
  ["a", "b"]: System.print("yes")     // expect: yes
}

switch("b") {
  ["a", "b"]: System.print("yes")     // expect: yes
}

switch("c") {
  ["a", "b"]: System.print("yes")
  else System.print("no")             // expect: no
}

// from https://github.com/wren-lang/wren/issues/956#issuecomment-813660551

// Using function literals
switch(7) {
  {|v| v>6 }: System.print("yes")     // expect: yes
}

switch(5) {
  {|v| v>6 }: System.print("yes")
  else System.print("no")             // expect: no
}

switch(7) {
  // with space before pipe
  { |v| v>6 }: System.print("yes")    // expect: yes
}

switch(1) {
  {|v| v != 2 }: System.print("yes")  // expect: yes
}

switch(2) {
  {|v| v != 2 }: System.print("yes")
  else System.print("no")             // expect: no
}

switch(2) {
  1..50: System.print("yes")          // expect: yes
}

switch(-1) {
  1..50: System.print("yes")
  else System.print("no")             // expect: no
}

switch(2) {
  {|v| v%2 == 0}: System.print("yes") // expect: yes
}

switch(1) {
  {|v| v%2 == 0}: System.print("yes")
  else System.print("no")             // expect: no
}

switch("abc") {
  String: System.print("yes")         // expect: yes
}

switch(1) {
  String: System.print("yes")
  else System.print("no")             // expect: no
}

// Using express Fn.new calls still works
switch(7) {
  Fn.new {|v| v>6 }: System.print("yes")   // expect: yes
  // NOTE: this would be much terser with a keyword `fn` or the like!
}

switch(5) {
  Fn.new {|v| v>6 }: System.print("yes")
  else System.print("no")             // expect: no
}

// Class/subclass tests
switch(String) {
  String: System.print("yes")         // expect: yes
}

switch(String) {  // String is Sequence
  Sequence: System.print("yes")       // expect: yes
}

/*
 * Other things mentioned at
 * <https://github.com/wren-lang/wren/issues/352#issuecomment-237459289>:
 * "pattern matching could end up metastasizing on us: value matching, type
 * matching, list destructing, map destructuring, object destructuring, etc.
 * Ranges? Predicate clauses? Wildcards?"
 *
 * TODO? write tests for these?
 */
