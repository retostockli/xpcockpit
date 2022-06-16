#!/bin/sh

# Updates XPCOCKPIT installation on raspis

ssh du1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
if [ $? != 0 ]
    echo "error in updating du1"
    exit 1
fi

ssh du2  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
if [ $? != 0 ]
    echo "error in updating du2"
    exit 2
fi

ssh eicas "cd xpcockpit/scripts; ./updatexpcockpit.sh"
if [ $? != 0 ]
    echo "error in updating eicas"
    exit 3
fi

ssh fmc1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
if [ $? != 0 ]
    echo "error in updating fmc1"
    exit 4
fi

#ssh fmc2  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
#if [ $? != 0 ]
#    echo "error in updating fmc2
#    exit 5
#fi
