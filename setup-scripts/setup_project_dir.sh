#!/bin/bash

FULL_PATH_WITH_DIR=$1
CURDIR=$(pwd)

mkdir -p $FULL_PATH_WITH_DIR/figures
mkdir -p $FULL_PATH_WITH_DIR/images
mkdir -p $FULL_PATH_WITH_DIR/csv-data
mkdir -p $FULL_PATH_WITH_DIR/gp-files
mkdir -p $FULL_PATH_WITH_DIR/build
mkdir -p $FULL_PATH_WITH_DIR/build/gnuplot
mkdir -p $FULL_PATH_WITH_DIR/build/figures
mkdir -p $FULL_PATH_WITH_DIR/build/pandoc
touch $FULL_PATH_WITH_DIR/figures/.gitkeep
touch $FULL_PATH_WITH_DIR/images/.gitkeep
touch $FULL_PATH_WITH_DIR/csv-data/.gitkeep
touch $FULL_PATH_WITH_DIR/gp-files/.gitkeep
touch $FULL_PATH_WITH_DIR/build/gnuplot/.gitkeep
touch $FULL_PATH_WITH_DIR/build/figures/.gitkeep
touch $FULL_PATH_WITH_DIR/build/pandoc/.gitkeep
cp $CURDIR/None.rasi $FULL_PATH_WITH_DIR/
cp $CURDIR/systems_book.sty $FULL_PATH_WITH_DIR/
cp $CURDIR/systems_book_md.sty $FULL_PATH_WITH_DIR/
cp $CURDIR/linear_xy_line_plot.gptemplate $FULL_PATH_WITH_DIR/gp-files/
cp $CURDIR/linear_xy_lines_and_points_comparison_plot.gptemplate $FULL_PATH_WITH_DIR/gp-files/
