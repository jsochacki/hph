#!/bin/bash

# Needs to be passed base dir of project without / i.e. /home/project

###############################################################################
# This is the portion where I define functions to be used by the script

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

# $1 is the pdf_tex full file name and path
# This function makes the compilable tex file in the build directory
# Note that it uses the pdf file in the figures folder still
function_render_pdf_tex () {

full_path=${1%/*} # the full path i.e. "/home/dir"
parent_path=${full_path%/*} # the full parent path i.e. "/home"
file_with_extension=${1##*/} # just the filename with the extension i.e. "file.gp"
file_only=${file_with_extension%.*} # just the filename without the extension i.e. "file"

full_file_out=$parent_path/build/figures/$file_only.tex

# Make the complete tex file
cat >> $full_file_out << 'EOF'
\documentclass{letter}
% Load packages
\usepackage{calc}
\usepackage{graphicx}
\usepackage{color}
\usepackage{transparent}
\usepackage{xcolor}
\usepackage{graphicx}
\usepackage{mathptmx}
\usepackage[english]{babel}
\usepackage[utf8]{inputenc}
\usepackage{amsmath}
\usepackage{float}
\usepackage[pdftex, letterpaper, margin=0pt]{geometry}
% No page numbers and no paragraph indentation
\pagestyle{empty}
\setlength{\parindent}{0bp}%
\begin{document}
EOF
cat $1 >> $full_file_out
echo '\end{document}' >> $full_file_out

# fix path error from export
sed -i "s@$file_only.pdf@$parent_path/figures/$file_only.pdf@" $full_file_out

}

function_svg_to_pdf_pdf_tex_and_render () {

if [ "$2" == "-f" ]
then
   files=$(find $1/figures)
elif [ "$2" == "-m" ]
then
   files=$(find $1/figures -mmin -0.1)
elif [ "$2" == "-mc" ]
then
   files=$(find $1/figures -mmin $3)
else
   exit 1
fi

for i in ${files}
do
  # This is so you dont attempt to convert directories or symbolic links etc..
  # and just convert actual local svg files
  if [ ! -f "$i" ]; then continue; fi
  if [ ${i##*.} == "svg" ]
  then
   # sed remove the annoying error message from diagramsio svg files
    sed -i 's@<switch><g requiredFeatures="http://www.w3.org/TR/SVG11/feature#Extensibility"/><a transform="translate(0,-5)" xlink:href="https://www.diagrams.net/doc/faq/svg-export-text-problems" target="_blank"><text text-anchor="middle" font-size="10px" x="50%" y="100%">Viewer does not support full SVG 1.1</text></a></switch>@@' $i
    # export the pdf and pdf_tex files
    inkscape $i --export-area-page --export-dpi 300 --export-pdf ${i%.*}.pdf --export-latex ${i%.*}.svg

    #inkscape has issues generating pdf_tex and makes multiple pdf includes
    #accidentally so we will remove them as they make files huge
    # Remove anythings that isn't page=1]
    sed -ri '/\\put\(0,0\)\{\\includegraphics\[width\=\\unitlength,page=1](.*)/!s/\\put\(0,0\)\{\\includegraphics\[width\=\\unitlength,page=[[:digit:]]+](.*)//g' ${i%.*}.pdf_tex
    #remove spaces left behind
    sed -ri '/^[[:space:]]*$/d' ${i%.*}.pdf_tex
  fi
done

# First we turn all svgs into pdfs and pdf_tex for latex and use in later steps

for i in ${files}
do
  # This is so you dont attempt to convert directories or symbolic links etc..
  # and just convert actual local pdf_tex files
  if [ ! -f "$i" ]; then continue; fi
  if [ ${i##*.} == "pdf_tex" ]
  then
    function_render_pdf_tex $i
  fi
done

}

function_render_tex_files () {

BUILD_DIR=$1/build/figures

cd $BUILD_DIR

all_tex_files=$(find $BUILD_DIR)
for i in ${all_tex_files}
do
  # This is so you dont attempt to convert directories or symbolic links etc..
  # and just convert actual local tex files
  if [ ! -f "$i" ]; then continue; fi
  if [ ${i##*.} == "tex" ]
  then
     file_with_extension=${i##*/} # just the filename with the extension i.e. "file.gp"
     file_only=${file_with_extension%.*} # just the filename without the extension i.e. "file"
     pdflatex -interaction=nonstopmode $file_with_extension
     if [ ${file_only:(-8)} == "-no-trim" ]
     then
       convert -density 600 -quality 100 -trim $file_only.pdf $1/images/$file_only.png
       convert -density 600 -quality 100 -shave 7x7 +repage $1/images/$file_only.png $1/images/$file_only.png
       # Note that 7x7 removed the exact thickness of the default box from drawio
     else
       convert -density 600 -quality 100 -trim $file_only.pdf $1/images/$file_only.png
     fi
     #convert -colorspace RGB -density 600 -quality 100 -trim $file_only.pdf $1/images/$file_only.png
  fi
done

cd $1

}

function_run_mode () {

OPTION_VAL=
TIME_VAL=
case "$2" in
   -force | -f) echo "Running on all figures regardless of update time"
      OPTION_VAL="-f"
      ;;
   -modified | -m) echo "Running only on figures modified in the last 6 seconds"
      OPTION_VAL="-m"
      ;;
   -modified-custom | -mc) echo "Running only on figures modified in the last custom set number of seconds"
      OPTION_VAL="-mc"
      TIME_VAL=$3
      ;;
   *) echo "No such command $2 supported for update_tex_figures.sh currently"
      ;;
esac

function_svg_to_pdf_pdf_tex_and_render $1 $OPTION_VAL $TIME_VAL
function_render_tex_files $1

}


###############################################################################
# This is the actual script portion

if [ $# -lt 2 ] || [ $# -gt 3 ]
then
   echo "Usage : Argument 1: $1 needs to be the base dir of the project without
   the / at the end, i.e. /home/project"
   echo "Usage : Argument 2: $2 needs to be the run mode command selected i.e.
   -force, -modified, etc...."
   echo "Usage : Argument 3 (optional): $3 the custom time to look for modified
   files, only for when passing run mode -mc, in the form \"-0.1\" for 6 seconds,
   \"-0.5\" for 30 seconds etc..."
   exit 1
fi

BUILD_DIR=$1/build/figures

MUTEX_LOCK="$BUILD_DIR/.figures_lock"
RUN_BUILD=$(function_run_or_not $MUTEX_LOCK)

# Exit if already in process
if [ ! $RUN_BUILD ]
then
   exit 1
fi

if [ $# -eq 3 ]
then
   function_run_mode $1 $2 $3
else
   function_run_mode $1 $2 "-0.1"
fi

FILES_TO_CLEAN=($(ls $BUILD_DIR))
if [ ${#FILES_TO_CLEAN[@]} -gt 0 ]
then
   rm $BUILD_DIR/*
fi

# Only a nuclear 9 will skip this part i.e. kill -9
trap "rm $MUTEX_LOCK" EXIT
