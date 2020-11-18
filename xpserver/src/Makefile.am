# Build a shared Library as a plugin by NOT using Libtool

# install directories
bindir = $(prefix)/plugins/$(PACKAGE_NAME)

# library names (Only 64 bit plugins are built)
bin_PROGRAMS = @os@.xpl

# plugin source files
@os@_xpl_SOURCES = xpserver.c clientdata.c customdata.c handleclient.c dictionary.c iniparser.c

# plugin headers that must go into a distribution
noinst_HEADERS = xpserver.h common.h clientdata.h customdata.h handleclient.h dictionary.h iniparser.h

@os@_xpl_CPPFLAGS = @AM_CPPFLAGS@
@os@_xpl_LDFLAGS  = @AM_LDFLAGS@

if OS_MAC
@os@_xpl_CPPFLAGS += -I../SDK/CHeaders/XPLM -DIBM=0 -DAPL=1 -DLIN=0
@os@_xpl_LDFLAGS  += -dynamiclib -Wl,-undefined -Wl,dynamic_lookup -compatibility_version 1 -current_version 1.0 -Wl,-single_module
endif

if OS_LIN
@os@_xpl_CPPFLAGS += -I../SDK/CHeaders/XPLM -DIBM=0 -DAPL=0 -DLIN=1 -w -fPIC -x c -MMD
@os@_xpl_LDFLAGS  += -shared
endif

if OS_WIN
@os@_xpl_CPPFLAGS += -I../SDK/CHeaders/XPLM -DIBM=1 -DAPL=0 -DLIN=0 -w -fPIC -x c -MMD
@os@_xpl_LDFLAGS  += -shared -L../SDK/Libraries/Win
if XPL32
@os@_xpl_LDADD     = -lXPLM -lws2_32
else
@os@_xpl_LDADD     = -lXPLM_64 -lws2_32
endif
endif
