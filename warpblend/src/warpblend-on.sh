#!/bin/sh

./warpblend 5 "../data/nv_warpblend_grid_1.dat" --warp --blend
sleep 1
./warpblend 3 "../data/nv_warpblend_grid_2.dat" --warp --blend
sleep 1
./warpblend 1 "../data/nv_warpblend_grid_3.dat" --warp --blend

