#!/bin/bash

cp ~/xpcockpit/warpblend/data/X-Plane\ Window\ Positions_NOWARPBLEND.prf ~/X-Plane\ 12/Output/preferences/X-Plane\ Window\ Positions.prf

##start xplane##
# Medium Rendering Load
#~/X-Plane\ 12/X-Plane-x86_64 --fps_test=33 --verbose --load_smo=Output/replays/fps_test_eddf.fps --monitor_bounds=0,0,1920,1080,1920,0,1920,1080,3840,0,1920,1080,5760,0,1920,1080 &
# Heavy Rendering Load
#~/X-Plane\ 12/X-Plane-x86_64 --fps_test=35 --verbose --load_smo=Output/replays/fps_test_eddf.fps &

~/X-Plane\ 12/X-Plane-x86_64
