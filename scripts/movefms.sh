#!/bin/sh

# Move FMS plans from Downloads Directory to X-Plane FMS Directory

DLDIR="$HOME/Downloads"
XPDIR="$HOME/X-Plane 12/Output/FMS plans"

cd $DLDIR

files=`ls *.fms`

for file in $files
do
    outfile=`echo $file | cut -c1-8`.fms
    echo Move $DLDIR/$file $XPDIR/$outfile
    mv "$DLDIR/$file" "$XPDIR/$outfile"
done
