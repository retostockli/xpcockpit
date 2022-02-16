#!/bin/sh

# Updates XPCOCKPIT installation on raspis

ssh cdu1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
ssh cdu2  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
ssh eicas "cd xpcockpit/scripts; ./updatexpcockpit.sh"
ssh fmc1  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
#ssh fmc2  "cd xpcockpit/scripts; ./updatexpcockpit.sh"
