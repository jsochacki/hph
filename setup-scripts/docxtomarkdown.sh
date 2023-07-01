#!/bin/bash

mkdir tempworkingdir
mv "$1".docx tempworkingdir
cd tempworkingdir
pandoc -s -f docx+styles -t markdown --extract-media ./ "$1".docx -o "$1".md
mv "$1".md ../
cd media
#libreoffice --headless --convert-to png *.emf
libreoffice --headless --convert-to pdf *.*
for file in image*.pdf
do
   convert -density 600 -trim "$file" "${file/.pdf/.png}"
done
for file in image*.png
do
   mv "$file" "${file/image/"$1"image}"
done
mv *.png ../../images
cd ../../
rm -rf tempworkingdir

