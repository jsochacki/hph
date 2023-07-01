#!/bin/bash

TMPDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd
homedir=$(pwd)
cd $TMPDIR

MNTDIR="/mnt/hgfs"
MNTFOLDER="git"

if [ ! -d "$MNTDIR" ]; then
   sudo mkdir -p "$MNTDIR"
fi

sudo vmhgfs-fuse .host:/ $MNTDIR -o subtype=vmhgfs-fuse,allow_other

echo "$MNTDIR/git"


echo '' >> $homedir/.bashrc
echo '# Aidding vmware shared folder mount at logon' >> $homedir/.bashrc
echo "if [ ! -d \"$MNTDIR/git\" ]; then" >> $homedir/.bashrc
echo "   sudo vmhgfs-fuse .host:/ $MNTDIR -o subtype=vmhgfs-fuse,allow_other" >> $homedir/.bashrc
echo 'fi' >> $homedir/.bashrc
