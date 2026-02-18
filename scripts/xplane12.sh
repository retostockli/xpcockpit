#!/bin/sh
cd ~/X-Plane\ 12/Output/preferences
rm X-Plane\ Window\ Positions.prf
cd ~/X-Plane\ 12; ./X-Plane-x86_64 &
#cd ~/X-Plane\ 12; ./X-Plane-x86_64 --safe_mode=UI &

numscreens=1
count=0
while [  $count -lt $numscreens ]; do
 sleep 1
 count=$( wmctrl -l | grep X-Plane | grep -v $USER | grep -v Firefox | wc -l)
 echo "Number of Screens $count of $numscreens"
done

id=`wmctrl -l | grep X-Plane | grep -v $USER | awk '{ print $1 }'`
wmctrl -i -r "$id" -b add,fullscreen

