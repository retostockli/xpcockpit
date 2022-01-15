2019/12/17 Reto Stockli

This is OpenGC adapted for X-Plane by use of a new TCP/IP data plugin between X-Plane
and OpenGC called XPSERVER. The plugin allows to transfer X-Plane internal and plane
specific data. The code has been tested on OSX 10.6 and on Ubuntu 18.04

The OpenGC code is based on OpenGC Version 0.55 and 0.57 by D. Shelton (http://opengc.sourceforge.net)
and is distributed under the GPL framework with limitations as described in Copyright.txt

I have removed the cmake dependency and implemented a standard gnu configure/autoconf/automake toolset.
Also, several of the codes were cleaned from warnings and the xplane datasource was completely remodeled,
plus all other data sources were turned off. They can be turned on with some modifications to their code,
so that they are in line again with the current state of the OpenGC framework. Windows compilation support
was removed (who cares about windows anyway ...).

Installation Dependencies
- gcc/g++ compiler
- gnu Autotools / Autoconf
- FTGL library (=2.1.2 stable release, 2.1.3 and above does not work yet)
- FLTK library (>=1.1.7)
- Freetype library (>=2.1.9)
- X11R6 Framework
- OpenGL library (GL and GLU)

Installation How-to
- resolve above depencies, for debian/ubuntu:

sudo apt install libfltk1.3-dev -y
sudo apt install libftgl-dev -y
sudo apt install libfreetype6-dev -y
sudo apt install build-essential -y
sudo apt install autoconf -y


In case you cloned from git, please first run these in order to complete automake/autoconf file structure:
autoreconf
automake --add-missing
autoreconf

Then type:
./configure 
make

This should build the application xpopengc in src/main.

Since either of the FTGL, FLTK, Plib, Freetype libraries might be located in a custom location, type ./configure --help
to see the options to customize the paths to those libraries during configuration. The freetype library somehow requires
to specify its prefix on Ubuntu/Debian: so try:

./configure --with-freetype=/usr

Run How-to:
-----------
edit the file inidata/default.ini and change the following items according to your OpenGC/X-Plane configuration:

cd src/main
./xpopengc default

It should start a X window with a default gauge and tell you on the command line that it expects a X-Plane connection

Start X-Plane with the xpserver plugin. You should see OpenGC display content changing according
To your flight data. The current code is in development, any updates welcome.

EGPWS
-----

If you want Terrain in the NAV display of the B737, please download the 16 DEM Tiles from the GLOBE Dataset:
https://www.ngdc.noaa.gov/mgg/topo/gltiles.html

and set DEMPath under "general" in the ini file to the directory where the GLOBE files are stored.

If you want correct shorelines and lakes on top of that, please download the
Global Self-consistent, Hierarchical, High-resolution Geography Database (GSHHG)
from NOAA (download and unzip gshhg-bin-X.Y.Z.zip):
https://www.ngdc.noaa.gov/mgg/shorelines/data/gshhg/latest/

and set GSHHGPath under "general" in the ini file to the directory where GSHHG files are stored




Good flight,
Reto Stockli