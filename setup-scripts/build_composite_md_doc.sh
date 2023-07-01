#!/bin/bash

rm "$1".md "$1".pdf
for name in "${@:2}"
do
   cat "$name".md >> "$1".md
done

run_pandoc_commands.sh $(pwd) -mdttexwtfp "$1"
# If you want word to have linked references you must build to latex and then to
# docx
run_pandoc_commands.sh $(pwd) -mdtdocxtl "$1"
