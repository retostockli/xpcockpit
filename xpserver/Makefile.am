SUBDIRS=src
EXTRA_DIST=data SDK

PLUGINDIR=$(prefix)/$(PACKAGE_NAME)

install-data-local:
	$(MKDIR_P) '$(PLUGINDIR)'
	$(INSTALL_DATA) '$(srcdir)/data/$(PACKAGE_NAME).ini' '$(PLUGINDIR)/'

install-exec-local:
if XPL32
	$(MKDIR_P) '$(PLUGINDIR)/32'
else
	$(MKDIR_P) '$(PLUGINDIR)/64'
endif
if OS_WIN
if XPL32
	mv -f '$(PLUGINDIR)/@os@.xpl.exe' '$(PLUGINDIR)/32/@os@.xpl'
else
	mv -f '$(PLUGINDIR)/@os@.xpl.exe' '$(PLUGINDIR)/64/@os@.xpl'
endif
else
if XPL32
	mv -f '$(PLUGINDIR)/@os@.xpl' '$(PLUGINDIR)/32/@os@.xpl'
else
	mv -f '$(PLUGINDIR)/@os@.xpl' '$(PLUGINDIR)/64/@os@.xpl'
endif
endif