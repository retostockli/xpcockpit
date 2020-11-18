#!/bin/sh
# Create a Windows ZIP file for distribution
# Include for now:
# xpserver plugin 
# opengc executable and shared files

zipdir=xpcockpit-windows
zipfile=${zipdir}.zip


mkdir ${zipdir}
mkdir ${zipdir}/xpserver/
mkdir ${zipdir}/xpopengc/

cp -a xpserver/plugins ${zipdir}/xpserver/
cp -a xpopengc/bin ${zipdir}/xpopengc/
cp -a xpopengc/share ${zipdir}/xpopengc/

cp README_WINDOWS ${zipdir}
