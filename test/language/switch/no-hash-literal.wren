// Hash literals are not allowed as switch tests.
switch("a") {
  { "a": 1, "b": 2 }: System.print("match") // expect error
}

/*
 * Instead of the above, you can do:
 *
 *  switch("a") {
 *    ["a", "b"]: System.print("match")
 *  }
 *
 */
