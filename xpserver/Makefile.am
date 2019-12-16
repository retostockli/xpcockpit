SUBDIRS=src
EXTRA_DIST=data

install-data-local:
	$(MKDIR_P) $(DESTDIR)dist/$(PACKAGE_NAME)
	$(INSTALL_DATA) $(srcdir)/data/$(PACKAGE_NAME).ini $(DESTDIR)dist/$(PACKAGE_NAME)

install-exec-hook:
	$(MKDIR_P) $(DESTDIR)dist/$(PACKAGE_NAME)/32
	$(MKDIR_P) $(DESTDIR)dist/$(PACKAGE_NAME)/64
	mv -f $(DESTDIR)dist/$(PACKAGE_NAME)/32-@OS@.xpl $(DESTDIR)dist/$(PACKAGE_NAME)/32/@OS@.xpl
	mv -f $(DESTDIR)dist/$(PACKAGE_NAME)/64-@OS@.xpl $(DESTDIR)dist/$(PACKAGE_NAME)/64/@OS@.xpl
