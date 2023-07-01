#!/bin/bash

#Not safe
#homedir=$(eval echo ~$USER)
#Prevents errors due to sudo run setting USER as root
TMPDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd
homedir=$(pwd)
cd $TMPDIR

sudo apt install -y libudev1=245.4-4ubuntu3.20
sudo apt install -y libudev-dev

cd ../src/hph-ft260
make
sudo ln -s $(pwd)/hph-ft260.ko /lib/modules/`uname -r`
sudo /sbin/depmod -a
sudo modprobe hph-ft260

#check /usr/lib/modules/`uname -r`/modules.dep for <kmod-name>

# Set up for launch with boot
sudo echo "hph-ft260" | sudo tee -a /etc/modules-load.d/hph-ft260.conf > /dev/null

cd $TMPDIR
