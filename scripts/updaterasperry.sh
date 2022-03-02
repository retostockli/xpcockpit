#!/bin/sh

# Updates XPCOCKPIT installation on raspis

ssh du1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
ssh du2  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
ssh eicas "cd xpcockpit/scripts; ./updatexpcockpit.sh"
ssh fmc1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
#ssh fmc2  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
