#!/bin/sh

# stop systemd services on raspis

ssh du1  "systemctl --user stop xpopengc-du1"
ssh du2  "systemctl --user stop xpopengc-du2"
ssh eicas "systemctl --user stop xpopengc-eicas; systemctl --user stop  xppi-mip"
ssh fmc1 "systemctl --user stop xpopengc-fmc1; systemctl --user stop xppi-fmc1"
ssh fmc2 "systemctl --user stop xpopengc-fmc2; systemctl --user stop xppi-fmc2"
