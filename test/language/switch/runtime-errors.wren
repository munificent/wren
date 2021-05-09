switch(123) {
  {|v| v.someNonexistentMethod }: System.print("oops") // expect runtime error: Num does not implement 'someNonexistentMethod'.
}
