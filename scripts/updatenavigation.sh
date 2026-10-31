#!/bin/sh

# Move NAVIGRAPH FMS DATA to X-Plane and unzip it there

DLDIR="$HOME/Downloads"
XPDIR="$HOME/X-Plane 12/Custom Data"

cd $DLDIR

files=`ls xplane12-native-*.zip`

for file in $files
do
    echo Move and unzip $DLDIR/$file to $XPDIR
    mv "$DLDIR/$file" "$XPDIR"
    cd "$XPDIR"
    unzip -o $file
done
