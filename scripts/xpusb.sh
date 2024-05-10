#!/bin/sh

count=$(ps -a | grep xpusb | grep -v xpusb.sh | wc -l)

if [ $count = 0 ] ; then

    cd ~/xpcockpit/xpusb/bin
    ./xpusb boeing737

    exit 0

else
    zenity --info --width=200 --height=50 --text="<big>xpusb is already running</big>" --title="xpusb Launcher"
    exit 1
fi




