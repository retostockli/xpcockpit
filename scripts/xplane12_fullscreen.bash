#!/bin/bash
###This script requires wmctrl, xdotool, grep, awk, and zenity###

numscreens=4

wmctrl -r "X-Plane_1" -e 1,0,0,1920,1080
if [ $numscreens -gt 1 ] ; then
    wmctrl -r "X-Plane_2" -e 1,1920,0,1920,1080
fi
if [ $numscreens -gt 2 ] ; then 
    wmctrl -r "X-Plane_3" -e 1,3840,0,1920,1080
fi
if [ $numscreens -gt 3 ] ; then
    wmctrl -r "X-Plane_4" -e 1,5760,0,1920,1080
fi

id=`wmctrl -l | grep X-Plane_1 | awk '{ print $1 }'`
wmctrl -i -r "$id" -b add,fullscreen
if [ $numscreens -gt 1 ] ; then
    id=`wmctrl -l | grep X-Plane_2 | awk '{ print $1 }'`
    wmctrl -i -r "$id" -b add,fullscreen
fi
if [ $numscreens -gt 2 ] ; then 
    id=`wmctrl -l | grep X-Plane_3 | awk '{ print $1 }'`
    wmctrl -i -r "$id" -b add,fullscreen
fi
if [ $numscreens -gt 3 ] ; then
    id=`wmctrl -l | grep X-Plane_4 | awk '{ print $1 }'`
    wmctrl -i -r "$id" -b add,fullscreen
fi

exit 0
