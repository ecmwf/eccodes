
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ -DYYDEBUG

AM_YFLAGS = -d

noinst_LTLIBRARIES = libgrib_tools.la

libgrib_tools_la_SOURCES = grib_tools.c \
                        grib_options.c


bin_PROGRAMS = grib_keys grib_histogram grib_error grib_add big2gribex \
               grib_debug grib_info grib_filter grib_ls grib_dump \
			   grib2ppm grib_set grib_get grib_get_data grib_copy grib_repair \
			   grib_packing grib_cmp grib_convert  grib_distance grib_corruption_check\
			   grib_compare grib_list_keys parser grib_count grib_index_build grib1to2 \
			   gg_sub_area_check grib_moments grib_to_netcdf

noinst_PROGRAMS =  gaussian grib_diff mars_request xref all_keys #compile #dumpload grib_compare_old

#test_SOURCES   =  test.c

#dumpload_SOURCES   =  dumpload.c loady.y loadl.l load.c dump.c load.h

grib_corruption_check_SOURCES   =  grib_corruption_check.c
grib_moments_SOURCES   =  grib_moments.c
grib_to_netcdf_SOURCES   =  grib_to_netcdf.c
grib_dump_SOURCES   =  grib_dump.c
grib_count_SOURCES   =  grib_count.c
grib_cmp_SOURCES   =  grib_cmp.c
grib_packing_SOURCES   =  grib_packing.c
grib_histogram_SOURCES   =  grib_histogram.c
grib_distance_SOURCES   =  grib_distance.c
grib_debug_SOURCES   =  grib_debug.c
parser_SOURCES      =  parser.c
grib_list_keys_SOURCES      =  list_keys.c
all_keys_SOURCES      =  all_keys.c
grib2ppm_SOURCES    =  grib2ppm.c
grib_info_SOURCES   =  grib_info.c
grib_keys_SOURCES   =  grib_keys.c

grib_ls_SOURCES     =  grib_ls.c
grib_copy_SOURCES   =  grib_copy.c
grib_repair_SOURCES   =  grib_repair.c
grib_convert_SOURCES     =  grib_convert.c
grib_filter_SOURCES =  grib_filter.c
grib_set_SOURCES     =  grib_set.c
grib_index_build_SOURCES     =  grib_index_build.c
grib_get_SOURCES     =  grib_get.c
grib_get_data_SOURCES     =  grib_get_data.c

grib_add_SOURCES     =  grib_add.c
grib_error_SOURCES     =  grib_error.c
grib_diff_SOURCES     =  grib_diff.c
grib_compare_SOURCES     =  grib_compare.c 
big2gribex_SOURCES     =  big2gribex.c
mars_request_SOURCES     =  mars_request.c
xref_SOURCES     =  xref.c
#compile_SOURCES     =  compile.c
gaussian_SOURCES     =  gaussian.c
gg_sub_area_check_SOURCES     =  gg_sub_area_check.c
grib1to2_SOURCES = grib1to2.txt

INCLUDES =  -I$(top_builddir)/src

LDADD = libgrib_tools.la $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libgrib_tools.la

grib1to2$(EXEEXT): grib1to2.txt
	rm -f grib1to2
	sed "s:toolsdir:$(bindir):" grib1to2.txt > grib1to2
	chmod +x grib1to2

noinst_HEADERS = grib_tools.h
