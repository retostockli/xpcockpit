2020/05/10 Reto Stockli and Hans Jansen

This file documents changes since XPSERVER Version 0.1

Version 0.62
------------
- Adding Windows support
- Removing 32 bit support

Version 0.61
------------
- changed timout allocation

Version 0.51
------------
- allow for multi data type datarefs (extend the FLT+DBL fix to any possible)

Version 0.50
------------
- adapted documentation
- checked packaging

Version 0.48
------------
- modified build system so that subdir-objects warning disappear
- mac.xpl is no longer a fat binary. 32 and 64 bit plugin versions
  all are in their respective 32/ and 64/ subdirs like in Linux

Version 0.47
------------
- corrected bug with partial TCP/IP sends/receives

Version 0.46
------------
- added checking for # of elements when retrieving from dataref

Version 0.46
------------
- renamed to xpserver (and built a sample client named xpclient)
  sorry for that renaming ...

Version 0.45
------------
- automake now builds fat plugin for mac and 32/64 bit plugin for linux
- changes in protocol and error handling

Version 0.44
------------
- added two menu items: "About" and "Client" information
- added transfer of client Package Name (taken from client's config.h)
  with each dataref that is initialized (20 bytes at the end of the transfer)

Version 0.43 (hans)
------------
- introduced handling of X-Plane commands
- corrected protocol error in network communication
- included the latest X-Plane SDK (2.12)
- reformatted for use as FAT plugin
- included the "dist" structure for plugin distribution
- added "dist" to Makefile.am (Reto)

Version 0.42
------------
- added option to link several arrays of same dataref with different array elements
- modified verbosity

Version 0.41
------------
- corrected handling of precision filtering for float values

Version 0.40
------------
- new protocol allowing the transfer of all X-Plane datatypes
- datarefs are no longer defined by ascii file
- better handlling of client terminations
- dynamic handling of client-requested datarefs and memory allocation

Version 0.37
------------
- renamed xpiocards in xpconnect, since xpiocards is the name of the full 
project while xpconnect is a subproject (the x-plane plugin)

Version 0.36
------------
- removed bug in precision calculation

Version 0.35
------------
- removed libtool again (very hard to make portable for plugin compilation)
- instead compiling with regular linking a shared library
- tested on OSX, yet to be tested on Linux

Version 0.34
------------
- cleaned up code headers and dependencies
- now using gnu autotools, especially libtool, to build the plugin

Version 0.32
------------
- modified several include statements for better compatibility with data types (specifically: uint_32 and fd_set)

Version 0.3
------------
- Modified x737 Datarefs so that they are compatible with Verison 4.2 of
  Benedikt Stratmanns Boeing 737 Plugin.
- Tested with X-Plane 9.60 on OSX

Version 0.12
------------
xpiocards_x737.dat and xpiocards.dat now contain a precision filter. Changed
datarefs are only sent to the client if they changed more than the precision
10^prec. Use prec=-99 for no precision check. The precision check ensures that
the TCP/IP communication is not overloaded by very small changes of flight data
(e.g. Exhaust Gas Temperature changing by 0.00024 Degrees Kelvin).
