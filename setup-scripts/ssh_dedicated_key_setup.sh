#!/bin/bash
# set -x
if [ "$#" -ne 5 ]; then
   echo "Usage: setup_ssh_key.sh <LOCAL_UNAME> <USRNAME_ON_HOST> <MACHINE_IP> <MACHINE_NICKNAME> <SSH_PORT>"
   echo "<MACHINE_NICKNAME> is literally whatever you want it to be but"
   echo "will be what you type after ssh, i.e. ssh <MACHINE_NICKNAME>"
   echo "and this will dealias to ssh $USRNAME@$MACHINE_IP -p <SSH_PORT>"
   echo "Unless this is a special case <SSH_PORT> should be 22"
   exit
fi

LOCAL_UNAME=$1
USRNAME_ON_HOST=$2
MACHINE_IP=$3
MACHINE_NICKNAME=$4
SSH_PORT=$5

mkdir -p ~/.ssh
sudo chown $LOCAL_UNAME ~/.ssh
sudo chgrp $LOCAL_UNAME ~/.ssh

echo "running keygen"

ssh-keygen -t rsa -b 4096 -f ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME

# Clugie way to do it, don't
# echo "adding alias to .bashrc"

# echo "" >> ~/.bashrc
# echo "Adding alias for ssh host $USRNAME_ON_HOST@$MACHINE_IP $MACHINE_NICKNAME" >> ~/.bashrc
# echo "alias $MACHINE_NICKNAME='$USRNAME_ON_HOST@$MACHINE_IP -p $SSH_PORT'" >> ~/.bashrc

# Proper way to do it
echo "adding Host to .ssh/config"

echo "Host $MACHINE_NICKNAME" >> ~/.ssh/config
echo "    HostName $MACHINE_IP" >> ~/.ssh/config
echo "    Port $SSH_PORT" >> ~/.ssh/config
echo "    User $USRNAME_ON_HOST" >> ~/.ssh/config
echo "    IdentityFile ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME" >> ~/.ssh/config

echo "adding IdentityFile ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME to authorized_keys on $MACHINE_NICKNAME"

sudo ssh-copy-id -i ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME.pub -p $SSH_PORT $USRNAME_ON_HOST@$MACHINE_IP

# If you don't do this programs wont be able to load the key
sudo chown $LOCAL_UNAME ~/.ssh/config ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME.pub
sudo chgrp $LOCAL_UNAME ~/.ssh/config ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME ~/.ssh/id_rsa-remote-ssh-$MACHINE_NICKNAME.pub

echo "Done"