#!/bin/sh

# update xpcockpit installation on rasperry

cd ~/xpcockpit
git pull origin B737
cd xpopengc
make 
make install
cd ..
cd xppi
make
make install

