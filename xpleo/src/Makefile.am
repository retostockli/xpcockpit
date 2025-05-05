# The Program name to build
bin_PROGRAMS=xpleo hidls

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
xpleo_SOURCES = $(OS_SRC) \
common.c \
xpleo.c \
libleo.c \
libusb_interface.c \
hidapi_interface.c \
test.c \
b737_yokerudder.c

hidls_SOURCES = $(OS_SRC) hidls.c

# The Headers needs to build the programs
noinst_HEADERS=\
common.h \
hidapi.h \
xpleo.h \
libleo.h \
libusb_interface.h \
hidapi_interface.h \
test.h \
b737_yokerudder.h
