#!/bin/sh

raspi="cdu1"

rsync -av ~/GLOBE/* ${raspi}:GLOBE/
rsync -av ~/GSHHG/* ${raspi}:GSHHG/

rsync -av ~/X-Plane\ 11/Custom\ Data/earth_fix.dat "${raspi}:X-Plane\ 11/Custom\ Data/"
rsync -av ~/X-Plane\ 11/Custom\ Data/earth_nav.dat "${raspi}:X-Plane\ 11/Custom\ Data/"
rsync -av ~/X-Plane\ 11/Resources/default\ data/earth_fix.dat "${raspi}:X-Plane\ 11/Resources/default\ data/"
rsync -av ~/X-Plane\ 11/Resources/default\ data/earth_nav.dat "${raspi}:X-Plane\ 11/Resources/default\ data/"
rsync -av ~/X-Plane\ 11/Resources/default\ scenery/default\ apt\ dat/Earth\ nav\ data/apt.dat "${raspi}:X-Plane\ 11/Resources/default\ scenery/default\ apt\ dat/Earth\ nav\ data/"
