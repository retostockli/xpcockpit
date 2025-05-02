#!/bin/sh

count=$(ps -a | grep xpteensy | grep -v xpteensy.sh | wc -l)

if [ $count = 0 ] ; then

    cd ~/xpcockpit/xpteensy/bin
    ./xpteensy boeing737

    exit 0

else
    zenity --info --width=200 --height=50 --text="<big>xpteensy is already running</big>" --title="xpteensy Launcher"
    exit 1
fi




