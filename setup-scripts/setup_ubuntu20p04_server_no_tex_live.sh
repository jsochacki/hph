#!/bin/bash

#Not safe
#homedir=$(eval echo ~$USER)
#Prevents errors due to sudo run setting USER as root
TMPDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd
homedir=$(pwd)
cd $TMPDIR

function_apt_wait_for_unlock () {
  while sudo fuser /var/lib/dpkg/lock >/dev/null 2>&1 ; do
    sleep 5
  done
  while sudo fuser /var/lib/dpkg/lock-frontend >/dev/null 2>&1 ; do
    sleep 5
  done
  while sudo fuser /var/lib/apt/lists/lock >/dev/null 2>&1 ; do
    sleep 5
  done
  if [ -f /var/log/unattended-upgrades/unattended-upgrades.log ]; then
    while sudo fuser /var/log/unattended-upgrades/unattended-upgrades.log >/dev/null 2>&1 ; do
      sleep 5
    done
  fi
  $@
}

# Inkscape fails to install if you don't
function_apt_wait_for_unlock sudo apt-get update -y

# Fix package dependencies issues created in last step
function_apt_wait_for_unlock sudo apt-get install -y --allow-downgrades libc6=2.31-0ubuntu9.7
function_apt_wait_for_unlock sudo apt-get install -y libc6-dbg libc6-dev glibc-doc
function_apt_wait_for_unlock sudo apt-get install -y linux-source

# hph driver test section
# Nope, have to manually build this one to get current functionality
#function_apt_wait_for_unlock sudo apt-get install -y libhidapi-dev
wget http://archive.ubuntu.com/ubuntu/pool/main/s/systemd/libudev-dev_245.4-4ubuntu3.22_amd64.deb
sudo apt install ./libudev-dev_245.4-4ubuntu3.22_amd64.deb 
sudo rm libudev-dev_245.4-4ubuntu3.22_amd64.deb
git clone https://github.com/libusb/hidapi.git
mkdir build
cd build
cmake ../hidapi/

# Need to get X display server starter for i3
function_apt_wait_for_unlock sudo apt install -y xinit

# Get git, curl, and make
function_apt_wait_for_unlock sudo apt-get install -y git curl make

# Get vim
#function_apt_wait_for_unlock sudo apt-get install -y vim build-essential clang-format

# Get Cpp development dependencies
function_apt_wait_for_unlock sudo apt-get install -y g++

# Get Cpp development dependencies
function_apt_wait_for_unlock sudo apt-get install -y build-essential clang-format

# you need to run this from the git directory

#function_apt_wait_for_unlock ./install_and_setup_texlive.sh

# Get window managers and window management utilities
function_apt_wait_for_unlock sudo apt-get install -y tmux bspwm i3 feh rxvt
# https://i3wm.org/docs/userguide.html

# Get PDF Viewer
function_apt_wait_for_unlock sudo apt-get install -y zathura zathura-pdf-poppler

# Get latex compiler for vimtex
function_apt_wait_for_unlock sudo apt-get install -y latexmk xzdec
# Get for forward searching in zatuhura with vimtex
function_apt_wait_for_unlock sudo apt-get install -y xdotool
# Get inkscape for non diagram figures and prefent "Failed to load module "canberra-gtk-module"" error
function_apt_wait_for_unlock sudo apt-get install -y libcanberra-gtk-module libcanberra-gtk3-module
# The other option is inkscape=0.92.5-1ubuntu1 but i believe 1.1 is the one all
# of this was developed around
function_apt_wait_for_unlock sudo apt-get install -y inkscape=0.92.5-1ubuntu1.1
# Need these to support automated vimtex inkscape interaction
function_apt_wait_for_unlock sudo apt-get install -y python3.8-dev rofi python3-pip

# Need to actually get pip3.8 as well
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python3.8 get-pip.py
rm get-pip.py
echo '' >> $homedir/.bashrc
echo '# Adding path for latex helpers' >> $homedir/.bashrc
echo 'export PATH="'$homedir'/.local/bin:$PATH"' >> $homedir/.bashrc

#Do now so you can finish setup without leaving the script
export PATH=$homedir/.local/bin:$PATH

sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 2
sudo update-alternatives  --set python3 /usr/bin/python3.8
#Can run this to visually check
#sudo update-alternatives --config python
#or this
#python3 --version

# Finally install
cd
git clone https://github.com/jsochacki/inkscape-latex-figures.git
cd inkscape-latex-figures
pip3.8 install --user .
cd ../
rm -rf inkscape-latex-figures

# Install the python version i made for diagrams.net images
git clone https://github.com/jsochacki/diagrams-net-figures.git
cd diagrams-net-figures
pip3.8 install --user .
cd ../
rm -rf diagrams-net-figures

#inkscape shortcut manager setup
git clone https://github.com/python-xlib/python-xlib.git
cd python-xlib
pip3.8 install --user .
cd ../
rm -rf python-xlib

# Just clone from mine instead
cd $homedir/.local/lib/python3.8/site-packages/
git clone https://github.com/jsochacki/inkscape-latex-shortcuts.git

# Get vim-plug and set it up
curl -fLo $homedir/.vim/autoload/plug.vim --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim

function_apt_wait_for_unlock sudo apt-get install -y xclip pdf2svg
# It isn't in his list but you need urxvt to use text insertion etc...
#function_apt_wait_for_unlock sudo apt-get install -y rxvt
# I already get it though for i3 so we are good


cd $TMPDIR
#"install" my script and files
ln -s $TMPDIR/update_tex_figures.sh $homedir/.local/bin/update_tex_figures.sh
ln -s $TMPDIR/make_gnuplots.sh $homedir/.local/bin/make_gnuplots.sh
ln -s $TMPDIR/run_pandoc_commands.sh $homedir/.local/bin/run_pandoc_commands.sh
ln -s $TMPDIR/run_doxygen_commands.sh $homedir/.local/bin/run_doxygen_commands.sh
ln -s $TMPDIR/run_cmake_commands.sh $homedir/.local/bin/run_cmake_commands.sh
#cp update_tex_figures.sh $homedir/.local/bin/update_tex_figures.sh

# Do some vimrc setup
ln -s $TMPDIR/.vimrc.plugged $homedir/
ln -s $TMPDIR/.vimrc.normal $homedir/
ln -s $TMPDIR/.vimrc.colors $homedir/
ln -s $TMPDIR/.vimrc.custom $homedir/
ln -s $TMPDIR/.vimrc $homedir/
ln -sf $TMPDIR/en.utf-8.add $homedir/.vim/spell/
#cp .vimrc.plugged $homedir/
#cp .vimrc.normal $homedir/
#cp .vimrc.colors $homedir/
#cp .vimrc.custom $homedir/
#cp .vimrc $homedir/

# Do some filetype specific setup
mkdir -p  $homedir/.vim/ftplugin/
ln -s $TMPDIR/c.vim $homedir/.vim/ftplugin/
ln -s $TMPDIR/python.vim $homedir/.vim/ftplugin/
ln -s $TMPDIR/tex.vim $homedir/.vim/ftplugin/
ln -s $TMPDIR/markdown.vim $homedir/.vim/ftplugin/
ln -s $TMPDIR/matlab.vim $homedir/.vim/ftplugin/
#cp c.vim $homedir/.vim/ftplugin/
#cp python.vim $homedir/.vim/ftplugin/
#cp tex.vim $homedir/.vim/ftplugin/
#cp markdown.vim $homedir/.vim/ftplugin/

# Do some non vimrc setup
mkdir -p $homedir/.vim/UltiSnips
ln -s $TMPDIR/tex.snippets $homedir/.vim/UltiSnips/
ln -s $TMPDIR/markdown.snippets $homedir/.vim/UltiSnips/
ln -s $TMPDIR/cpp.snippets $homedir/.vim/UltiSnips/
ln -s $TMPDIR/c.snippets $homedir/.vim/UltiSnips/
ln -s $TMPDIR/cu.snippets $homedir/.vim/UltiSnips/
ln -s $TMPDIR/matlab.snippets $homedir/.vim/UltiSnips/
#cp tex.snippets $homedir/.vim/UltiSnips/
#cp cpp.snippets $homedir/.vim/UltiSnips/
#cp c.snippets $homedir/.vim/UltiSnips/
#cp cu.snippets $homedir/.vim/UltiSnips/

# Install vim, you need to wait until python 3 is setup as we compile with
# python 3
function_apt_wait_for_unlock ./setup_vim.sh

#Manual youcompletemeinstall if plug doesn't work which it shouldnt
function_apt_wait_for_unlock ./install_and_setup_youcompleteme.sh

# Add matlab syntax file in case you decide to install matlab
mkdir -p $homedir/.vim/syntax
ln -s $TMPDIR/mymatlab.vim $homedir/.vim/syntax/

# Copy over scripts and config files
mkdir -p $homedir/.local/bin
ln -s $TMPDIR/myclang-format.py3 $homedir/.local/bin/clang-format.py
ln -s $TMPDIR/.clang-format $homedir/
#cp myclang-format.py3 $homedir/.local/bin/clang-format.py
#cp .clang-format $homedir/
mkdir -p $homedir/.config/zathura/
ln -s $TMPDIR/zathurarc $homedir/.config/zathura/
#cp zathurarc $homedir/.config/zathura/
mkdir -p $homedir/.config/i3/
ln -s $TMPDIR/i3config $homedir/.config/i3/config
ln -s $TMPDIR/.Xresources $homedir/
#cp i3config $homedir/.config/i3/config
#cp .Xresources $homedir/
mkdir -p $homedir/.cfiles
ln -s $TMPDIR/c.ycm_extra_conf.py $homedir/.cfiles/.ycm_extra_conf.py
#cp c.ycm_extra_conf.py $homedir/.cfiles/.ycm_extra_conf.py
mkdir -p $homedir/.pyfiles
ln -s $TMPDIR/py.ycm_extra_conf.py $homedir/.pyfiles/.ycm_extra_conf.py
#cp py.ycm_extra_conf.py $homedir/.pyfiles/.ycm_extra_conf.py
# Dont actually need to do this below to get urxvt settings to take
#echo 'xrdb $homedir/.Xresources' >> $homedir/.xinitrc
mkdir -p $homedir/.vimfiles

# Setup auto google
ln -s $TMPDIR/Googleit.py $homedir/.cfiles/
#cp Googleit.py $homedir/.cfiles/
chmod guo+x $homedir/.cfiles/Googleit.py
ln -s $homedir/.cfiles/Googleit.py $homedir/.pyfiles/
ln -s $homedir/.cfiles/Googleit.py $homedir/.vimfiles/
#cp $homedir/.cfiles/Googleit.py $homedir/.pyfiles/
#cp $homedir/.cfiles/Googleit.py $homedir/.vimfiles/

# Opens vim, installs the plugins, then quits back to shell
vim +PlugInstall +qa


mkdir -p $homedir/Pictures
mkdir -p $homedir/Snips

# Get backgrounds
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter1.jpg http://wallpaperim.net/_data/i/upload/2014/09/23/20140923661374-3acd5e08-me.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter2.jpg https://cache.desktopnexus.com/wallpapers/391/391188-1920x1080-beautiful-winter-landscape-1920x1080-wallpaper-568.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter3.jpg http://www.bhmpics.com/walls/dead_snow_winter-other.jpg
#wget -O $homedir/Pictures/winter4.jpg https://wallpapercave.com/wp/0sCOQyE.jpg
#wget -O $homedir/Pictures/winter5.jpg https://wallpapercave.com/wp/mc5kprj.jpg
#wget -O $homedir/Pictures/winter6.jpg https://wallpapercave.com/wp/jNHBSir.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter7.jpg https://images.alphacoders.com/727/727275.png
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter8.jpg https://images.unsplash.com/photo-1482358625854-d7d631ba1858?ixlib=rb-1.2.1&ixid=eyJhcHBfaWQiOjEyMDd9&auto=format&fit=crop&w=1050&q=80
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter9.jpg http://www.wallpaperstop.com/wallpapers/nature-wallpapers/winter-wallpapers/winter-candle-1920x1200-164125.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter10.jpg http://wallpaperswide.com/download/winter_cat-wallpaper-1920x1080.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter11.jpg https://images4.alphacoders.com/184/thumb-1920-184306.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter12.jpg https://images5.alphacoders.com/354/thumb-1920-354500.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter13.jpg https://latestwallpapershd.com/wp-content/uploads/2019/03/Beautiful-winter-wallpaper-HD.jpg
wget --timeout=1 --waitretry=0 --tries=5 --retry-connrefused -O $homedir/Pictures/winter14.jpg https://animal-wallpaper.com/wallpaper/winter-wallpaper-desktop-background-For-Background-HD-Wallpaper.jpg


# Install a decent browser
function_apt_wait_for_unlock ./Chrome_Setup.sh

# Install pandoc
wget https://github.com/jgm/pandoc/releases/download/2.11.2/pandoc-2.11.2-linux-amd64.tar.gz
# strip-componenets means instead of /usr/local/pandoc/bin/* and /usr/local/pandoc/share/* will
# infact and instead land in /usr/local/bin and /usr/local/share
sudo tar xvzf pandoc-2.11.2-linux-amd64.tar.gz --strip-components 1 -C /usr/local/
rm pandoc-2.11.2-linux-amd64.tar.gz

# install diagrams.net
# Version matters and must be 14.1.8
#sudo snap install drawio
# Cant snap install anymore as version 15 breaks everything and this was all
# based around version 14.1.8 so we need to manually install that now
wget https://github.com/jgraph/drawio-desktop/releases/download/v14.1.8/draw.io-amd64-14.1.8.deb
sudo dpkg -i draw.io-amd64-14.1.8.deb
rm draw.io-amd64-14.1.8.deb

# Install Libre office
function_apt_wait_for_unlock sudo apt-get install -y libreoffice libreoffice-numbertext libreoffice-ogltrans libreoffice-writer2latex libreoffice-writer2xhtml

# Install a rastered image editor since all you have is vector editors currently
function_apt_wait_for_unlock sudo apt-get install -y pinta


# Get Data Grabbing Tools
wget https://www.digitizeit.de/downloads/DigitizeIt_unix.zip
unzip DigitizeIt_unix.zip -x __*
chmod ugo+x DigitizeIt_unix/DigitizeIt.jar
sudo mv DigitizeIt_unix /opt/digitizelt
sudo rm DigitizeIt_unix.zip

wget https://www.datathief.org/Datathief.jar
chmod ugo+x Datathief.jar
sudo mkdir -p /opt/datathiefIII
sudo mv Datathief.jar /opt/datathiefIII/

# Add Data Grabbing Tools To Path
echo '' >> $homedir/.bashrc
echo '# Adding path for Data Grabbing Tools' >> $homedir/.bashrc
echo 'export PATH="/opt/datathiefIII:/opt/digitizelt:$PATH"' >> $homedir/.bashrc

#Do now so you can finish setup without leaving the script
export PATH=/opt/datathiefIII:/opt/digitizelt:$PATH


# Install gnuplot
function_apt_wait_for_unlock sudo apt-get install -y gnuplot

# Get sshfs for remote development using local system (mounts remotes filesystem locally)
function_apt_wait_for_unlock sudo apt-get install -y sshfs

# Make it so ImageMagic Can turn pdf and PS files into png files
IMGMGK_FULLPATH=$(sudo find /etc/ -name 'ImageMagick*')
IMGMGK_POLICY_FULLFILE="$IMGMGK_FULLPATH/policy.xml"
if [ -f "$IMGMGK_POLICY_FULLFILE" ];
then
   sudo sed -i 's@<policy domain="coder" rights="none" pattern="PS" />@<policy domain="coder" rights="read | write" pattern="PS" />@' $IMGMGK_POLICY_FULLFILE
   sudo sed -i 's@<policy domain="coder" rights="none" pattern="PDF" />@<policy domain="coder" rights="read | write" pattern="PDF" />@' $IMGMGK_POLICY_FULLFILE
fi


# Launch i3 on startup
echo '' >> $homedir/.bashrc
echo '# Adding i3 autolaunch' >> $homedir/.bashrc
echo 'if [ -z "${DISPLAY}" ] && [ "${XDG_VTNR}" -eq 1 ]; then' >> $homedir/.bashrc
echo '   startx' >> $homedir/.bashrc
echo 'fi' >> $homedir/.bashrc


# Install gsl
function_apt_wait_for_unlock ./install_gsl.sh

# Install doxygen
function_apt_wait_for_unlock ./install_doxygen.sh

# Install mermaid and mermaid-cli for system and mermaid-filter for pandoc
# Need to get newer version of Node.js first
function_apt_wait_for_unlock ./setup_nodejs.sh
function_apt_wait_for_unlock sudo npm install -g npm@8.1.1 to update
function_apt_wait_for_unlock sudo npm install -g mermaid
function_apt_wait_for_unlock sudo npm install -g mermaid-filter
function_apt_wait_for_unlock sudo npm install -g mermaid-cli
function_apt_wait_for_unlock sudo npm install -g mermaid.cli


# Dont let there be swap, ever
sudo swapoff -a

# Install HPH Drivers
#Dont do this one as it is useless
#function_apt_wait_for_unlock ./update-hid-ft260.sh

source $homedir/.bashrc
