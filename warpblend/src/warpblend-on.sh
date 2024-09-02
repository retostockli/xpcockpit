#!/bin/sh

./warpblend 7 1 "../data/X-Plane Window Positions.prf" --warp --blend
sleep 1
./warpblend 2 2 "../data/X-Plane Window Positions.prf" --warp --blend
sleep 1
./warpblend 0 3 "../data/X-Plane Window Positions.prf" --warp --blend

