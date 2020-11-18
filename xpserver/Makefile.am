SUBDIRS=src
EXTRA_DIST=data SDK

install-data-local:
	$(MKDIR_P) $(DESTDIR)plugins/$(PACKAGE_NAME)
	$(INSTALL_DATA) $(srcdir)/data/$(PACKAGE_NAME).ini $(DESTDIR)plugins/$(PACKAGE_NAME)/

install-exec-hook:
if XPL32
	$(MKDIR_P) $(DESTDIR)plugins/$(PACKAGE_NAME)/32
else
	$(MKDIR_P) $(DESTDIR)plugins/$(PACKAGE_NAME)/64
endif
if OS_WIN
if XPL32
	mv -f $(DESTDIR)plugins/$(PACKAGE_NAME)/@os@.xpl.exe $(DESTDIR)plugins/$(PACKAGE_NAME)/32/@os@.xpl
else
	mv -f $(DESTDIR)plugins/$(PACKAGE_NAME)/@os@.xpl.exe $(DESTDIR)plugins/$(PACKAGE_NAME)/64/@os@.xpl
endif
else
if XPL32
	mv -f $(DESTDIR)plugins/$(PACKAGE_NAME)/@os@.xpl $(DESTDIR)plugins/$(PACKAGE_NAME)/32/@os@.xpl
else
	mv -f $(DESTDIR)plugins/$(PACKAGE_NAME)/@os@.xpl $(DESTDIR)plugins/$(PACKAGE_NAME)/64/@os@.xpl
endif
endif