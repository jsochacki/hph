#!/bin/bash

# This will get bison, flex, and libiconv(which is included in libc6 which you
# will already have by now but just in case you don't we add here)/make
# graphviz is only 9MB so just get in for the future use if you need it
sudo apt-get install -y bison flex libc6 make graphviz

# Get the latest stable version
wget https://doxygen.nl/files/doxygen-1.9.1.src.tar.gz

# Make a tempdir since the version can change and you need to be able to figure
# out the folder name
mkdir tempdir
tar -xf doxygen-1.9.1.src.tar.gz --directory tempdir
cd tempdir
FOLDER=$(ls ./)
cd $FOLDER
mkdir build
cd build

cmake -G "Unix Makefiles" ..

# configure, make, check and install
make
#cmake -Dbuild_doc=YES ..
#make docs install
cmake ..
sudo make install

# Return to the root and cleanup
cd ../../../
sudo rm -rf tempdir
rm doxygen-1.9.1.src.tar.gz

# Note , some things to do in other files when you have the time
# doxygen Doxyfile.in
# cd out/latex
# pdflatex refman.tex

