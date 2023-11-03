#!/bin/sh

./warpblend 7 1 --unwarp --unblend
sleep 1
./warpblend 2 2 --unwarp --unblend
sleep 1
./warpblend 0 3 --unwarp --unblend

