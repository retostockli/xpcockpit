#!/bin/sh

count=$(ps -a | grep xpleo | grep -v xpleo.sh | wc -l)

if [ $count = 0 ] ; then

    cd ~/xpcockpit/xpleo/bin
    ./xpleo boeing737

    exit 0

else
    zenity --info --width=200 --height=50 --text="<big>xpleo is already running</big>" --title="xpleo Launcher"
    exit 1
fi
