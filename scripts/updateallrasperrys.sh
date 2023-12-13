#!/bin/sh

xterm -e "./updaterasperry.sh du1; $SHELL" &
sleep 2
xterm -e "./updaterasperry.sh du2; $SHELL" &
sleep 2
xterm -e "./updaterasperry.sh eicas; $SHELL" &
sleep 2
xterm -e "./updaterasperry.sh fmc1; $SHELL" &

