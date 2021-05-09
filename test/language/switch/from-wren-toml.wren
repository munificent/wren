// This test modified from code by David Newman
// at <https://github.com/datatypevoid/wren-toml/blob/6818065a0043106958d394b4943bb59b69633029/src/scanner.wren#L149-L191>.
// Referenced at <https://github.com/wren-lang/wren/issues/956#issuecomment-817412149>.
//
// This file is licensed as follows:
//
// Copyright (c) 2018, David Newman
// Copyright (c) 2021, Christopher White
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// SPDX-License-Identifier: ISC

var char = ","

if (false) {
  System.print("oops")
} else if (false) {
  System.print("also oops")
} else switch(char) {
  "=": System.print("Equal")
  "\"": {
    System.print("\"")
  }
  "#": System.print("not this either")
  "[": System.print("LeftBracket")
  "]": System.print("RightBracket")
  "'": System.print("'")
  ",": System.print("Comma")          // expect: Comma
  "{": System.print("LeftBrace")
  "}": System.print("RightBrace")
  else Fiber.abort("very oops")
}
