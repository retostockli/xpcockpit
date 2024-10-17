#!/bin/sh
# Transforms .prf file from linux to windows style (monitor numbering)

for mon in 1 2 3; do
   sed -i "s/monitor\/${mon}\/m_monitor 0/monitor\/${mon}\/m_monitor ${mon}/g" X-Plane\ Window\ Positions.prf
done
