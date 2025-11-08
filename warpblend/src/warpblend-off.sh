#!/bin/sh

./warpblend 7 --unwarp --unblend
sleep 1
./warpblend 2 --unwarp --unblend
sleep 1
./warpblend 0 --unwarp --unblend

