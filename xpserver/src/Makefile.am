# Build a shared Library as a plugin by NOT using Libtool

# install directories
bindir = $(prefix)/dist/$(PACKAGE_NAME)

# library names (32 and 64 bit plugins are built simultaneously)
bin_PROGRAMS = 32-@OS@.xpl 64-@OS@.xpl

# plugin source files
32_@OS@_xpl_SOURCES = xpserver.c clientdata.c customdata.c handleclient.c dictionary.c iniparser.c
64_@OS@_xpl_SOURCES = xpserver.c clientdata.c customdata.c handleclient.c dictionary.c iniparser.c

# plugin headers that must go into a distribution
noinst_HEADERS = xpserver.h common.h clientdata.h customdata.h handleclient.h dictionary.h iniparser.h CHeaders

# plugin compilation / linking flags
32_@OS@_xpl_CPPFLAGS = @AM_CPPFLAGS@
32_@OS@_xpl_LDFLAGS  = @AM_LDFLAGS@

if OS_APPLE
32_@OS@_xpl_CPPFLAGS += -m32 -ICHeaders/XPLM -DIBM=0 -DAPL=1 -DLIN=0
32_@OS@_xpl_LDFLAGS  += -m32 -dynamiclib -Wl,-undefined -Wl,dynamic_lookup -compatibility_version 1 -current_version 1.0 -Wl,-single_module
endif

if OS_LINUX
32_@OS@_xpl_CPPFLAGS += -m32 -ICHeaders/XPLM -DIBM=0 -DAPL=0 -DLIN=1 -w -fPIC -x c -MMD
32_@OS@_xpl_LDFLAGS  += -m32 -shared
endif

64_@OS@_xpl_CPPFLAGS = @AM_CPPFLAGS@
64_@OS@_xpl_LDFLAGS  = @AM_LDFLAGS@

if OS_APPLE
64_@OS@_xpl_CPPFLAGS += -m64 -ICHeaders/XPLM -DIBM=0 -DAPL=1 -DLIN=0
64_@OS@_xpl_LDFLAGS  += -m64 -dynamiclib -Wl,-undefined -Wl,dynamic_lookup -compatibility_version 1 -current_version 1.0 -Wl,-single_module
endif

if OS_LINUX
64_@OS@_xpl_CPPFLAGS += -m64 -ICHeaders/XPLM -DIBM=0 -DAPL=0 -DLIN=1 -w -fPIC -x c -MMD
64_@OS@_xpl_LDFLAGS  += -m64 -shared
endif
