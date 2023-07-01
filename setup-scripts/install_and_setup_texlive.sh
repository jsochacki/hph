#!/bin/bash

# Go get a working version of texlive before installing latexmk
# as the apt repo version doesn't have a functional tlmgr
wget -O ./tlinstall.tar.gz http://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz
tar -zxvf tlinstall.tar.gz

#Versions get changed all the time so hack get actual value
for entry in "./"*
do
  if [[ $entry == ./install-tl* ]];
  then
    TMPDIR="${entry:2}"
    echo "${entry:2}"
  fi
done

cd $TMPDIR

sed -e 's/\s*\([\+0-9a-zA-Z]*\).*/\1/' << EOF | sudo ./install-tl
  S # set installation scheme: scheme-basic
  d # basic scheme (plain and latex)
  R # Return to main menue
  C # Manually jump to collection addition
  b # BibTeX additional styles
  e # Additional fonts
  f # Recommended fonts
  j # Humanities packages
  p # US and UK English
  E # LaTeX additional packages
  F # LaTeX recommended packages
  H # MetaPost and Metafint packages
  I # Music packages
  J # Fraphics, pictures, diagrams
  K # Plain (La)TeX packages
  M # Publisher styles, theses, etc.
  P # Mathematics, natural sciences, computer science packages
  R # Return to main menue
  O # Set Options
  P # use letter size instead of A4 by default
  L # create symlinks in standard directories:
    # binaries to: /usr/local/bin
    # manpages to: /usr/local/man
    # info to: /usr/local/info
  R # Return to main menue
  I # Install the software
EOF

cd ../
sudo rm -rf $TMPDIR
sudo rm tlinstall.tar.gz
