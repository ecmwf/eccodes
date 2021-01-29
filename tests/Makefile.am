TESTS = definitions.sh \
        grib_calendar.sh \
        codes_split_file.sh \
        bufrdc_desc_ref.sh \
        bufrdc_ref.sh \
        grib_data_quality_checks.sh \
        bufr_dump_data.sh \
        bufr_dump_samples.sh \
        bufr_dump_subset.sh \
        bufr_json_samples.sh \
        bufr_filter.sh \
        bufr_json_data.sh \
        bufr_compare.sh \
        bufr_dump_descriptors.sh \
        bufr_copy.sh \
        bufr_count.sh \
        bufr_get.sh \
        bufr_ls.sh \
        bufr_set.sh \
        bufr_change_edition.sh \
        gts_ls.sh \
        gts_compare.sh \
        gts_get.sh \
        metar_ls.sh \
        metar_dump.sh \
        ieee.sh \
        grib1to2.sh \
        grib1to3.sh \
        unit_tests.sh \
        grib2to1.sh \
        grib2to3.sh \
        grib_bad.sh \
        grib_ls.sh \
        grib_filter.sh  \
        grib_multi.sh \
        pseudo_budg.sh \
        pseudo_diag.sh \
        grib_gridType.sh \
        grib_change_packing.sh \
        grib_octahedral.sh \
        grib_global.sh \
        grib_concept.sh \
        grib_decimalPrecision.sh \
        grib_bitsPerValue.sh \
        grib_get_fail.sh \
        grib_missing.sh \
        grib_local.sh \
        grib_step.sh \
        grib_set.sh \
        grib_iterator.sh \
        grib_proj_string.sh \
        grib_compare.sh \
        grib_level.sh \
        grib_indexing.sh \
        grib_list.sh \
        grib_second_order.sh \
        grib_change_scanning.sh \
        julian.sh \
        grib_statistics.sh \
        grib_tigge.sh \
        grib_uerra.sh \
        grib_tigge_conversions1.sh \
        grib_tigge_conversions2.sh \
        read_any.sh \
        grib_padding.sh \
        grib_grid_lamb_az_eq_area.sh \
        grib_grid_lambert_conformal.sh \
        grib_grid_space_view.sh \
        grib_grid_polar_stereographic.sh \
        grib_grid_mercator.sh \
        grib_grid_unstructured.sh \
        grib_to_netcdf.sh \
        grib_dump_debug.sh \
        grib_dump_json.sh \
        grib_jpeg.sh \
        grib_ccsds.sh \
        grib_md5.sh \
        grib_neg_fctime.sh \
        bufr_ecc-286.sh \
        bufr_ecc-288.sh \
        bufr_ecc-313.sh \
        bufr_ecc-359.sh \
        bufr_ecc-379.sh \
        bufr_ecc-393.sh \
        bufr_ecc-428.sh \
        bufr_ecc-433.sh \
        bufr_ecc-517.sh \
        bufr_ecc-556.sh \
        bufr_ecc-616.sh \
        bufr_ecc-673.sh \
        bufr_ecc-686.sh \
        bufr_ecc-690.sh \
        bufr_ecc-750.sh \
        bufr_ecc-765.sh \
        bufr_ecc-875.sh \
        bufr_ecc-887.sh \
        grib_ecc-136.sh \
        grib_ecc-967.sh \
        grib_ecc-1065.sh \
        grib_ecc-1053.sh \
        grib_ecc-490.sh \
        grib_ecc-756.sh \
        grib_ecc-873.sh \
        grib_ecc-600.sh \
        grib_ecc-923.sh \
        grib_ecc-979.sh \
        grib_ecc-984.sh \
        grib_ecc-1000.sh \
        grib_ecc-1001.sh \
        grib_ecc-1030.sh \
        grib_lam_gp.sh \
        grib_lam_bf.sh \
        bufr_get_element.sh \
        bufr_extract_headers.sh

# First download all the necessary data for testing
# Note: if download fails, no tests will be done
$(TESTS): download_data
download_data:
	@$(top_srcdir)/data/download.sh $(top_srcdir)/data

noinst_PROGRAMS = packing_check gauss_sub read_any grib_double_cmp packing pack_unpack \
                  julian grib_read_index grib_indexing gribex_perf\
                  jpeg_perf grib_ccsds_perf so_perf png_perf grib_bpv_limit laplacian \
                  unit_tests bufr_ecc-517 grib_lam_gp grib_lam_bf grib_sh_imag \
                  bufr_extract_headers bufr_get_element

laplacian_SOURCES = laplacian.c
packing_SOURCES = packing.c
packing_check_SOURCES = packing_check.c
read_any_SOURCES = read_any.c
grib_double_cmp_SOURCES = grib_double_cmp.c
pack_unpack_SOURCES = pack_unpack.c
julian_SOURCES = julian.c
unit_tests_SOURCES = unit_tests.c
grib_indexing_SOURCES = grib_indexing.c
grib_read_index_SOURCES = grib_read_index.c
jpeg_perf_SOURCES = jpeg_perf.c
so_perf_SOURCES = so_perf.c
png_perf_SOURCES = png_perf.c
grib_ccsds_perf_SOURCES  = grib_ccsds_perf.c
gribex_perf_SOURCES = gribex_perf.c
gauss_sub_SOURCES = gauss_sub.c
bufr_ecc_517_SOURCES = bufr_ecc-517.c
grib_lam_gp_SOURCES = grib_lam_gp.c
grib_lam_bf_SOURCES = grib_lam_bf.c
grib_sh_imag_SOURCES = grib_sh_imag.c
bufr_extract_headers_SOURCES = bufr_extract_headers.c
bufr_get_element_SOURCES = bufr_get_element.c

LDADD = $(top_builddir)/src/libeccodes.la $(EMOS_LIB)

INCLUDES   = -I$(top_builddir)/src

EXTRA_DIST = $(TESTS) mf.rules filter_rules include.sh include.ctest.sh.in utils.sh \
             grib_lamb_az_eq_area.ref CMakeLists.txt number_compare.pl

