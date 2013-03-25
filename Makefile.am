LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./configure_status libtool
ACLOCAL_AMFLAGS = -I m4

CONFIGURE_DEPENDENCIES = $(top_srcdir)/version.sh

$(top_srcdir)/configure: $(top_srcdir)/version.sh

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = grib_api.pc grib_api_f90.pc

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

SUBDIRS = src tools definitions samples ifs_samples/grib1 ifs_samples/grib1_mlgrib2 ifs_samples/grib1_mlgrib2_ieee64 tests tigge $(FORTRAN_MOD) examples/C $(F90_CHECK) $(PERL_OPT) python examples/python

EXTRA_DIST = README AUTHORS NOTICE LICENSE ChangeLog version.sh html \
                src/extrules.am \
                definitions/extrules.am \
				src/dummy.am \
				definitions/dummy.am \
				data/ls.log \
				data/multi.ok \
				data/in_copy.grib \
				data/bad.grib \
				data/local.good.log \
				data/typeOfProcessedData.ok \
				data/gts.grib \
				data/v.grib2 \
				data/satellite.grib \
				data/pad.grib \
				data/missing_field.grib1 \
				data/multi_step.txt \
				data/multi_created.grib2 \
				data/multi.grib2 \
				data/sample.grib2 \
				data/bitmap.diff \
				data/no_bitmap.diff \
				data/ieee_test.good \
				data/index.grib \
				data/index.ok \
				data/index_f90.ok \
				data/grid_ieee.grib \
				data/spherical_model_level.grib1_32.good \
				data/spherical_model_level.grib1.good \
				data/budg \
				data/tp_ecmwf.grib \
				data/statistics.out.good \
				data/jpeg.grib2 \
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
				data/tigge_pf_ecmwf.grib2 \
				data/regular_gaussian_pressure_level.grib1 \
				data/regular_gaussian_model_level.grib1 \
				data/regular_gaussian_surface.grib1 \
				data/regular_latlon_surface.grib1 \
				data/reduced_gaussian_model_level.grib1 \
				data/reduced_gaussian_pressure_level.grib1 \
				data/reduced_gaussian_lsm.grib1 \
				data/reduced_gaussian_surface_jpeg.grib2 \
				data/timeRangeIndicator_0.grib \
				data/timeRangeIndicator_10.grib  \
				data/timeRangeIndicator_5.grib \
				data/reduced_gaussian_model_level.grib1 \
				data/reduced_gaussian_surface.grib1 \
				data/reduced_latlon_surface.grib1 \
				data/spherical_model_level.grib1 \
				data/spherical_pressure_level.grib1 \
				data/missing.grib2 \
				data/gfs.c255.grib2 \
                data/constant_field.grib1 \
                data/reduced_gaussian_pressure_level_constant.grib1 \
                data/reduced_latlon_surface_constant.grib1 \
                data/regular_gaussian_pressure_level_constant.grib1 \
                data/regular_latlon_surface_constant.grib1 \
                data/reduced_gaussian_sub_area.grib1 \
				data/constant_field.grib1 \
				data/60_model_levels \
				data/list_points \
				data/step.log \
				data/step_grib1.filter \
				data/step_grib1.log \
				data/julian.out.good \
				data/tigge/tigge_ammc_pl_gh.grib  \
				data/tigge/tigge_ammc_pl_q.grib  \
				data/tigge/tigge_ammc_pl_t.grib  \
				data/tigge/tigge_ammc_pl_u.grib  \
				data/tigge/tigge_ammc_pl_v.grib  \
				data/tigge/tigge_ammc_sfc_10u.grib  \
				data/tigge/tigge_ammc_sfc_10v.grib  \
				data/tigge/tigge_ammc_sfc_2t.grib  \
				data/tigge/tigge_ammc_sfc_lsm.grib  \
				data/tigge/tigge_ammc_sfc_mn2t6.grib  \
				data/tigge/tigge_ammc_sfc_msl.grib  \
				data/tigge/tigge_ammc_sfc_mx2t6.grib  \
				data/tigge/tigge_ammc_sfc_orog.grib  \
				data/tigge/tigge_ammc_sfc_sf.grib  \
				data/tigge/tigge_ammc_sfc_sp.grib  \
				data/tigge/tigge_ammc_sfc_st.grib  \
				data/tigge/tigge_ammc_sfc_tcc.grib  \
				data/tigge/tigge_ammc_sfc_tcw.grib  \
				data/tigge/tigge_ammc_sfc_tp.grib  \
				data/tigge/tigge_babj_pl_gh.grib  \
				data/tigge/tigge_babj_pl_q.grib  \
				data/tigge/tigge_babj_pl_t.grib  \
				data/tigge/tigge_babj_pl_u.grib  \
				data/tigge/tigge_babj_pl_v.grib  \
				data/tigge/tigge_babj_sfc_10u.grib  \
				data/tigge/tigge_babj_sfc_10v.grib  \
				data/tigge/tigge_babj_sfc_2d.grib  \
				data/tigge/tigge_babj_sfc_2t.grib  \
				data/tigge/tigge_babj_sfc_lsm.grib  \
				data/tigge/tigge_babj_sfc_mn2t6.grib  \
				data/tigge/tigge_babj_sfc_msl.grib  \
				data/tigge/tigge_babj_sfc_mx2t6.grib  \
				data/tigge/tigge_babj_sfc_orog.grib  \
				data/tigge/tigge_babj_sfc_sd.grib  \
				data/tigge/tigge_babj_sfc_sf.grib  \
				data/tigge/tigge_babj_sfc_slhf.grib  \
				data/tigge/tigge_babj_sfc_sp.grib  \
				data/tigge/tigge_babj_sfc_sshf.grib  \
				data/tigge/tigge_babj_sfc_ssr.grib  \
				data/tigge/tigge_babj_sfc_str.grib  \
				data/tigge/tigge_babj_sfc_tcc.grib  \
				data/tigge/tigge_babj_sfc_tcw.grib  \
				data/tigge/tigge_babj_sfc_tp.grib  \
				data/tigge/tigge_cwao_pl_gh.grib  \
				data/tigge/tigge_cwao_pl_q.grib  \
				data/tigge/tigge_cwao_pl_t.grib  \
				data/tigge/tigge_cwao_pl_u.grib  \
				data/tigge/tigge_cwao_pl_v.grib  \
				data/tigge/tigge_cwao_sfc_10u.grib  \
				data/tigge/tigge_cwao_sfc_10v.grib  \
				data/tigge/tigge_cwao_sfc_2d.grib  \
				data/tigge/tigge_cwao_sfc_2t.grib  \
				data/tigge/tigge_cwao_sfc_mn2t6.grib  \
				data/tigge/tigge_cwao_sfc_msl.grib  \
				data/tigge/tigge_cwao_sfc_mx2t6.grib  \
				data/tigge/tigge_cwao_sfc_orog.grib  \
				data/tigge/tigge_cwao_sfc_sd.grib  \
				data/tigge/tigge_cwao_sfc_skt.grib  \
				data/tigge/tigge_cwao_sfc_sp.grib  \
				data/tigge/tigge_cwao_sfc_st.grib  \
				data/tigge/tigge_cwao_sfc_tcc.grib  \
				data/tigge/tigge_cwao_sfc_tcw.grib  \
				data/tigge/tigge_cwao_sfc_tp.grib  \
				data/tigge/tigge_ecmf_pl_gh.grib  \
				data/tigge/tigge_ecmf_pl_q.grib  \
				data/tigge/tigge_ecmf_pl_t.grib  \
				data/tigge/tigge_ecmf_pl_u.grib  \
				data/tigge/tigge_ecmf_pl_v.grib  \
				data/tigge/tigge_ecmf_pt_pv.grib  \
				data/tigge/tigge_ecmf_pv_pt.grib  \
				data/tigge/tigge_ecmf_pv_u.grib  \
				data/tigge/tigge_ecmf_pv_v.grib  \
				data/tigge/tigge_ecmf_sfc_10u.grib  \
				data/tigge/tigge_ecmf_sfc_10v.grib  \
				data/tigge/tigge_ecmf_sfc_2d.grib  \
				data/tigge/tigge_ecmf_sfc_2t.grib  \
				data/tigge/tigge_ecmf_sfc_cap.grib  \
				data/tigge/tigge_ecmf_sfc_cape.grib  \
				data/tigge/tigge_ecmf_sfc_mn2t6.grib  \
				data/tigge/tigge_ecmf_sfc_msl.grib  \
				data/tigge/tigge_ecmf_sfc_mx2t6.grib  \
				data/tigge/tigge_ecmf_sfc_sd.grib  \
				data/tigge/tigge_ecmf_sfc_sf.grib  \
				data/tigge/tigge_ecmf_sfc_skt.grib  \
				data/tigge/tigge_ecmf_sfc_slhf.grib  \
				data/tigge/tigge_ecmf_sfc_sm.grib  \
				data/tigge/tigge_ecmf_sfc_sp.grib  \
				data/tigge/tigge_ecmf_sfc_sshf.grib  \
				data/tigge/tigge_ecmf_sfc_ssr.grib  \
				data/tigge/tigge_ecmf_sfc_st.grib  \
				data/tigge/tigge_ecmf_sfc_str.grib  \
				data/tigge/tigge_ecmf_sfc_sund.grib  \
				data/tigge/tigge_ecmf_sfc_tcc.grib  \
				data/tigge/tigge_ecmf_sfc_tcw.grib  \
				data/tigge/tigge_ecmf_sfc_tp.grib  \
				data/tigge/tigge_ecmf_sfc_ttr.grib  \
				data/tigge/tigge_egrr_pl_gh.grib  \
				data/tigge/tigge_egrr_pl_q.grib  \
				data/tigge/tigge_egrr_pl_t.grib  \
				data/tigge/tigge_egrr_pl_u.grib  \
				data/tigge/tigge_egrr_pl_v.grib  \
				data/tigge/tigge_egrr_pt_pv.grib  \
				data/tigge/tigge_egrr_pv_pt.grib  \
				data/tigge/tigge_egrr_pv_u.grib  \
				data/tigge/tigge_egrr_pv_v.grib  \
				data/tigge/tigge_egrr_sfc_10u.grib  \
				data/tigge/tigge_egrr_sfc_10v.grib  \
				data/tigge/tigge_egrr_sfc_2d.grib  \
				data/tigge/tigge_egrr_sfc_2t.grib  \
				data/tigge/tigge_egrr_sfc_mn2t6.grib  \
				data/tigge/tigge_egrr_sfc_msl.grib  \
				data/tigge/tigge_egrr_sfc_mx2t6.grib  \
				data/tigge/tigge_egrr_sfc_sd.grib  \
				data/tigge/tigge_egrr_sfc_sf.grib  \
				data/tigge/tigge_egrr_sfc_skt.grib  \
				data/tigge/tigge_egrr_sfc_slhf.grib  \
				data/tigge/tigge_egrr_sfc_sm.grib  \
				data/tigge/tigge_egrr_sfc_sp.grib  \
				data/tigge/tigge_egrr_sfc_sshf.grib  \
				data/tigge/tigge_egrr_sfc_ssr.grib  \
				data/tigge/tigge_egrr_sfc_st.grib  \
				data/tigge/tigge_egrr_sfc_str.grib  \
				data/tigge/tigge_egrr_sfc_tcc.grib  \
				data/tigge/tigge_egrr_sfc_tcw.grib  \
				data/tigge/tigge_egrr_sfc_tp.grib  \
				data/tigge/tigge_egrr_sfc_ttr.grib  \
				data/tigge/tigge_kwbc_pl_gh.grib  \
				data/tigge/tigge_kwbc_pl_q.grib  \
				data/tigge/tigge_kwbc_pl_t.grib  \
				data/tigge/tigge_kwbc_pl_u.grib  \
				data/tigge/tigge_kwbc_pl_v.grib  \
				data/tigge/tigge_kwbc_pt_pv.grib  \
				data/tigge/tigge_kwbc_pv_pt.grib  \
				data/tigge/tigge_kwbc_pv_u.grib  \
				data/tigge/tigge_kwbc_pv_v.grib  \
				data/tigge/tigge_kwbc_sfc_10u.grib  \
				data/tigge/tigge_kwbc_sfc_10v.grib  \
				data/tigge/tigge_kwbc_sfc_2d.grib  \
				data/tigge/tigge_kwbc_sfc_2t.grib  \
				data/tigge/tigge_kwbc_sfc_cap.grib  \
				data/tigge/tigge_kwbc_sfc_cape.grib  \
				data/tigge/tigge_kwbc_sfc_ci.grib  \
				data/tigge/tigge_kwbc_sfc_lsm.grib  \
				data/tigge/tigge_kwbc_sfc_mn2t6.grib  \
				data/tigge/tigge_kwbc_sfc_msl.grib  \
				data/tigge/tigge_kwbc_sfc_mx2t6.grib  \
				data/tigge/tigge_kwbc_sfc_sd.grib  \
				data/tigge/tigge_kwbc_sfc_sf.grib  \
				data/tigge/tigge_kwbc_sfc_skt.grib  \
				data/tigge/tigge_kwbc_sfc_slhf.grib  \
				data/tigge/tigge_kwbc_sfc_sm.grib  \
				data/tigge/tigge_kwbc_sfc_sp.grib  \
				data/tigge/tigge_kwbc_sfc_sshf.grib  \
				data/tigge/tigge_kwbc_sfc_ssr.grib  \
				data/tigge/tigge_kwbc_sfc_st.grib  \
				data/tigge/tigge_kwbc_sfc_str.grib  \
				data/tigge/tigge_kwbc_sfc_tcw.grib  \
				data/tigge/tigge_kwbc_sfc_tp.grib  \
				data/tigge/tigge_kwbc_sfc_ttr.grib  \
				data/tigge/tigge_lfpw_pl_gh.grib  \
				data/tigge/tigge_lfpw_pl_q.grib  \
				data/tigge/tigge_lfpw_pl_t.grib  \
				data/tigge/tigge_lfpw_pl_u.grib  \
				data/tigge/tigge_lfpw_pl_v.grib  \
				data/tigge/tigge_lfpw_pv_pt.grib  \
				data/tigge/tigge_lfpw_pv_u.grib  \
				data/tigge/tigge_lfpw_pv_v.grib  \
				data/tigge/tigge_lfpw_sfc_10u.grib  \
				data/tigge/tigge_lfpw_sfc_10v.grib  \
				data/tigge/tigge_lfpw_sfc_2d.grib  \
				data/tigge/tigge_lfpw_sfc_2t.grib  \
				data/tigge/tigge_lfpw_sfc_cap.grib  \
				data/tigge/tigge_lfpw_sfc_cape.grib  \
				data/tigge/tigge_lfpw_sfc_mn2t6.grib  \
				data/tigge/tigge_lfpw_sfc_msl.grib  \
				data/tigge/tigge_lfpw_sfc_mx2t6.grib  \
				data/tigge/tigge_lfpw_sfc_sd.grib  \
				data/tigge/tigge_lfpw_sfc_sf.grib  \
				data/tigge/tigge_lfpw_sfc_skt.grib  \
				data/tigge/tigge_lfpw_sfc_slhf.grib  \
				data/tigge/tigge_lfpw_sfc_sp.grib  \
				data/tigge/tigge_lfpw_sfc_sshf.grib  \
				data/tigge/tigge_lfpw_sfc_ssr.grib  \
				data/tigge/tigge_lfpw_sfc_st.grib  \
				data/tigge/tigge_lfpw_sfc_str.grib  \
				data/tigge/tigge_lfpw_sfc_tcc.grib  \
				data/tigge/tigge_lfpw_sfc_tcw.grib  \
				data/tigge/tigge_lfpw_sfc_tp.grib  \
				data/tigge/tigge_lfpw_sfc_ttr.grib  \
				data/tigge/tigge_rjtd_pl_gh.grib  \
				data/tigge/tigge_rjtd_pl_q.grib  \
				data/tigge/tigge_rjtd_pl_t.grib  \
				data/tigge/tigge_rjtd_pl_u.grib  \
				data/tigge/tigge_rjtd_pl_v.grib  \
				data/tigge/tigge_rjtd_sfc_10u.grib  \
				data/tigge/tigge_rjtd_sfc_10v.grib  \
				data/tigge/tigge_rjtd_sfc_2d.grib  \
				data/tigge/tigge_rjtd_sfc_2t.grib  \
				data/tigge/tigge_rjtd_sfc_mn2t6.grib  \
				data/tigge/tigge_rjtd_sfc_msl.grib  \
				data/tigge/tigge_rjtd_sfc_mx2t6.grib  \
				data/tigge/tigge_rjtd_sfc_sd.grib  \
				data/tigge/tigge_rjtd_sfc_skt.grib  \
				data/tigge/tigge_rjtd_sfc_slhf.grib  \
				data/tigge/tigge_rjtd_sfc_sm.grib  \
				data/tigge/tigge_rjtd_sfc_sp.grib  \
				data/tigge/tigge_rjtd_sfc_sshf.grib  \
				data/tigge/tigge_rjtd_sfc_ssr.grib  \
				data/tigge/tigge_rjtd_sfc_str.grib  \
				data/tigge/tigge_rjtd_sfc_tcc.grib  \
				data/tigge/tigge_rjtd_sfc_tcw.grib  \
				data/tigge/tigge_rjtd_sfc_tp.grib  \
				data/tigge/tigge_rjtd_sfc_ttr.grib  \
				data/tigge/tigge_rksl_pl_gh.grib  \
				data/tigge/tigge_rksl_pl_q.grib  \
				data/tigge/tigge_rksl_pl_t.grib  \
				data/tigge/tigge_rksl_pl_u.grib  \
				data/tigge/tigge_rksl_pl_v.grib  \
				data/tigge/tigge_rksl_sfc_10u.grib  \
				data/tigge/tigge_rksl_sfc_10v.grib  \
				data/tigge/tigge_rksl_sfc_2t.grib  \
				data/tigge/tigge_rksl_sfc_msl.grib  \
				data/tigge/tigge_rksl_sfc_sp.grib  \
				data/tigge/tigge_sbsj_pl_gh.grib  \
				data/tigge/tigge_sbsj_pl_q.grib  \
				data/tigge/tigge_sbsj_pl_t.grib  \
				data/tigge/tigge_sbsj_pl_u.grib  \
				data/tigge/tigge_sbsj_pl_v.grib  \
				data/tigge/tigge_sbsj_sfc_10u.grib  \
				data/tigge/tigge_sbsj_sfc_10v.grib  \
				data/tigge/tigge_sbsj_sfc_2t.grib  \
				data/tigge/tigge_sbsj_sfc_msl.grib  \
				data/tigge/tigge_sbsj_sfc_sf.grib  \
				data/tigge/tigge_sbsj_sfc_skt.grib  \
				data/tigge/tigge_sbsj_sfc_sp.grib  \
				data/tigge/tigge_sbsj_sfc_ssr.grib  \
				data/tigge/tigge_sbsj_sfc_st.grib  \
				data/tigge/tigge_sbsj_sfc_tcc.grib  \
				data/tigge/tigge_sbsj_sfc_tcw.grib  \
				data/tigge/tigge_sbsj_sfc_tp.grib \
				data/tigge/tiggelam_cnmc_sfc.grib \
				data/constant_width_bitmap.grib \
				data/constant_width_boust_bitmap.grib \
				data/gen.grib \
				data/gen_bitmap.grib \
				data/gen_ext.grib \
				data/gen_ext_bitmap.grib \
				data/gen_ext_boust.grib \
				data/gen_ext_boust_bitmap.grib \
				data/gen_ext_spd_2.grib \
				data/gen_ext_spd_2_bitmap.grib \
				data/gen_ext_spd_2_boust_bitmap.grib \
				data/gen_ext_spd_3.grib \
				data/gen_ext_spd_3_boust_bitmap.grib \
				data/row.grib \
				data/simple.grib \
				data/read_any.ok \
				data/simple_bitmap.grib \
				data/second_ord_rbr.grib1 \
				perf/jmeter.awk \
				perf/time.sh


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

rpmspec = $(PACKAGE_TARNAME).spec

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
	find . -type f | grep $(prefix) | \
		cpio -ov -H ustar > $(main_package) ; \
	gzip $(main_package)

distclean-local:
	rm -rf $${PWD}/_rpm
	find $${PWD} -name "*.rpm" -exec rm {} \;
	rm -f *.tar.gz
	rm -rf $(RPM_HOST_CPU)
	rm -rf _dist _distdefs


