#!/bin/bash

# This will get dev and debug headers and ensure libgslcblas0 is installed as
# well along with libgsl23
sudo apt-get install -y libgsl-dev libgsl-dbg

# and get libatlas while you are at it anyways but not necessary at all
sudo apt-get install -y libatlas-base-dev

# Get the latest stable version
wget https://mirror.ibcp.fr/pub/gnu/gsl/gsl-latest.tar.gz

# Make a tempdir since the version can change and you need to be able to figure
# out the folder name
mkdir tempdir
tar -xf gsl-latest.tar.gz --directory tempdir
cd tempdir
FOLDER=$(ls ./)
cd $FOLDER

# Add checking for libgslcblas and libgsl
#sed -i 's@  AC_CHECK_LIB(m, cos)@  AC_CHECK_LIB([m], [cos])\n  AC_CHECK_LIB([gslcblas],[cblas_dgemm])\n  AC_CHECK_LIB([gsl],[gsl_blas_dgemm])@' configure.ac
sed -i 's@  AC_CHECK_LIB(m, cos)@  AC_CHECK_LIB(m,cos)\n  AC_CHECK_LIB(gslcblas,cblas_dgemm)\n  AC_CHECK_LIB(gsl,gsl_blas_dgemm)@' configure.ac

# configure, make, check and install
./configure
make
make check
sudo make install

# Return to the root and cleanup
cd ../../
rm -rf tempdir
rm gsl-latest.tar.gz

