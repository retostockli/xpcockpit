SUBDIRS=src
EXTRA_DIST=data SDK

install-data-local:
	$(MKDIR_P) $(DESTDIR)dist/$(PACKAGE_NAME)
	$(INSTALL_DATA) $(srcdir)/data/$(PACKAGE_NAME).ini $(DESTDIR)dist/$(PACKAGE_NAME)

install-exec-hook:
if XPL32
	$(MKDIR_P) $(DESTDIR)dist/$(PACKAGE_NAME)/32
else
	$(MKDIR_P) $(DESTDIR)dist/$(PACKAGE_NAME)/64
endif
if OS_WIN
if XPL32
	mv -f $(DESTDIR)dist/$(PACKAGE_NAME)/@os@.xpl.exe $(DESTDIR)dist/$(PACKAGE_NAME)/32/@os@.xpl
else
	mv -f $(DESTDIR)dist/$(PACKAGE_NAME)/@os@.xpl.exe $(DESTDIR)dist/$(PACKAGE_NAME)/64/@os@.xpl
endif
else
if XPL32
	mv -f $(DESTDIR)dist/$(PACKAGE_NAME)/@os@.xpl $(DESTDIR)dist/$(PACKAGE_NAME)/32/@os@.xpl
else
	mv -f $(DESTDIR)dist/$(PACKAGE_NAME)/@os@.xpl $(DESTDIR)dist/$(PACKAGE_NAME)/64/@os@.xpl
endif
endif