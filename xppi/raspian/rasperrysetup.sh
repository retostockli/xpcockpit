#!/bin/sh

# AS USER PI

# disable bluetooth
# disable camera
# enable ssh

# sudo apt install mc
# edit /etc/dhcpcd.conf and add fixed IP address for wlan0 and static router
# edit /etc/hostname and add the name of the rasperry pi computer
# edit /etc/hosts and add the name of the rasperry pi computer to 127.0.1.1
# sudo /etc/init.d/networking restart

# sudo adduser stockli
# Do add the user stockli first and add sudoers then logout and login as stockli
sudo usermod -aG sudo stockli
sudo usermod -aG tty stockli
sudo usermod -aG gpio stockli
sudo usermod -aG adm stockli
sudo usermod -aG dialout stockli
sudo usermod -aG audio stockli
sudo usermod -aG video stockli
sudo usermod -aG plugdev stockli
sudo usermod -aG input stockli
sudo usermod -aG netdev stockli
sudo usermod -aG lpadmin stockli

# AS USER STOCKLI

# copy ssh keys from mariachi
# scp -p .ssh/id_ecdsa $raspiname:.ssh/

# scp -p .ssh/id_ecdsa.pub $raspiname:.ssh/

# REBOOT and LOGIN as STOCKLI


#git config --global user.email "reto.stockli@gmail.com"
#git config --global user.name "Reto Stockli"
#git clone git@github.com:retostockli/xpcockpit.git xpcockpit


cat ~/.ssh/id_ecdsa.pub > ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys

#


sudo apt install mc -y
sudo apt install emacs -y
sudo apt install libfltk1.3-dev -y
sudo apt install libftgl-dev -y
sudo apt install libfreetype6-dev -y
sudo apt install libudev-dev -y
sudo apt install build-essential -y
sudo apt install autoconf -y

mkdir -p ~/GLOBE
mkdir -p ~/GSHHG
mkdir -p ~/X-Plane\ 11/Custom\ Data
mkdir -p ~/X-Plane\ 11/Resources/default\ data
mkdir -p ~/X-Plane\ 11/Resources/default\ scenery/default\ apt\ dat/Earth\ nav\ data

