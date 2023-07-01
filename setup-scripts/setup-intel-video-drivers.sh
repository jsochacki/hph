#!/bin/bash

# add apt sources first
sudo apt-get install -y gpg-agent wget
wget -qO - https://repositories.intel.com/graphics/intel-graphics.key |
  sudo gpg --dearmor --output /usr/share/keyrings/intel-graphics.gpg
echo 'deb [arch=amd64 signed-by=/usr/share/keyrings/intel-graphics.gpg] https://repositories.intel.com/graphics/ubuntu focal main' | \
  sudo tee -a /etc/apt/sources.list.d/intel.list

# Update the packages
sudo apt-get update -y
sudo apt-get dist-upgrade -y

# Install intel video driver
sudo apt-get install -y \
  intel-opencl-icd \
  intel-level-zero-gpu level-zero \
  intel-media-va-driver-non-free libmfx1 libmfxgen1 libvpl2
  
sudo gpasswd -a ${USER} render
newgrp render

exit
