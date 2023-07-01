#!/bin/bash

# To build vim with literally all features use this
# Note you already have these all and these will mess up the versions you have so DON"T run this in your scripts
# it is just for information completeness
#sudo apt-get install -y python3 g++ make

# Get Vim (needed Python 3 all set up first sadly for vim build to support ultisnips)
curl https://nodejs.org/dist/v16.13.0/node-v16.13.0.tar.gz --output node
tar -xvf node
cd node-v16.13.0
./configure
make -j8
sudo make install
cd ../
sudo rm -rf node
sudo rm -rf node-v16.13.0
