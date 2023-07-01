# This function just formats the entire file you are currently in

# It operates on the current, potentially unsaved buffer and does not create
# or save any files. To revert a formatting, just undo.
from __future__ import absolute_import, division, print_function

import difflib
import json
import platform
import subprocess
import sys
import vim
# To get absolute path to files
from pathlib import Path
# For symbolic Link Creation
import os

# Here we create a symbolic link in the local directory to the global
# .clang-format file so that we can run with the command line file option
# and have it use our global settings, always make a new one even if one
# already exists so that there are no issues
clang_file_fullpath = str(Path.home())+"/.clang-format"
temporary_symbolic_link = str(os.getcwd())+"/.clang-format"
if os.path.exists(temporary_symbolic_link):
  os.remove(temporary_symbolic_link)
os.symlink(clang_file_fullpath, temporary_symbolic_link)

# set g:clang_format_path to the path to clang-format if it is not on the path
# Change this to the full path if clang-format is not on the path.
binary = 'clang-format-10'
if vim.eval('exists("g:clang_format_path")') == "1":
  binary = vim.eval('g:clang_format_path')

# Change this to format according to other formatting styles. See the output of
# 'clang-format --help' for a list of supported styles. The default looks for
# a '.clang-format' or '_clang-format' file to indicate the style that should be
# used.
style = 'file'
fallback_style = 'google'
if vim.eval('exists("g:clang_format_fallback_style")') == "1":
  fallback_style = vim.eval('g:clang_format_fallback_style')

def get_buffer(encoding):
  if platform.python_version_tuple()[0] == '3':
    return vim.current.buffer
  return [ line.decode(encoding) for line in vim.current.buffer ]

def main():
  # Get the current text.
  encoding = vim.eval("&encoding")
  buf = get_buffer(encoding)
  # Join the buffer into a single string with a terminating newline
  text = '\n'.join(buf) + '\n'

  # Determine the cursor position.so that the cursor ends back where it started
  # after the formatting operation
  cursor = int(vim.eval('line2byte(line("."))+col(".")')) - 2
  if cursor < 0:
    print('Couldn\'t determine cursor position. Is your file empty?')
    return

  # Avoid flashing an ugly, ugly cmd prompt on Windows when invoking clang-format.
  startupinfo = None
  if sys.platform.startswith('win32'):
    startupinfo = subprocess.STARTUPINFO()
    startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startupinfo.wShowWindow = subprocess.SW_HIDE

  # Call formatter.
  command = [binary, '-cursor', str(cursor)]
  if style:
    command.extend(['-style', style])
  if fallback_style:
    command.extend(['-fallback-style', fallback_style])
  if vim.current.buffer.name:
    command.extend(['-assume-filename', vim.current.buffer.name])
  p = subprocess.Popen(command,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                       stdin=subprocess.PIPE, startupinfo=startupinfo)
  stdout, stderr = p.communicate(input=text.encode(encoding))

  # If successful, replace buffer contents.
  if stderr:
    print(stderr)

  if not stdout:
    print(
        'No output from clang-format (crashed?).\n'
        'Please report to bugs.llvm.org.'
    )
  else:
    lines = stdout.decode(encoding).split('\n')
    output = json.loads(lines[0])
    # Strip off the trailing newline (added above).
    # This maintains trailing empty lines present in the buffer if
    # the -lines specification requests them to remain unchanged.
    lines = lines[1:-1]
    sequence = difflib.SequenceMatcher(None, buf, lines)
    for op in reversed(sequence.get_opcodes()):
      if op[0] != 'equal':
        vim.current.buffer[op[1]:op[2]] = lines[op[3]:op[4]]
    if output.get('IncompleteFormat'):
      print('clang-format: incomplete (syntax errors)')
    vim.command('goto %d' % (output['Cursor'] + 1))

    # Here we just remove the symbolic link we created so we dont trash up
    # the directory structure
    if os.path.exists(temporary_symbolic_link):
       os.remove(temporary_symbolic_link)
    else:
       print("Error!!! Symbolic link to .clang-format file was never created")

main()