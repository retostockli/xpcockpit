#!/bin/sh

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

ping -c 1 192.168.1.53
if [ $? != 0 ]
then
    echo "Error connecting to PEDESTAL Card"
    exit 2
fi


cd ~/xpcockpit/xpsismo/bin
./xpsismo boeing737
