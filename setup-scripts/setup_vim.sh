#!/bin/bash

# To build vim with literally all features use this
sudo apt-get install -y libncurses5-dev libgtk2.0-dev libatk1.0-dev libcairo2-dev libx11-dev  libxpm-dev libxt-dev python-dev ruby-dev mercurial

# Get Vim (needed Python 3 all set up first sadly for vim build to support ultisnips)
sudo apt-get install -y libncurses5-dev libncursesw5-dev build-essential
git clone https://github.com/vim/vim.git
cd vim/src
./configure --with-features=huge --enable-python3interp=yes --with-x
# Build with both
#./configure --with-features=huge --enable-python3interp=yes --enable-gui=auto --with-x
make
sudo make install
cd ../..
rm -rf vim
sudo update-alternatives --install /usr/bin/vim vim /usr/local/bin/vim 1
sudo update-alternatives  --set vim /usr/local/bin/vim
