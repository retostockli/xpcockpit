# The Program name to build
bin_PROGRAMS=xpusb hidls

if OS_DARWIN
OS_SRC = hid_darwin.c
endif

if OS_LINUX
OS_SRC = hid_linux.c
endif

# The Sources needed to build the programs
xpusb_SOURCES = $(OS_SRC) \
xpusb.c \
libiocards.c \
serverdata.c \
handleserver.c \
libusb_interface.c \
hidapi_interface.c \
dictionary.c \
iniparser.c \
mcp_737.c \
pedestal_737.c \
throttle_737.c \
efis_737.c \
yokerudder_737.c \
check_x737.c \
check_zibo.c \
iocard_test.c \
usbkeys_test.c \
usbservos_test.c \
dcmotorsplus_test.c \
bu0836_test.c \
a320_overhead.c \
a320_mcdu_keys.c \
a320_pedestal_mip.c

hidls_SOURCES = $(OS_SRC) hidls.c

# The Headers needs to build the programs
noinst_HEADERS=\
common.h \
hidapi.h \
dictionary.h \
iniparser.h \
xpusb.h \
libiocards.h \
serverdata.h \
libusb_interface.h \
hidapi_interface.h \
handleserver.h \
iocard_test.h \
usbkeys_test.h \
usbservos_test.h \
dcmotorsplus_test.h \
bu0836_test.h \
mcp_737.h \
pedestal_737.h \
throttle_737.h \
efis_737.h \
yokerudder_737.h \
check_x737.h \
check_zibo.h \
a320_overhead.h \
a320_mcdu_keys.h \
a320_pedestal_mip.h
