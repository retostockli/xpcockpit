#!/bin/sh
# Code which scans a network range for a specific open UDP port
sudo nmap -sU -p 1024 192.168.1.2-255
