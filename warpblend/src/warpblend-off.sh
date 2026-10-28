#!/bin/sh

./warpblend 7 --unwarp --unblend
sleep 1
./warpblend 5 --unwarp --unblend
sleep 1
./warpblend 3 --unwarp --unblend
sleep 1
./warpblend 1 --unwarp --unblend

