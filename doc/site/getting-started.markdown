^title Getting Started

Getting Wren up and running on your machine should be pretty straightforward.
Tiny C programs with no dependencies are nice that way. If you're on a Unix or
Mac and you can rock a command line, it's just:

    :::bash
    $ git clone https://github.com/munificent/wren.git
    $ cd wren
    $ make
    $ ./wren

Binaries for other configurations are built to `bin/`. Static and shared
libraries for embedding Wren get built in `lib/`.

For Mac users, there is also an XCode project under `project/xcode`. For
Windows brethren, `project/msvc2013` contains a Visual Studio solution. Note
that these may not have the exact same build settings as the makefile. The
makefile is the "official" way to compile Wren.

## Interactive mode

The above instructions will drop you into Wren's standalone interpreter in
interactive mode. You can type in a line of code, and it will immediately
execute it. Here's something to try:

    :::dart
    IO.print("Hello, world!")

Or a little more exciting:

    :::dart
    for (i in 1..10) IO.print("Counting up ", i)

You can exit the interpreter using good old Ctrl-C or Ctrl-D, or just throw
your computer to the ground and storm off.

## Running scripts

The standalone interpreter can also load scripts from files and run them. Just
pass the name of the script to wren. Create a file named "my_script.wren" in
your favorite text editor and paste this into it:

    :::dart
    for (yPixel in 0...24) {
      var y = yPixel / 12 - 1
      for (xPixel in 0...80) {
        var x = xPixel / 30 - 2
        var x0 = x
        var y0 = y
        var iter = 0
        while (iter < 11 && x0 * x0 + y0 * y0 <= 4) {
          var x1 = (x0 * x0) - (y0 * y0) + x
          var y1 = 2 * x0 * y0 + y
          x0 = x1
          y0 = y1
          iter = iter + 1
        }
        IO.write(" .-:;+=xX$& "[iter])
      }

      IO.print("")
    }

Now run:

    :::bash
    $ ./wren my_script.wren

Neat, right? You're a Wren programmer now! The next step is to [read more
docs](syntax.html) and learn your way around the language. If you run into
bugs, or have ideas or questions, any of the following work:

 *  **Ask on the [Wren mailing list][list].**
 *  Tell me on twitter at [@munificentbob][twitter].
 *  [File a ticket][issue] at [the GitHub repo][repo].
 *  Send a pull request.
 *  Email me at [`robert@stuffwithstuff.com`](mailto:robert@stuffwithstuff.com).

[list]: https://groups.google.com/forum/#!forum/wren-lang
[twitter]: https://twitter.com/intent/user?screen_name=munificentbob
[issue]: https://github.com/munificent/wren/issues
[repo]: https://github.com/munificent/wren
