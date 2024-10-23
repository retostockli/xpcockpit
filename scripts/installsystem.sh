#sudo cp Documents/99-hidraw-permissions.rules /etc/udev/rules.d/
sudo cp Documents/99-hidraw-permissions.rules /usr/lib/udev/rules.d/
#sudo cp Documents/modprobe.conf /etc/modprobe.d/modprobe.conf
sudo apt install synaptic -y
sudo apt install build-essential
sudo apt install libopenal-dev -y
sudo apt install cmake -y
sudo apt install mc -y
sudo apt install tilix -y
sudo apt install keepassxc -y
sudo apt install pavucontrol -y
sudo apt install paprefs -y
sudo apt install git -y
sudo apt install automake -y
sudo apt install libudev-dev -y
sudo apt install libglu1-mesa-dev -y
sudo apt install libfltk1.3-dev -y
sudo apt install libftgl-dev -y
sudo apt install emacs -y
sudo apt install evolution -y
sudo apt install p7zip -y
sudo apt install imagemagick -y
sudo apt install libxnvctrl-dev
sudo apt install solaar -y
sudo apt install python3-numpy -y
sudo apt install python3-matplotlib -y
sudo apt install libfreeimage3 -7
sudo apt install libzip4 -y
sudo apt install libxcb-cursor-dev -y
sudo apt install libssl-dev -y

#sudo apt install libsecret-tools

gsettings set org.gnome.desktop.peripherals.mouse middle-click-emulation true

#secret-tool store --label='KeePassXC' 'keepass' 'default'
#https://github.com/keepassxreboot/keepassxc/issues/1267

#/etc/hosts:
# Flight Sim Hosts
192.168.1.10	yakari
192.168.1.51	mcp
192.168.1.52	mip
192.168.1.53	pedestal
192.168.1.61	fmc1
192.168.1.62	fmc2
192.168.1.63	du1
192.168.1.64	eicas
192.168.1.65	du2
192.168.1.90	raspberrypi


