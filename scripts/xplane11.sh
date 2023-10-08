#!/bin/sh

#xterm -geometry 80x24+1500+500 -e "cd ~/X-Plane\ 11;./X-Plane-x86_64;"
#xterm -geometry 80x24+1500+500 -e "cd ~/X-Plane\ 11;./X-Plane-x86_64 ‐‐pref:_draw_volume_fog01=0;"
#xterm -geometry 80x24+1500+500 -e "cd ~/X-Plane\ 11.26;./X-Plane-x86_64 ‐‐pref:_draw_volume_fog01=0;"
#xterm -geometry 80x24+1500+500 -e "cd ~/X-Plane\ 11;./X-Plane-x86_64 --fps_test=1"
#cd ~/X-Plane\ 11; ./X-Plane-x86_64 --monitor_bounds=0,0,1920,1080
cd ~/X-Plane\ 11/Output/preferences
rm X-Plane\ Window\ Positions.prf
cd ~/X-Plane\ 11; ./X-Plane-x86_64