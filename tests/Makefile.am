TESTS = definitions.sh \
        grib_calendar.sh \
        bufrdc_desc_ref.sh \
        bufrdc_ref.sh \
        bufr_dump.sh \
        bufr_filter.sh \
        bufr_json.sh \
        bufr_compare.sh \
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
        badgrib.sh \
        grib_ls.sh \
        grib_filter.sh  \
        grib_multi.sh \
        budg.sh \
        grib_gridType.sh \
        grib_octahedral.sh \
        grib_global.sh \
        grib_concept.sh \
        decimalPrecision.sh \
        grib_bitsPerValue.sh \
        get_fail.sh \
        missing.sh \
        grib_local.sh \
        grib_step.sh \
        grib_set.sh \
        grib_iterator.sh \
        grib_compare.sh \
        grib_level.sh \
        index.sh \
        grib_bitmap.sh \
        list.sh \
        grib_second_order.sh \
        multi_from_message.sh \
        grib_change_scanning.sh \
        julian.sh \
        grib_statistics.sh \
        tigge.sh \
        uerra.sh \
        tigge_conversions.sh \
        read_any.sh \
        grib_padding.sh \
        lamb_az_eq_area.sh \
        grib_to_netcdf.sh \
        grib_dump_debug.sh \
        grib_jpeg.sh \
        grib_ccsds.sh \
        md5.sh \
        grib_util_set_spec.sh \
        grib_neg_fctime.sh \
        bufr_ecc-197.sh \
        bufr_ecc-286.sh \
        bufr_ecc-288.sh

# First download all the necessary data for testing
# Note: if download fails, no tests will be done
$(TESTS): download_data
download_data:
	@$(top_srcdir)/data/download.sh $(top_srcdir)/data

noinst_PROGRAMS = packing_check gauss_sub read_any grib_double_cmp packing pack_unpack \
                  multi_from_message julian read_index index gribex_perf\
                  jpeg_perf grib_ccsds_perf so_perf png_perf bpv_limit laplacian \
                  unit_tests grib_util_set_spec

multi_from_message_SOURCES = multi_from_message.c
laplacian_SOURCES = laplacian.c
packing_SOURCES = packing.c
packing_check_SOURCES = packing_check.c
read_any_SOURCES = read_any.c
grib_double_cmp_SOURCES = grib_double_cmp.c
pack_unpack_SOURCES = pack_unpack.c
julian_SOURCES = julian.c
unit_tests_SOURCES = unit_tests.c
index_SOURCES = index.c
read_index_SOURCES = read_index.c
jpeg_perf_SOURCES = jpeg_perf.c
so_perf_SOURCES = so_perf.c
png_perf_SOURCES = png_perf.c
grib_ccsds_perf_SOURCES  = grib_ccsds_perf.c
gribex_perf_SOURCES = gribex_perf.c
gauss_sub_SOURCES = gauss_sub.c
grib_util_set_spec_SOURCES = grib_util_set_spec.c

LDADD = $(top_builddir)/src/libeccodes.la $(EMOS_LIB)

INCLUDES   = -I$(top_builddir)/src

EXTRA_DIST = $(TESTS) mf.rules filter_rules include.sh include.ctest.sh.in utils.sh \
             lamb_az_eq_area.ref CMakeLists.txt number_compare.pl

