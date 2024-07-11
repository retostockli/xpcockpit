#!/bin/sh

# Select user stockli upon first login

# set hostname in gui
# disable bluetooth
# disable camera
# enable ssh
# disable blanking


# sudo apt install mc
# edit /etc/dhcpcd.conf and add fixed IP address for eth0 and static router

# disable wlan and bluetooth
sudo rfkill block wlan0
sudo rfkill block bluetooth

# if hostname not set yet:
# edit /etc/hostname and add the name of the rasperry pi computer
# edit /etc/hosts and add the name of the rasperry pi computer to 127.0.1.1
# reboot in order networking changes to take effect

# if user was not chosen upon first install:
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

# open raspi-config
# Select Auto Desktop Login for User stockli
# select GL Driver

# enable boot messages:
# remove "quiet" from /boot/cmdline.txt

mkdir ~/.ssh

# copy ssh keys from mariachi
scp -p .ssh/id_ecdsa $raspiname:.ssh/
scp -p .ssh/id_ecdsa.pub $raspiname:.ssh/


cat ~/.ssh/id_ecdsa.pub > ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys


git config --global user.email "reto.stockli@gmail.com"
git config --global user.name "Reto Stockli"
git clone git@github.com:retostockli/xpcockpit.git xpcockpit
cd xpcockpit
git checkout B737

cd xpcockpit/xppi/raspian

# define no-desktop xsession
cp xsession ~/.xsession

# for fmc1 and fmc2
sudo cp 20-screen.conf /etc/X11/xorg.conf.d/

# for fmc1 and fmc2 you may also need to edit
/boot/config.txt
and set:
hdmi_group=1
hdmi_mode=1

# no password shutdown etc:
sudo cp 010_pi-nopasswd /etc/sudoers.d/


# copy systemd files
mkdir -p ~/.config/systemd/user
cp *.service ~/.config/systemd/user


sudo apt install x11-apps -y
sudo apt install emacs -y
sudo apt install libfltk1.3-dev -y
sudo apt install libftgl-dev -y
sudo apt install libfreetype6-dev -y
sudo apt install libudev-dev -y
sudo apt install build-essential -y
sudo apt install autoconf -y
# for DU1 and DU2:
mkdir -p ~/GLOBE
mkdir -p ~/GSHHG
mkdir -p ~/X-Plane\ 11/Custom\ Data
mkdir -p ~/X-Plane\ 11/Resources/default\ data
mkdir -p ~/X-Plane\ 11/Resources/default\ scenery/default\ apt\ dat/Earth\ nav\ data



# Update Rasperry OS
sudo apt update
sudo apt full-upgrade
sudo apt clean
