#!/usr/bin/env python3

import sys
from os.path import basename, dirname, join, realpath, isfile
from glob import iglob
import re

WREN_IMPLEMENTATION = 'WREN_IMPLEMENTATION'
INCLUDE_PATTERN = re.compile(r'^\s*#include "([\w.]+)"')
GUARD_PATTERN = re.compile(r'^#ifndef wren(_\w+)?_h$')
WREN_DIR = dirname(dirname(realpath(__file__)))

seen_files = set()
out = sys.stdout

# Find a file in the different folders of the src dir.
def find_file(filename):
  names = [
    join(WREN_DIR, 'src', 'include', filename),
    join(WREN_DIR, 'src', 'vm', filename),
    join(WREN_DIR, 'src', 'optional', filename),
  ]
  for f in names:
    if isfile(f):
      return f
  raise Exception('File "{0}" not found!'.format(filename))

# Prints a plain text file, adding comment markers.
def add_comment_file(filename):
  with open(filename, 'r') as f:
    for line in f:
      out.write('// ')
      out.write(line)

# Prints the given C source file, recursively resolving local #includes.
def add_file(filename):
  bname = basename(filename)
  # Only include each file at most once.
  if bname in seen_files:
    return
  once = False

  out.write('// Begin file "{0}"\n'.format(bname))
  if bname.endswith('.c'):
    out.write('#ifdef {0}\n'.format(WREN_IMPLEMENTATION))
  with open(filename, 'r') as f:
    for line in f:
      m = INCLUDE_PATTERN.match(line)
      if m:
        add_file(find_file(m.group(1)))
      else:
        out.write(line)
      if GUARD_PATTERN.match(line):
        once = True
  if bname.endswith('.c'):
    out.write('\n#endif // {0}\n'.format(WREN_IMPLEMENTATION))
  out.write('// End file "{0}"\n'.format(bname))

  # Only skip header files which use #ifndef guards.
  # This is necessary because of the X Macro technique.
  if once:
    seen_files.add(bname)

# Print license on top.
add_comment_file(join(WREN_DIR, 'LICENSE'))
out.write('\n')

# Source files.
add_file(join(WREN_DIR, 'src', 'include', 'wren.h'))

# Must be included here because of conditional compilation.
add_file(join(WREN_DIR, 'src', 'vm', 'wren_debug.h'))

for f in iglob(join(WREN_DIR, 'src', 'vm', '*.c')):
  add_file(f)

for f in iglob(join(WREN_DIR, 'src', 'optional', '*.c')):
  add_file(f)
