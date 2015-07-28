LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./configure_status libtool
ACLOCAL_AMFLAGS = -I m4

CONFIGURE_DEPENDENCIES = $(top_srcdir)/version.sh

$(top_srcdir)/configure: $(top_srcdir)/version.sh

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rpms/eccodes.pc rpms/eccodes_f90.pc

if WITH_PERL
  PERL_OPT = @PERLDIR@
else
  PERL_OPT =
endif

if WITH_PYTHON
  PYTHON_OPT = python
else
  PYTHON_OPT =
endif

SUBDIRS = src tools definitions samples ifs_samples/grib1 ifs_samples/grib1_mlgrib2 ifs_samples/grib1_mlgrib2_ieee64 \
          tests tigge $(FORTRAN_MOD) examples/C $(F90_CHECK) $(PERL_OPT) $(PYTHON_OPT) examples/python data

EXTRA_DIST = README AUTHORS NOTICE LICENSE ChangeLog version.sh html \
		src/extrules.am \
		definitions/extrules.am \
		python/extrules.am \
		src/dummy.am \
		definitions/dummy.am \
		definitions/make_makefile_am.pl \
		data/grib_data_files.txt \
		data/ls.log \
		data/multi.ok \
		data/local.good.log \
		data/typeOfProcessedData.ok \
		data/multi_step.txt \
		data/bitmap.diff \
		data/no_bitmap.diff \
		data/ieee_test.good \
		data/index.ok \
		data/index_f90.ok \
		data/spherical_model_level.grib1_32.good \
		data/spherical_model_level.grib1.good \
		data/statistics.out.good \
		data/scan_x_regular_gg_5_7_good.dump \
		data/scan_x_regular_ll_5_4_good.dump \
		data/scan_x_regular_ll_5_7_good.dump \
		data/scan_x_regular_ll_8_4_good.dump \
		data/scan_x_regular_ll_8_7_good.dump \
		data/scan_x_rotated_ll_5_4_good.dump \
		data/scan_x_rotated_ll_5_7_good.dump \
		data/scan_x_rotated_ll_8_4_good.dump \
		data/scan_x_rotated_ll_8_7_good.dump \
		data/scan_y_regular_ll_5_4_good.dump \
		data/scan_y_regular_ll_5_7_good.dump \
		data/scan_y_regular_ll_8_4_good.dump \
		data/scan_y_regular_ll_8_7_good.dump \
		data/scan_y_rotated_ll_5_4_good.dump \
		data/scan_y_rotated_ll_5_7_good.dump \
		data/scan_y_rotated_ll_8_4_good.dump \
		data/scan_y_rotated_ll_8_7_good.dump \
		data/tigge_pf_ecmwf.grib2.ref \
		data/60_model_levels \
		data/list_points \
		data/step.log \
		data/step_grib1.filter \
		data/step_grib1.log \
		data/julian.out.good \
		data/tigge/tigge_data_files.txt \
		data/read_any.ok \
		data/download.sh \
		perf/jmeter.awk \
		perf/time.sh \
		CMakeLists.txt \
		project_summary.cmake \
		VERSION.cmake \
		eccodes_config.h.in \
		examples/CMakeLists.txt \
		data/CMakeLists.txt \
		data/tigge/CMakeLists.txt \
		data/bufr/CMakeLists.txt \
		ifs_samples/grib1/CMakeLists.txt \
		ifs_samples/grib1_mlgrib2_ieee64/CMakeLists.txt \
		ifs_samples/grib1_mlgrib2_ieee32/CMakeLists.txt \
		ifs_samples/CMakeLists.txt \
		ifs_samples/grib1_mlgrib2/CMakeLists.txt \
		samples/CMakeLists.txt \
		windows/msvc/grib_api.sln \
		windows/msvc/grib_api_lib/grib_api_lib.vcproj \
		windows/msvc/grib_dump/grib_dump.vcproj \
		windows/msvc/grib_compare/grib_compare.vcproj \
		windows/msvc/grib_copy/grib_copy.vcproj \
		windows/msvc/grib_filter/grib_filter.vcproj \
		windows/msvc/grib_get/grib_get.vcproj \
		windows/msvc/grib_get_data/grib_get_data.vcproj \
		windows/msvc/grib_ls/grib_ls.vcproj \
		windows/msvc/grib_set/grib_set.vcproj


perf_dir = @abs_builddir@/perf


check-perf:
	@echo "Running and benchmarking tests $(perf_dir). This process will take some time."
#       -rm -f $(perf_dir)/report.out*
	for i in 1 2 3 ; do \
		echo "Running iteration $$i" ;\
			$(MAKE) check PYTHON=$(PYTHON) TOPBUILDDIR=$(abs_builddir) TESTS_ENVIRONMENT=$(perf_dir)/time.sh \
                > $(perf_dir)/report.out.$$i 2>&1; done
	cat $(perf_dir)/report.out* | $(AWK) -f $(perf_dir)/jmeter.awk -v JMETER="true" TIMESTAMP=`date +%s`000 > $(perf_dir)/report.jtl
	cat $(perf_dir)/report.out* | $(AWK) -f $(perf_dir)/jmeter.awk -v JMETER="false" TIMESTAMP=`date +%s`000 > $(perf_dir)/report.xml
#	-rm -f $(perf_dir)/report.out*
	@echo "Testing and benchmarks have finished"

rpmspec = rpms/$(PACKAGE_TARNAME).spec

rpmmacros = \
    --define="_rpmdir $${PWD}"\
    --define="_srcrpmdir $${PWD}"\
    --define="_sourcedir $${PWD}"\
    --define="_specdir $${PWD}"\
    --define="_builddir $${PWD}"

RPMBUILD = rpmbuild
RPMFLAGS = --nodeps --buildroot="$${PWD}/_rpm"

rpmcheck:
	@if [ which $(RPMBUILD) &> /dev/null ]; then \
		echo "*** This make target requires an rpm-based Linux distribution"; \
		(exit 1); exit 1; \
	fi

srcrpm: dist rpmcheck $(rpmspec)
	$(RPMBUILD) $(RPMFLAGS) -bs $(rpmmacros) $(rpmspec)

rpms: dist rpmcheck $(rpmspec)
	$(RPMBUILD) $(RPMFLAGS) -ba $(rpmmacros) $(rpmspec)

main_package = $(abs_top_srcdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-$(host_os)-$(host_cpu)-$(RPM_RELEASE).tar

dist-defs:
	@rm -f $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-defs.tar.gz
	cd definitions; \
	$(MAKE) top_distdir=$(abs_top_srcdir)/_distdefs distdir=$(abs_top_srcdir)/_distdefs/definitions distdir; \
	cd $(abs_top_srcdir)/_distdefs; \
	tar zcvf $(abs_top_srcdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-defs.tar.gz definitions > /dev/null
	@rm -rf _distdefs
	@echo "Created definitions tar ball \"$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-defs.tar.gz\""

bindist:
	$(MAKE) DESTDIR=$(abs_top_srcdir)/_dist
	$(MAKE) DESTDIR=$(abs_top_srcdir)/_dist install
	cd $(abs_top_srcdir)/_dist; \
	find . -type f -o -type l | grep $(prefix) | \
		cpio -ov -H ustar > $(main_package) ; \
	gzip $(main_package)

distclean-local:
	rm -rf $${PWD}/_rpm
	find $${PWD} -name "*.rpm" -exec rm {} \;
	rm -f *.tar.gz
	rm -rf $(RPM_HOST_CPU)
	rm -rf _dist _distdefs


