#!/bin/sh

sleep 2

id=`xinput --list --id-only 'ILITEK ̚         EK-TI͌IT'`

if [ $id ] ; then

    echo "Touchscreen Device ID: $id"

#    xinput set-prop $id --type=float "Coordinate Transformation Matrix" 0 -1 1 -1 0 1 0 0 1
    xinput set-prop $id --type=float "Coordinate Transformation Matrix" 0 1 0 1 0 0 0 0 1
    xinput map-to-output $id HDMI-0

else
    echo "No Touchscreen found"

fi

sleep 5