2019/12/17 Reto Stockli

GENERAL OVERVIEW
================

xpserver uses the X-Plane SDK Version 2.1.1. It is a X-Plane's plugin and 
interacts with the X-Plane internal data structures. 
They are called 'datarefs'. Datarefs are pointers to X-Plane internal 
flight data. Clients can subscribe to these datarefs by use of
the dta exchange protocol defined later. xpserver reads these datarefs 
and sends the data via the TCP/IP protocol to clients connected to xpserver. 
It also reads data from connected client applications and updates X-Plane 
internal datarefs. It also accepts non-X-Plane datarefs (custom client
datarefs). Upon subscription by a client custom datarefs are allocated as 
a memory structure in X-Plane and can be exchanged between clients (X-Plane
itself does not care about those).

When xpserver is loaded in X-Plane, the following operations take place:
1. the xpserver.ini file is parsed
2. the TCP/IP server is started and listens on the given port
3. a callback is registered within X-Plane to periodically call xpserver

On each callback the following operations take place:

1. Check if a new client has connected

2. Receive data from client and updated X-Plane/Custom Datarefs

3. Send changed X-Plane Datarefs to client

4. Check if a client has disconnected

5. Schedule a new callback

INSTALLATION
============

If downloaded via git first do:
autoreconf
automake --add-missing
autoreconf
in order to set up all automake/autoconf files. Then do:

./configure
make
make install

This will generate a directory plugins/xpserver.
Copy the xpserver subdirectory to your X-Plane directory under Resources/plugins/

You're all set. Once launching X-Plane you should have the xpserver plugin 
showing up under plugins. In the xpserver directory you will find an ini file
which can be used to change the TCP/IP port number or change verbosity level.
xpserver will dump data to xpserver.log in your X-Plane main directory.


SPECIFIC PROTOCOL
=================

All data is sent via the system's byte order. Each transmission has an end transmission marker.
Each transmitted dataref has a start marker, followed by the data. The client 
subscribes to datarefs of specific type which are confirmed (or denied) by 
x-plane. Each dataref is assigned a unique (internal) offset number which is 
then subsequently used to identify data packets by the server and the client. 
The data of these datarefs is then exchanged between x-plane and the client 
whenever data changes on either x-plane's or the client's side. For regular
data transmission integer offset numbers are chosen instead of the full dataref
name in order to avoid string comparisons at each send/receive.

The server/client data exchange protocol goes as follows. Any of the actions
1-7 can be mixed within the transmission since each step is identified
by a marker. So you can subscribe to one dataref, unlink another, and 
send data to yet another in the same transmission. You can subscribe and
unsubscribe to a dataref as much as you want. Since the first integer
in each data transmission is the addition of the marker plus the offset number
it makes sense not to have more than 1 Million datarefs ;-). Since
memory for datarefs is allocated dynamically with the maximum offset number,
clients should best start with offset 0 for the first subscribed dataref and 
work their way up from there (you can start at offset 1000, but you will 
simply waste memory for 1000 offsets).

1. client subscribes to dataref
-------------------------------
client sends 220 bytes to X-Plane:
4 byte INTEGER: Link marker (MARK_LINK) + offset number (add both together)
4 byte INTEGER: X-Plane Data type (for data type values see below)
4 byte INTEGER: number of elements in dataref array (or 1 if scalar)
4 byte INTEGER: element of array (or -1 if all elements are requested)
4 byte INTEGER: requested precision in 10^x, where x is the precision, or INT_MISS for no precision)
100 byte CHAR : dataref name
100 byte CHAR : client name


2. X-Plane confirms dataref subscription
----------------------------------------
X-Plane sends 104 bytes to client:
4 byte INTEGER: Link marker (MARK_LINK) + offset number
100 byte CHAR : dataref name (if ok) or error message (if subscription is not ok)


3. Sending data (both directions)
---------------------------------
Either client or X-Plane sends 4 bytes plus data
4 byte INTEGER: data marker (MARK_DATA) + offset number
4 byte INTEGER: XP integer type (1)
OR
4 byte FLOAT: XP float type (2)
OR
8 byte DOUBLE: XP double type (4)
OR 
4 byte FLOAT * number of selected elements: XP float array type (8)
OR 
4 byte INTEGER * number of selected elements: XP integer array type (16)
OR
number of selected bytes of CHAR type: XP data array type (32)


4. client unsubscribes to dataref
---------------------------------
client sends 104 bytes:
4 byte INTEGER: unlink marker (MARK_UNLINK) + offset number
100 byte CHAR : dataref name


5. X-Plane confirms unsubscription
----------------------------------
X-Plane sends 104 bytes:
4 byte INTEGER: unlink marker (MARK_UNLINK) + offset number
100 byte CHAR : dataref name (if unlink is ok) or error message (if not ok)


6. End of transmission
----------------------
After each loop a packet of data or link requests 
is completed with an EOT marker, from either the client or the X-Plane
by sending 4 bytes to the other side:
4 byte INTEGER: EOT marker (MARK_EOT)


7. Terminating connection
-------------------------
A connection can be terminated from either client or X-Plane by
sending 4 bytes to the other side:
4 byte INTEGER: disconnect marker (MARK_DISCONNECT)


TRANSMISSION MARKERS
====================

MARK_DATA       1000000
MARK_LINK       2000000
MARK_UNLINK     3000000
MARK_EOT        4000000
MARK_DISCONNECT 5000000


X-PLANE DATATYPES
=================
from http://www.xsquawkbox.net/xpsdk/mediawiki/Category:XPLMDataAccess

XPTYPE_UNKNOWN            0 // unknown 
XPTYPE_INT                1 // int
XPTYPE_FLT                2 // float
XPTYPE_DBL                4 // double (2+4=6 can concur!)
XPTYPE_FLT_ARR            8 // float array
XPTYPE_INT_ARR           16 // int array
XPTYPE_BYTE_ARR          32 // byte/char data
XPTYPE_CMD_ONCE          64 // one-shot command
XPTYPE_CMD_HOLD         128 // on/off command


MISSING DATA VALUES
===================

INT_MISS -2000000000
FLT_MISS -2000000000.0
DBL_MISS -2000000000.0


WHERE TO FIND DARAREFS
======================

General X-Plane Dararefs are documented on this website:
http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html

There are numerous other plugins useful to display and manage Datarefs:
http://wiki.x-plane.com/DataRefEditor
