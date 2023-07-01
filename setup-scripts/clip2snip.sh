#!/bin/bash

# Here we assume only png files are in Snips
# and that those files are just named 1.png, 2.png and so on

current_number=1
file=$(ls ~/Snips/ | sort -g -r | head -n 1)

if [ ! -z $file ]
then
   filename=$(basename "$file")
   extension=${filename##*.}
   filename=${filename%.*}

   echo "$filename $extension"
   echo "$extension"
   if [ $extension == "png" ]
   then
      if [ $filename -ge 1 ]
      then
          current_number=$(($filename + 1))
      else
          current_number=1
      fi
      echo "$current_number"
   fi
else
   current_number=1
fi

xclip -selection clipboard -t image/png -o > ~/Snips/$current_number.png
