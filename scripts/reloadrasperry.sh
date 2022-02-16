#!/bin/sh

# reloads systemd services on raspis

ssh cdu1  "systemctl --user restart xpopengc-cdu1"
ssh cdu2  "systemctl --user restart xpopengc-cdu2"
ssh eicas "systemctl --user restart xpopengc-eicas"
ssh fmc1 "systemctl --user restart xpopengc-fmc1; systemctl --user restart xppi-fmc1"
#ssh fmc2 "systemctl --user restart xpopengc-fmc2; systemctl --user restart xppi-fmc2"
