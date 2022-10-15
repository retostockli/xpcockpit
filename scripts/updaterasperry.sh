#!/bin/sh

# Updates XPCOCKPIT installation on rasperrys
# First argument specifies hostname of respective rasperry

ssh $1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
if [ $? != 0 ]
then
    echo "error in updating $1"
    exit 1
fi
