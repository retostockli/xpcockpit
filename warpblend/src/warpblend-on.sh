#!/bin/sh

./warpblend 7 1 "X-Plane Window Positions.prf" --warp --blend
sleep 1
./warpblend 2 2 "X-Plane Window Positions.prf" --warp --blend
sleep 1
./warpblend 0 3 "X-Plane Window Positions.prf" --warp --blend

