#!/bin/bash

FULL_PATH_WITH_DIR=$1
CURDIR=$(pwd)

cp -r $CURDIR/library_source_tree $FULL_PATH_WITH_DIR
mkdir -p $FULL_PATH_WITH_DIR
mkdir -p $FULL_PATH_WITH_DIR/doc
mkdir -p $FULL_PATH_WITH_DIR/doc/pdf
mkdir -p $FULL_PATH_WITH_DIR/doc/html
mkdir -p $FULL_PATH_WITH_DIR/doc/images
mkdir -p $FULL_PATH_WITH_DIR/doc/build/doc
touch $FULL_PATH_WITH_DIR/doc/pdf/.gitkeep
touch $FULL_PATH_WITH_DIR/doc/html/.gitkeep
touch $FULL_PATH_WITH_DIR/doc/images/.gitkeep
touch $FULL_PATH_WITH_DIR/doc/build/doc/.gitkeep
cp $CURDIR/Doxyfile.in $FULL_PATH_WITH_DIR/
cp $CURDIR/doxygen_header.tex $FULL_PATH_WITH_DIR/
cp $CURDIR/doxygen_footer.tex $FULL_PATH_WITH_DIR/
cp $CURDIR/systems_book_doxygen.sty $FULL_PATH_WITH_DIR/
