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

function_build_gnuplot_files() {

if [ "$2" == "-f" ]
then
   files=$(find $1/gp-files)
elif [ "$2" == "-m" ]
then
   files=$(find $1/gp-files -mmin -0.1)
elif [ "$2" == "-mc" ]
then
   files=$(find $1/gp-files -mmin $3)
else
   exit 1
fi

for i in ${files}
do
  # This is so you dont attempt to convert directories or symbolic links etc..
  # and just convert actual local gp files
  if [ ! -f "$i" ]; then continue; fi
  if [ ${i##*.} == "gp" ]
  then
     #echo $i
     gnuplot $i
  fi
done

}

function_render_tex_files () {

BUILD_DIR=$1/build/gnuplot

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
     mv $file_only.pdf $1/figures/
     convert -density 600 -quality 100 -trim $1/figures/$file_only.pdf $1/images/$file_only.png
     #convert -colorspace RGB -density 600 -quality 100 -trim $1/figures/$file_only.pdf $1/images/$file_only.png
  fi
done

cd $1

}

function_run_mode () {

OPTION_VAL=
TIME_VAL=
case "$2" in
   -force | -f) echo "Running on all gnuplot files regardless of update time"
      OPTION_VAL="-f"
      ;;
   -modified | -m) echo "Running only on gnuplot files modified in the last 6 seconds"
      OPTION_VAL="-m"
      ;;
   -modified-custom | -mc) echo "Running only on gnuplot files modified in the last custom set number of seconds"
      OPTION_VAL="-mc"
      TIME_VAL=$3
      ;;
   *) echo "No such command $2 supported for make_gnuplots.sh currently"
      ;;
esac

function_build_gnuplot_files $1 $OPTION_VAL $TIME_VAL
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

BUILD_DIR=$1/build/gnuplot

MUTEX_LOCK="$BUILD_DIR/.gnuplot_lock"
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
