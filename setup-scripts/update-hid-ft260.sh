#!/bin/bash

#Not safe
#homedir=$(eval echo ~$USER)
#Prevents errors due to sudo run setting USER as root
TMPDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd
homedir=$(pwd)
cd $TMPDIR

sudo apt install -y --reinstall linux-headers-$(uname -r)
sudo apt install -y gpiod

cd ../lib
git clone https://github.com/MichaelZaidman/hid-ft260.git
cd hid-ft260
make

sudo rmmod hid-ft260
# Deal with dependencies
#sudo mkdir -p /lib/modules/`uname -r`/kernel/drivers/hph
#sudo ln -s $(pwd)/hid-ft260.ko /lib/modules/`uname -r`/kernel/drivers/hph
#echo "search hph build-in" | sudo tee -a /etc/depmod.d/hph.conf > /dev/null
#echo "override hid-ft260 * hph" | sudo tee -a /etc/depmod.d/hid-ft260.conf > /dev/null
#sudo /sbin/depmod -b /lib/modules/`uname -r`/kernel/drivers/hph -a
#sudo modprobe hid-ft260

sudo ln -s $(pwd)/hid-ft260.ko /lib/modules/`uname -r`
#echo "search build-in" | sudo tee -a /etc/depmod.d/hph.conf > /dev/null
#echo "override hid-ft260 * " | sudo tee -a /etc/depmod.d/hid-ft260.conf > /dev/null
sudo /sbin/depmod -a
sudo modprobe hid-ft260

#check /usr/lib/modules/`uname -r`/modules.dep for <kmod-name>

# Set up for launch with boot
sudo echo "hid-ft260" | sudo tee -a /etc/modules-load.d/hid-ft260.conf > /dev/null

cd $TMPDIR
