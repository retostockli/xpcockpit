2020/05/03 Reto Stockli / Hans Jansen

This file documents changes since Version 0.60

Version 0.81
------------
- much faster link_dataref commands

Version 0.80
------------
- corrected handling of abrupt X-Plane disconnect. Upon X-Plane restart all datarefs are subscribed again now
- moved to github
- added experimental WXR data reception in B737 NAV display
- added support for ZIBO MOD 737 in addition to x737
- bug fixes for space shuttle re-entry with very large elevation numbers in PFD

Version 0.77
------------
- moved ini files to share/opengc/ during installation
- added B737 FMC first version
- added B737 MIP Annunciators

Version 0.76
------------
- removed ogcNavRadioDataContainer.h
- removed all dependencies on FS/FG
- removed all dependencies on airframecontainer etc.

Version 0.75
------------
- removed the Rotation parameter from .ini file and code - was not really a good solution
- added "general:Name" parameter to .ini file and code - clientname for xpserver plugin
- added a bunch of Airbus A320 instruments
- cleaned up some of the code
- included missing headers in package

Version 0.74
------------
- corrected bug with partial TCP/IP sends/receives

Version 0.73
------------
- updated to new xpserver protocol

Version 0.72
------------
- added transfer of client's PACKAGE_NAME (taken from config.h) with each
  dataref that is initialized
  
Version 0.71
------------
- added option to link several arrays of same dataref with different array elements
- modified verbosity

Version 0.70
------------
- implemented new xpconnect protocol allowing for transfer of all X-Plane data types including full arrays
- fixed a few bugs with precision control

Version 0.66
------------
- adapted for new xpconnect.xpl plugin (renamed from xpiocards.xpl plugin)

Version 0.63
------------
- included FG and FSUIPC data source code (which is not operational at this time)
- implemented the 777 PFD AP Heading and NAV1 Heading bugs

Version 0.62
------------
- corrected bug in automake preventing updating the main code when library was changed

Version 0.61
------------
- implemented gnu automake and autoconf and removed cmake entirely
