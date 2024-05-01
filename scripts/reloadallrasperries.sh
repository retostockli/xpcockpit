#!/bin/sh

# reloads systemd services on raspis

ssh du1  "systemctl --user daemon-reload; systemctl --user restart xpopengc-du1"
ssh du2  "systemctl --user daemon-reload; systemctl --user restart xpopengc-du2"
ssh eicas "systemctl --user daemon-reload; systemctl --user restart xpopengc-eicas; systemctl --user restart xppi-mip"
ssh fmc1 "systemctl --user daemon-reload; systemctl --user restart xpopengc-fmc1; systemctl --user restart xppi-fmc1"
ssh fmc2 "systemctl --user daemon-reload; systemctl --user restart xpopengc-fmc2; systemctl --user restart xppi-fmc2"
