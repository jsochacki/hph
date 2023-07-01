#!/bin/bash

# Needs to be passed base dir of project without / i.e. /home/project

###############################################################################
# This is the portion where I define the functions

function_run_or_not () {

PROCEED=

if [ ! -f "$1" ]
then
   touch $1
   # Note not an actual bool since bash has none but just a string named true
   # if you don't assign a string though it will eval to false so acts as bool
   # the way that I use it here
   PROCEED=true
fi

echo "$PROCEED"

}

function_run_pandoc_command () {

case "$2" in
   -markdown-to-pdf | -mdtpdf) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.pdf
      pandoc -s -f markdown-implicit_figures -F mermaid-filter -t pdf $INFILE -o $OUTFILE
      ;;
   -markdown-to-pdf-with-tex-figure-placement | -mdtpdfwtfp) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.pdf
      pandoc -s -f markdown -F mermaid-filter -t pdf $INFILE -o $OUTFILE
      ;;
   -markdown-to-tex-with-tex-figure-placement | -mdttexwtfp) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.tex
      pandoc -s -f markdown -F mermaid-filter -t latex $INFILE -o $OUTFILE
      sed -i 's@begin{figure}$@begin{figure}[H]@g' $OUTFILE
      # Need to run twice to get reference and bibtex to work for some reason
      pdflatex -interaction=nonstopmode -output-directory=$1/build/pandoc $3.tex
      pdflatex -interaction=nonstopmode -output-directory=$1/build/pandoc $3.tex
      mv $1/build/pandoc/$3.pdf $1/
      rm -f $3.tex
      ;;
   -markdown-to-pptx | -mdtpptx) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.pptx
      pandoc -s -f markdown-implicit_figures -F mermaid-filter -t pptx $INFILE -o $OUTFILE --reference-doc template.pptx
      ;;
   -markdown-to-pptx-no-template | -mdtpptxnt) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.pptx
      pandoc -s -f markdown-implicit_figures -F mermaid-filter -t pptx $INFILE -o $OUTFILE
      ;;
   -markdown-to-docx | -mdtdocx) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.docx
      pandoc -s -f markdown -F mermaid-filter -t docx $INFILE -o $OUTFILE --reference-doc template.docx
      # pandoc -s -f markdown-implicit_figures -F mermaid-filter -t docx $INFILE -o $OUTFILE --reference-doc template.docx
      ;;
   -markdown-to-docx-no-template | -mdtdocxnt) echo "$2 selected"
      INFILE=$1/$3.md
      OUTFILE=$1/$3.docx
      pandoc -s -f markdown -F mermaid-filter -t docx $INFILE -o $OUTFILE
      # pandoc -s -f markdown-implicit_figures -F mermaid-filter -t docx $INFILE -o $OUTFILE
      ;;
   -markdown-to-docx-through-latex | -mdtdocxtl) echo "$2 selected"
      INFILE=$1/$3.md
      TMPFILE=$1/$3.tex
      OUTFILE=$1/$3.docx
      pandoc -s -f markdown -F mermaid-filter -t latex $INFILE -o $TMPFILE
      pandoc -s -f latex -t docx $TMPFILE -o $OUTFILE --reference-doc template.docx
      # pandoc -s -f markdown-implicit_figures -F mermaid-filter -t docx $INFILE -o $OUTFILE --reference-doc template.docx
      ;;
   -markdown-to-docx-through-latex-no-template | -mdtdocxtlnt) echo "$2 selected"
      INFILE=$1/$3.md
      TMPFILE=$1/$3.tex
      OUTFILE=$1/$3.docx
      pandoc -s -f markdown -F mermaid-filter -t latex $INFILE -o $TMPFILE
      pandoc -s -f latex -t docx $TMPFILE -o $OUTFILE
      # pandoc -s -f markdown-implicit_figures -F mermaid-filter -t docx $INFILE -o $OUTFILE
      ;;
   *) echo "No such command $2 supported for run_pandoc_commands.sh currently"
      ;;
 
esac

}


###############################################################################
# This is the actual script portion

if [ $# -ne 3 ]
then
   echo "Usage : Argument 1: $1 needs to be the base dir of the project without
   the / at the end, i.e. /home/project"
   echo "Usage : Argument 2: $2 needs to be the pandoc command selected i.e.
   -markdown-to-pdf, -markdown-to-pdf-with-tex-figure-placement, etc...."
   echo "Usage : Argument 3: $3 needs to be the pandoc input file name alone, no
   extension i.e. my-file"
   exit 1
fi

BUILD_DIR=$1/build/pandoc

MUTEX_LOCK="$BUILD_DIR/.pandoc_lock"
RUN_BUILD=$(function_run_or_not $MUTEX_LOCK)

# Exit if already in process
if [ ! $RUN_BUILD ]
then
   exit 1
fi

RETURN_STATUS=$(function_run_pandoc_command $1 $2 $3)

echo $RETURN_STATUS

FILES_TO_CLEAN=($(ls $BUILD_DIR))
if [ ${#FILES_TO_CLEAN[@]} -gt 0 ]
then
   rm $BUILD_DIR/*
fi

# Only a nuclear 9 will skip this part i.e. kill -9
trap "rm $MUTEX_LOCK" EXIT
