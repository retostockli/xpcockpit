#!/bin/sh

# Select user stockli upon first login

# set hostname in gui
# disable bluetooth
# disable camera
# enable ssh
# disable blanking

# find your Raspberry inthe network
sudo nmap -p 22 192.168.1.0/24

# if your eth0 is now called enx####, then add:
# net.ifnames=0 biosdevname=0 to /boot/cmdline.txt

# sudo apt install mc
# edit /etc/dhcpcd.conf and add fixed IP address for eth0 and static router

# New raspbian os bookworm dhcp & ip management with nmcli
# https://gist.github.com/hivian/590b44885940aa927e3bfcd388615a49
sudo nmcli -p connection show
sudo nmcli c mod "Wired connection 1" ipv4.addresses 192.168.1.90/24 ipv4.method manual
sudo nmcli con mod "Wired connection 1" ipv4.gateway 192.168.1.1
sudo nmcli con mod "Wired connection 1" ipv4.dns 192.168.1.1
sudo nmcli c down "Wired connection 1"
sudo nmcli c up "Wired connection 1"

# disable wlan
sudo rfkill block wlan0
sudo rfkill block wifi
# with bookworm, use:
sudo nmcli radio wifi off
# disable bluetooth
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

# WiringPi is alive again
https://github.com/WiringPi/WiringPi/releases

However, xppi uses pigpio first if available, so you need to remove it:
Remove pigpio and pigpiod
remove unused packages (will remove pigpio header and library etc.)

# Update Rasperry OS to Bullseye
sudo apt update
sudo apt full-upgrade
sudo reboot
sudo mcedit /etc/apt/sources.list (replace buster with bullseye)
sudo apt update
sudo apt install gcc-8-base (fixes error in dist-upgrade)
sudo apt dist-upgrade
sudo apt autoclean
sudo reboot

