#!/bin/sh
# Transforms prf file from windows to linux (monitor numbering)

for mon in 1 2 3; do
   sed -i "s/monitor\/${mon}\/m_monitor ${mon}/monitor\/${mon}\/m_monitor 0/g" X-Plane\ Window\ Positions.prf
done
