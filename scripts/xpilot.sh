#!/bin/sh

cd ~/xPilot
./xPilot.AppImage

#./xPilot.AppImage &

#sleep 2

# get window id of xPilot
#id=$( wmctrl -l | grep xPilot | grep -v Tilix | awk '{print $1}')

#echo $id

#wmctrl -i -r $id -e 1,1601,0,700,1000
#wmctrl -i -r $id -t 1

