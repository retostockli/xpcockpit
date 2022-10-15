#!/bin/sh

xterm -e "./updaterasperry.sh du1; $SHELL"
xterm -e "./updaterasperry.sh du2; $SHELL"
xterm -e "./updaterasperry.sh eicas; $SHELL"
xterm -e "./updaterasperry.sh fmc1; $SHELL"

