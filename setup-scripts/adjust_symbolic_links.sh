#!/bin/bash


#Not safe
#homedir=$(eval echo ~$USER)
#Prevents errors due to sudo run setting USER as root
TMPDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd
homedir=$(pwd)
cd $TMPDIR



ln -sf $TMPDIR/update_tex_figures.sh $homedir/.local/bin/update_tex_figures.sh
ln -sf $TMPDIR/make_gnuplots.sh $homedir/.local/bin/make_gnuplots.sh
ln -sf $TMPDIR/run_pandoc_commands.sh $homedir/.local/bin/run_pandoc_commands.sh
ln -sf $TMPDIR/run_doxygen_commands.sh $homedir/.local/bin/run_doxygen_commands.sh
ln -sf $TMPDIR/run_cmake_commands.sh $homedir/.local/bin/run_cmake_commands.sh

ln -sf $TMPDIR/.vimrc.plugged $homedir/
ln -sf $TMPDIR/.vimrc.normal $homedir/
ln -sf $TMPDIR/.vimrc.colors $homedir/
ln -sf $TMPDIR/.vimrc.custom $homedir/
ln -sf $TMPDIR/.vimrc $homedir/
mkdir -p $homedir/.vim/spell
ln -sf $TMPDIR/en.utf-8.add $homedir/.vim/spell/


mkdir -p  $homedir/.vim/ftplugin/
ln -sf $TMPDIR/c.vim $homedir/.vim/ftplugin/
ln -sf $TMPDIR/python.vim $homedir/.vim/ftplugin/
ln -sf $TMPDIR/tex.vim $homedir/.vim/ftplugin/
ln -sf $TMPDIR/markdown.vim $homedir/.vim/ftplugin/
ln -sf $TMPDIR/matlab.vim $homedir/.vim/ftplugin/

mkdir -p $homedir/.vim/UltiSnips
ln -sf $TMPDIR/tex.snippets $homedir/.vim/UltiSnips/
ln -sf $TMPDIR/markdown.snippets $homedir/.vim/UltiSnips/
ln -sf $TMPDIR/cpp.snippets $homedir/.vim/UltiSnips/
ln -sf $TMPDIR/c.snippets $homedir/.vim/UltiSnips/
ln -sf $TMPDIR/cu.snippets $homedir/.vim/UltiSnips/
ln -sf $TMPDIR/matlab.snippets $homedir/.vim/UltiSnips/


mkdir -p $homedir/.vim/syntax
ln -sf $TMPDIR/mymatlab.vim $homedir/.vim/syntax/

# Copy over scripts and config files
mkdir -p $homedir/.local/bin
ln -sf $TMPDIR/myclang-format.py3 $homedir/.local/bin/clang-format.py
ln -sf $TMPDIR/.clang-format $homedir/
#cp myclang-format.py3 $homedir/.local/bin/clang-format.py
#cp .clang-format $homedir/
mkdir -p $homedir/.config/zathura/
ln -sf $TMPDIR/zathurarc $homedir/.config/zathura/
#cp zathurarc $homedir/.config/zathura/
mkdir -p $homedir/.config/i3/
ln -sf $TMPDIR/i3config $homedir/.config/i3/config
ln -sf $TMPDIR/.Xresources $homedir/
#cp i3config $homedir/.config/i3/config
#cp .Xresources $homedir/
mkdir -p $homedir/.cfiles
ln -sf $TMPDIR/c.ycm_extra_conf.py $homedir/.cfiles/.ycm_extra_conf.py
#cp c.ycm_extra_conf.py $homedir/.cfiles/.ycm_extra_conf.py
mkdir -p $homedir/.pyfiles
ln -sf $TMPDIR/py.ycm_extra_conf.py $homedir/.pyfiles/.ycm_extra_conf.py
#cp py.ycm_extra_conf.py $homedir/.pyfiles/.ycm_extra_conf.py
# Dont actually need to do this below to get urxvt settings to take
#echo 'xrdb $homedir/.Xresources' >> $homedir/.xinitrc
mkdir -p $homedir/.vimfiles

# Setup auto google
ln -sf $TMPDIR/Googleit.py $homedir/.cfiles/
#cp Googleit.py $homedir/.cfiles/
chmod guo+x $homedir/.cfiles/Googleit.py
ln -sf $homedir/.cfiles/Googleit.py $homedir/.pyfiles/
ln -sf $homedir/.cfiles/Googleit.py $homedir/.vimfiles/
#cp $homedir/.cfiles/Googleit.py $homedir/.pyfiles/
#cp $homedir/.cfiles/Googleit.py $homedir/.vimfiles/


