
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ -DYYDEBUG
AM_CPPFLAGS = -DGRIB_TEMPLATES_PATH=\"$(GRIB_TEMPLATES_PATH)\" \
	-DGRIB_SAMPLES_PATH=\"$(GRIB_SAMPLES_PATH)\" \
	-DGRIB_DEFINITION_PATH=\"$(GRIB_DEFINITION_PATH)\"

AM_YFLAGS = -d

noinst_LTLIBRARIES = libgrib_tools.la

libgrib_tools_la_SOURCES = grib_tools.c \
                        grib_options.c

dist_bin_SCRIPTS = bufr_compare_dir

bin_PROGRAMS = grib_keys grib_histogram grib_error grib_add big2gribex \
               grib_info grib_filter grib_ls grib_dump grib_merge \
               grib2ppm grib_set grib_get grib_get_data grib_copy grib_repair \
               grib_packing grib_distance grib_corruption_check\
               grib_compare grib_list_keys parser grib_count grib_index_build grib1to2 \
               gg_sub_area_check grib_moments grib_to_netcdf grib_to_json \
               bufr_filter bufr_ls bufr_dump bufr_set bufr_get \
               bufr_copy bufr_compare bufr_index_build \
               taf_dump taf_ls taf_get taf_filter \
               metar_dump metar_ls metar_get metar_filter

noinst_PROGRAMS =  gaussian grib_diff mars_request xref all_keys #compile #dumpload grib_compare_old

#test_SOURCES   =  test.c

#dumpload_SOURCES   =  dumpload.c loady.y loadl.l load.c dump.c load.h

grib_corruption_check_SOURCES =  grib_corruption_check.c
grib_moments_SOURCES          =  grib_moments.c
grib_merge_SOURCES            =  grib_merge.c
grib_to_netcdf_SOURCES        =  grib_to_netcdf.c
grib_to_json_SOURCES          =  grib_to_json.c
grib_dump_SOURCES             =  grib_dump.c
grib_count_SOURCES            =  grib_count.c
grib_packing_SOURCES          =  grib_packing.c
grib_histogram_SOURCES        =  grib_histogram.c
grib_distance_SOURCES         =  grib_distance.c
#grib_debug_SOURCES            =  grib_debug.c
parser_SOURCES                =  parser.c
grib_list_keys_SOURCES        =  list_keys.c
all_keys_SOURCES              =  all_keys.c
grib2ppm_SOURCES              =  grib2ppm.c
grib_info_SOURCES             =  grib_info.c
grib_keys_SOURCES             =  grib_keys.c
grib_ls_SOURCES               =  grib_ls.c
grib_copy_SOURCES             =  grib_copy.c
grib_repair_SOURCES           =  grib_repair.c
grib_filter_SOURCES           =  grib_filter.c
grib_set_SOURCES              =  grib_set.c
grib_index_build_SOURCES      =  grib_index_build.c
grib_get_SOURCES              =  grib_get.c
grib_get_data_SOURCES         =  grib_get_data.c
grib_add_SOURCES              =  grib_add.c
grib_error_SOURCES            =  grib_error.c
grib_diff_SOURCES             =  grib_diff.c
grib_compare_SOURCES          =  grib_compare.c 
big2gribex_SOURCES            =  big2gribex.c
mars_request_SOURCES          =  mars_request.c
xref_SOURCES                  =  xref.c
#compile_SOURCES              =  compile.c
gaussian_SOURCES              =  gaussian.c
gg_sub_area_check_SOURCES     =  gg_sub_area_check.c
grib1to2_SOURCES              = grib1to2.in

bufr_filter_SOURCES   = bufr_filter.c
bufr_ls_SOURCES   = bufr_ls.c
bufr_dump_SOURCES   = bufr_dump.c
bufr_set_SOURCES   = bufr_set.c
bufr_get_SOURCES   = bufr_get.c
bufr_copy_SOURCES   = bufr_copy.c
bufr_compare_SOURCES   = bufr_compare.c
bufr_index_build_SOURCES   = bufr_index_build.c

metar_dump_SOURCES   = metar_dump.c
metar_ls_SOURCES   = metar_ls.c
metar_get_SOURCES   = metar_get.c
metar_filter_SOURCES   = metar_filter.c

taf_dump_SOURCES   = taf_dump.c
taf_ls_SOURCES   = taf_ls.c
taf_get_SOURCES   = taf_get.c
taf_filter_SOURCES   = taf_filter.c

INCLUDES =  -I$(top_builddir)/src

LDADD = libgrib_tools.la $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

grib_to_netcdf_LDADD = $(LDADD) $(NETCDF_LDFLAGS) 

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libgrib_tools.la

grib1to2$(EXEEXT): grib1to2.in
	rm -f grib1to2
	sed "s|@CMAKE_INSTALL_PREFIX@/bin|$(bindir)|" grib1to2.in > grib1to2
	chmod +x grib1to2

# wingetopt files are for Microsoft Windows Visual Studio support
noinst_HEADERS = grib_tools.h wingetopt.h wingetopt.c

EXTRA_DIST= CMakeLists.txt grib1to2.in
