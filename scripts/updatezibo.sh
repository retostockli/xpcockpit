#!/bin/sh

# Move ZIBO Update to the Aircraft folder, and unzips it over full install

DLDIR="$HOME/Downloads"
XPDIR="$HOME/Aircraft/ZIBO/B737-800X"

PREFIX="B738X"
SUFFIX="zip"

cd $DLDIR

files=`ls $PREFIX*.$SUFFIX`

for file in $files
do
    MAJOR=`echo $file | cut -d '_' -f 2`
    MINOR=`echo $file | cut -d '_' -f 3`
    REVISION=`echo $file | cut -d '_' -f 4 | cut -d '.' -f 1`
    
    echo Found ZIBO with Version: $MAJOR Minor: $MINOR Revision: $REVISION
    mv $DLDIR/$file $XPDIR-$MAJOR.$MINOR/
    cd $XPDIR-$MAJOR.$MINOR
    chmod -R u+w *
    unzip -o $file
    chmod -R u+w *
done

cd $DLDIR