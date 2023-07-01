#!/bin/bash

#THIS FILE IS NOT WORKING, I HAVE NOT SPENT ANY TIME ON IT AND THINK THAT I WILL
#DEPRICATE IT BUT RIGHT NOW IT IS INSTALLED SO I JUST WANTED TO WARN YOU
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################

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

function_build_path () {

BASEPROJECTPATH=
IN_PATH_LOWERCASE=$(echo "$1"  | tr [:upper:] [:lower:])
IN_PATH_LOWERCASE_FOLDERS=($(echo "$IN_PATH_LOWERCASE"  | tr "/" " "))

if [ ${#IN_PATH_LOWERCASE_FOLDERS[@]} -eq 0 ]
then
   #echo 'You are running this from your systems root, please read how to use
   #      this script and try again'
   echo $BASEPROJECTPATH
   exit 1
fi

if [ ${#IN_PATH_LOWERCASE_FOLDERS[@]} -eq 1 ]
then
   #echo 'You are running this from one level up from your systems root,
   #      if your project is really in the root folder_variable you need to move it and try
   #      again please'
   echo $BASEPROJECTPATH
   exit 1
fi

let i=0
let ind=0

if [ ${#IN_PATH_LOWERCASE_FOLDERS[@]} -gt 1 ]
then
   for folder_variable in ${IN_PATH_LOWERCASE_FOLDERS[*]}
   do
      if [ "$folder_variable" == "lib-src-inc" ]
      then
         let ind=$i
         let i=-1
         break
      fi
      let i+=1
   done
fi

if [ $i -ne -1 ]
then
   #echo 'lib-src-inc directory not found, please check usage and try again'
   echo $BASEPROJECTPATH
   exit 1
fi

# If you made it this far you found the folder_variable and are good to go
let i=0
for folder_variable in ${IN_PATH_LOWERCASE_FOLDERS[*]}
do
   if [ $i -lt $ind ]
   then
      BASEPROJECTPATH+="/$folder_variable"
   fi
   let i+=1
done

echo "$BASEPROJECTPATH"

}

function_run_cmake_command () {

case "$2" in
   -force-build-full | -fbf) echo "$2 selected"
      LAUNCH_DIRECTORY=$(pwd)
      LATEX_DIR="$1/lib-src-inc/build/doc/latex"
      cd $1/lib-src-inc
      cmake Doxyfile.in
      cd $LATEX_DIR
      sed -i "s@DOCUMENTTITLE@Code Documentation@g" $LATEX_DIR/refman.tex
      sed -i "s@REVISIONNUMBER@0.0.0@g" $LATEX_DIR/refman.tex
      make
      cp $LATEX_DIR/refman.pdf $1/code-documentation.pdf
      mv $LATEX_DIR/refman.pdf $1/lib-src-inc/doc/pdf/code-documentation.pdf
      rm -rf $1/lib-src-inc/doc/html
      mv $1/lib-src-inc/build/doc/html $1/lib-src-inc/doc/html
      touch $1/lib-src-inc/doc/html/.gitkeep
      cd $LAUNCH_DIRECTORY
      ;;
   -view-the-pdf-docs | -vtpdfd) echo "$2 selected"
      if [ -a $1/code-documentation.pdf ]
      then
         zathura $1/code-documentation.pdf &
      fi
      ;;
   -view-the-html-docs | -vthtmld) echo "$2 selected"
      if [ -a $1/lib-src-inc/doc/html/index.html ]
      then
         google-chrome-stable $1/lib-src-inc/doc/html/index.html &
      fi
      ;;
   *) echo "No such command $2 supported for run_cmake_commands.sh currently"
      ;;
esac

}


###############################################################################
# This is the actual script portion

if [ $# -ne 2 ]
then
   echo "Usage : Argument 1: $1 needs to be the code directory of the project or
   a directory within the code directory of the project.  Paths can omit the
   slash but they can also have it as well
   i.e. /home/project/lib-src-inc or /home/project/lib-src-inc/src/"
   echo "Usage : Argument 2: $2 needs to be the cmake command selected i.e.
   -force-build-full, -build-full, -fbf, -bf, etc..."
   exit 1
fi

PROJECT_DIRECTORY=$(function_build_path $1)
echo $PROJECT_DIRECTORY
echo $1
# Exit if project directory not found
if [ ! $PROJECT_DIRECTORY ]
then
   exit 1
fi

BUILD_DIR="$PROJECT_DIRECTORY/lib-src-inc/build/doc"

MUTEX_LOCK="$BUILD_DIR/.cmake_lock"
RUN_BUILD=$(function_run_or_not $MUTEX_LOCK)

# Exit if already in process
if [ ! $RUN_BUILD ]
then
   exit 1
fi

RETURN_STATUS=$(function_run_cmake_command $PROJECT_DIRECTORY $2)

echo $RETURN_STATUS

FILES_TO_CLEAN=($(ls $BUILD_DIR))
if [ ${#FILES_TO_CLEAN[@]} -gt 0 ]
then
   rm -rf $BUILD_DIR/*
fi

# Only a nuclear 9 will skip this part i.e. kill -9
trap "rm $MUTEX_LOCK" EXIT
