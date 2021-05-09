// switch statements that are valid but bizarre

System.print("before")        // expect: before
switch(1){
}
System.print("after")         // expect: after

switch(1){
  else System.print("yes")    // expect: yes
}

switch(1){
  else: System.print("yes")   // expect: yes
}

switch(1){
  1: System.print("yes")      // expect: yes
}

switch(1){
  1: 1    // the statement can be an expression
}
