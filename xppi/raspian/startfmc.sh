#!/bin/sh

cd ~/xpcockpit/xppi/src
./xppi boeing737fmc &
cd ~/xpcockpit/xpopengc/src/main
./xpopengc boeing737fmc &
