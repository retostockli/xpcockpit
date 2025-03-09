#!/bin/sh

count=$(ps -a | grep xpsismo | grep -v xpsismo.sh | wc -l)

if [ $count = 0 ] ; then

ping -c 1 192.168.1.51
if [ $? != 0 ]
then
    echo "Error connecting to MCP Card"
    exit 2
fi


ping -c 1 192.168.1.52
if [ $? != 0 ]
then
    echo "Error connecting to MIP Card"
    exit 2
fi

ping -c 1 192.168.1.55
if [ $? != 0 ]
then
    echo "Error connecting to AFT OVERHEAD Card"
    exit 2
fi

trap - SIGINT SIGTERM

cd ~/xpcockpit/xpsismo/bin
./xpsismo boeing737

read -p "Press key to continue.. " -n1 -s

exit 0

else
 zenity --info --width=200 --height=50 --text="<big>xpsismo is already running</big>" --title="xpsismo Launcher"
 exit 1
fi

