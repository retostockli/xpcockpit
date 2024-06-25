#!/bin/sh

xterm -e "./updateraspberry.sh du1; $SHELL" &
sleep 2
xterm -e "./updateraspberry.sh du2; $SHELL" &
sleep 2
xterm -e "./updateraspberry.sh eicas; $SHELL" &
sleep 2
xterm -e "./updateraspberry.sh fmc1; $SHELL" &
sleep 2
xterm -e "./updateraspberry.sh fmc2; $SHELL" &
