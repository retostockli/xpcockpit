# The Program name to build
bin_PROGRAMS=xpusb hidls

if OS_DARWIN
OS_SRC = hid_darwin.c
endif

if OS_LINUX
OS_SRC = hid_linux.c
endif

if OS_WINDOWS
OS_SRC = hid_windows.c
endif

# The Sources needed to build the programs
xpusb_SOURCES = $(OS_SRC) \
common.c \
xpusb.c \
libiocards.c \
libusb_interface.c \
hidapi_interface.c \
iocard_test.c \
usbkeys_test.c \
usbservos_test.c \
dcmotorsplus_test.c \
bu0836_test.c \
b737_mcp.c \
b737_pedestal.c \
b737_throttle.c \
b737_efis.c \
b737_yokerudder.c \
b737_overhead_fwd.c \
a320_overhead.c \
a320_chrono.c \
a320_mcdu_keys.c \
a320_pedestal_mip.c

hidls_SOURCES = $(OS_SRC) hidls.c

# The Headers needs to build the programs
noinst_HEADERS=\
common.h \
hidapi.h \
xpusb.h \
libiocards.h \
libusb_interface.h \
hidapi_interface.h \
iocard_test.h \
usbkeys_test.h \
usbservos_test.h \
dcmotorsplus_test.h \
bu0836_test.h \
b737_mcp.h \
b737_pedestal.h \
b737_throttle.h \
b737_efis.h \
b737_yokerudder.h \
b737_overhead_fwd.h \
a320_overhead.h \
a320_chrono.h \
a320_mcdu_keys.h \
a320_pedestal_mip.h
