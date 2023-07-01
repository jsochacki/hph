#!/bin/bash


#Manual youcompletemeinstall if plug doesn't work which it shouldnt
sudo apt-get install -y build-essential cmake python3-dev
sudo apt-get install -y mono-complete golang nodejs npm
# Need this for python code completion and linting
pip3 install jedi
# install clangd from https://clangd.llvm.org/
sudo apt-get install -y clangd
curdir=$(pwd)
#This location doesnt' work while the next one does
#mkdir -p ~/.vim/bundle/
#cd ~/.vim/bundle/
mkdir -p ~/.vim/pack/YouCompleteMe/opt/
cd ~/.vim/pack/YouCompleteMe/opt/
git clone https://github.com/ycm-core/YouCompleteMe.git
cd YouCompleteMe
git submodule update --init --recursive

#cut this section out from here
#now do the build
#cd ~
#mkdir ycm_build
#cd ycm_build
#cmake -G "<generator>" . ~/.vim/bundle/YouCompleteMe/third_party/ycmd/cpp
#cmake --build . --target ycm_core --config Release
#sut this section out to here
#python3 install.py --clang-completer --ts-completer
python3 install.py --clang-completer
#Alternative option i have seen
#python3 install.py --all
cd $(echo $curdir | tr -d '\r')