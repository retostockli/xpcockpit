#!/bin/bash
###This script requires wmctrl, xdotool, grep, awk, and zenity###

numscreens=4
count=$(wmctrl -lx | grep X-Plane | grep -v $USER | grep -v Firefox | tee >(wc -l))
if [ $count = 0 ] ; then
###Modify X-Plane path based on installation location###
###1st Restore preferences and set performance###
#nvidia-settings --load-config-only
#sudo cpupower frequency-set -g performance
cp "X-Plane Window Positions_NOWARPBLEND.prf" "X-Plane 12/Output/preferences/X-Plane Window Positions.prf" 
#cp "X-Plane Window Positions_NOPROJECTION.prf" "X-Plane 12/Output/preferences/X-Plane Window Positions.prf" 
#cp "X-Plane Window Positions_NOBLEND.prf" "X-Plane 12/Output/preferences/X-Plane Window Positions.prf" 
#cp "X-Plane Window Positions.prf" "X-Plane 12/Output/preferences/X-Plane Window Positions.prf" 

##start xplane##
# Medium Rendering Load
#./X-Plane\ 12/X-Plane-x86_64 --fps_test=33 --verbose --load_smo=Output/replays/fps_test_eddf.fps &
# Heavy Rendering Load
./X-Plane\ 12/X-Plane-x86_64 --fps_test=35 --verbose --load_smo=Output/replays/fps_test_eddf.fps &
while [  $count -lt $numscreens ]; do
  sleep 1
  count=$( wmctrl -l | grep X-Plane | grep -v $USER | grep -v Firefox | wc -l)
  echo "Number of Screens $count of $numscreens"
 done
else
 zenity --info --width=200 --height=50 --text="<big>X-Plane is already running</big>" --title="X-Plane Launcher"
 exit 1
fi
screencount=1
wmctrl -l | grep X-Plane | grep -v $USER | grep -v Firefox | awk '{ print $1 }' | while read; do
  wmctrl -i -r "$REPLY" -T "X-Plane_$screencount"
  let  screencount=screencount+1
done
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
