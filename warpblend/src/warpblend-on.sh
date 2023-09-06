#!/bin/sh

./warpblend 7 1 --warp --blend
sleep 1
./warpblend 2 2 --warp --blend
sleep 1
./warpblend 0 3 --warp --blend

