#!/bin/sh

./warpblend 7 "../data/nv_warpblend_grid_1.dat" --warp --unblend
sleep 1
./warpblend 2 "../data/nv_warpblend_grid_2.dat" --warp --unblend
sleep 1
./warpblend 0 "../data/nv_warpblend_grid_3.dat" --warp --unblend

