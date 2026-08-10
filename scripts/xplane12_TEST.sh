#!/bin/sh
cp ~/xpcockpit/warpblend/data/X-Plane\ Window\ Positions_TEST.prf ~/X-Plane\ 12/Output/preferences/X-Plane\ Window\ Positions.prf
cd ~/X-Plane\ 12; ./X-Plane-x86_64 --monitor_bounds=0,0,1920,1080,1920,0,1920,1080,3840,0,1920,1080,5760,0,1920,1080 &#cd ~/X-Plane\ 12; ./X-Plane-x86_64 --safe_mode=UI &
