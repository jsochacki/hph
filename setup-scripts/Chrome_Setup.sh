#!/bin/bash

echo "Installing latest version of Chrome"
wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
sudo dpkg -i google-chrome-stable_current_amd64.deb
sudo apt-get -y install -f
sudo rm google-chrome-stable_current_amd64.deb
