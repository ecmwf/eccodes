
API_DIR=GRIB-API
PERLMAKEMAKER=$(API_DIR)/Makefile.PL
PERLMAKEFILE=$(API_DIR)/Makefile
PERLLIB=$(API_DIR)/blib/arch/auto/GRIB/API/API.so

dist-hook:
	( cd $(API_DIR) &&  \
	   cat MANIFEST \
	       | cpio -pdum $(distdir)/$(API_DIR) 2> /dev/null ; )

all-local: all-perl

all-perl: $(PERLLIB)

$(PERLMAKEFILE): $(PERLMAKEMAKER)
	cd $(API_DIR) && @PERL@ Makefile.PL @PERL_INSTALL_OPTIONS@ @PERL_MAKE_OPTIONS@ @PERL_LD_OPTIONS@ 

$(PERLLIB): $(PERLMAKEFILE) $(top_builddir)/src/libgrib_api.a
	cd $(API_DIR) && $(MAKE)

$(top_builddir)/src/libgrib_api.a: $(top_builddir)/src/Makefile
	cd $(top_builddir)/src  && $(MAKE)

install-exec-perl: $(PERLMAKEFILE)
	cd $(API_DIR) && $(MAKE) install && $(MAKE) clean

install-exec-am:install-exec-perl

check-perl: $(PERLMAKEFILE)
	cd $(API_DIR) && $(MAKE) test

clean-perl: $(PERLMAKEFILE)
	cd $(API_DIR) && \
	$(MAKE) clean && \
	rm -f Makefile.old

distclean-perl: clean-perl
clean: clean-perl
