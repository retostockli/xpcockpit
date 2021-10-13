#!/bin/sh

# Do add the user stockli first and add sudoers then logout and login as stockli
# sudo adduser stockli
# sudo usermod -aG sudo stockli
# sudo usermod -aG tty stockli
# sudo usermod -aG gpio stockli
# sudo usermod -aG adm stockli
# sudo usermod -aG dialout stockli
# sudo usermod -aG audio stockli
# sudo usermod -aG video stockli
# sudo usermod -aG plugdev stockli
# sudo usermod -aG input stockli
# sudo usermod -aG netdev stockli
# sudo usermod -aG lpadmin stockli

# edit /etc/hosts and add fixed host addresses
# edit /etc/dhcpcd.conf and add fixed IP address for wlan0 and static router

# copy ssh keys from mariachi
# scp -p mariachi:.ssh/id_ecdsa .ssh/
# scp -p mariachi:.ssh/id_ecdsa.pub .ssh/
# cat .ssh/id_ecdsa.pub > .ssh/authorized_keys
# chmod 600 .ssh/authorized_keys


sudo apt install mc -y
sudo apt install emacs -y
sudo apt install fltk1.3-dev -y
sudo apt install libftgl-dev -y
sudo apt install libfreetype6-dev -y
sudo apt install libudev-dev -y
sudo apt install build-essential -y
sudo apt install autoconf -y

git config --global user.email "reto.stockli@gmail.com"
git config --global user.name "Reto Stockli"
git clone git@github.com:retostockli/xpcockpit.git xpcockpit
