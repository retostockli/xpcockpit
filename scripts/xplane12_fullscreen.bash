#!/bin/bash
###This script requires wmctrl, xdotool, grep, awk, and zenity###

numscreens=4
#count=$(wmctrl -lx | grep X-System | tee >(wc -l))
#if [ $count = 0 ] ; then
###Modify X-Plane path based on installation location###
###1st Restore preferences and set performance###
#nvidia-settings --load-config-only
#sudo cpupower frequency-set -g performance
#cp "X-Plane Window Positions.prf" "X-Plane 12 PROD/Output/preferences/X-Plane Window Positions.prf" 
#cp "X-Plane Window Positions_NEW.prf" "X-Plane 12/Output/preferences/X-Plane Window Positions.prf" 

##start xplane##
#./X-Plane\ 12/X-Plane-x86_64 &
# while [  $count -lt $numscreens ]; do
#  sleep 1
#  count=$( wmctrl -l | grep X-System | wc -l)
#  echo "Number of Screens $count of $numscreens"
# done
#else
# zenity --info --width=200 --height=50 --text="<big>X-Plane is already running</big>" --title="X-Plane Launcher"
# exit 1
#fi
#screencount=1
#wmctrl -l | grep X-System | awk '{ print $1 }' | while read; do
#  wmctrl -i -r "$REPLY" -T "X-System_$screencount"
#  let  screencount=screencount+1
#done
wmctrl -r "X-System_1" -e 1,0,0,1920,1080
if [ $numscreens -gt 1 ] ; then
    wmctrl -r "X-System_2" -e 1,1920,0,1920,1080
fi
if [ $numscreens -gt 2 ] ; then 
    wmctrl -r "X-System_3" -e 1,3840,0,1920,1080
fi
if [ $numscreens -gt 3 ] ; then
    wmctrl -r "X-System_4" -e 1,5760,0,1920,1080
fi

id=`wmctrl -l | grep X-System_1 | awk '{ print $1 }'`
wmctrl -i -r "$id" -b add,fullscreen
if [ $numscreens -gt 1 ] ; then
    id=`wmctrl -l | grep X-System_2 | awk '{ print $1 }'`
    wmctrl -i -r "$id" -b add,fullscreen
fi
if [ $numscreens -gt 2 ] ; then 
    id=`wmctrl -l | grep X-System_3 | awk '{ print $1 }'`
    wmctrl -i -r "$id" -b add,fullscreen
fi
if [ $numscreens -gt 3 ] ; then
    id=`wmctrl -l | grep X-System_4 | awk '{ print $1 }'`
    wmctrl -i -r "$id" -b add,fullscreen
fi

exit 0
