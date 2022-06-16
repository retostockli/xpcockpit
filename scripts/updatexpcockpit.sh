#!/bin/sh

# update xpcockpit installation on rasperry

cd ~/xpcockpit
git pull origin B737
if [ $? != 0 ]
then
    echo "error in pulling git branch"
    exit 1
fi
cd xpopengc
make 
if [ $? != 0 ]
then
    echo "error in make xpopengc"
    exit 2
fi
make install
if [ $? != 0 ]
then
    echo "error in make install xpopengc"
    exit 3
fi
cd ..
cd xppi
make
if [ $? != 0 ]
then
    echo "error in make xppi"
    exit 4
fi
make install
if [ $? != 0 ]
then
    echo "error in make install xppi"
    exit 5
fi
cd raspian
cp *.service ~/.config/systemd/user/
if [ $? != 0 ]
then
    echo "error in copying systemd scripts"
    exit 6
fi


