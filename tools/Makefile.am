
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ -DYYDEBUG
AM_CPPFLAGS = -DECCODES_SAMPLES_PATH=\"$(ECCODES_SAMPLES_PATH)\" \
	-DECCODES_DEFINITION_PATH=\"$(ECCODES_DEFINITION_PATH)\"

AM_YFLAGS = -d

noinst_LTLIBRARIES = libgrib_tools.la

libgrib_tools_la_SOURCES = grib_tools.c \
                        grib_options.c

dist_bin_SCRIPTS = bufr_compare_dir

bin_PROGRAMS = grib_histogram big2gribex \
               codes_info codes_count grib_filter grib_ls grib_dump grib_merge \
               grib2ppm grib_set grib_get grib_get_data grib_copy grib_repair \
               grib_compare grib_list_keys codes_parser grib_count grib_index_build grib1to2 \
               gg_sub_area_check grib_to_netcdf grib_to_json \
               bufr_count codes_bufr_filter bufr_ls bufr_dump bufr_set bufr_get \
               bufr_copy bufr_compare bufr_index_build \
               gts_copy gts_dump gts_filter gts_ls \
							 metar_dump metar_ls

noinst_PROGRAMS =  gaussian mars_request xref #compile #dumpload grib_compare_old

#test_SOURCES   =  test.c

#dumpload_SOURCES   =  dumpload.c loady.y loadl.l load.c dump.c load.h

grib_merge_SOURCES            =  grib_merge.c
grib_to_netcdf_SOURCES        =  grib_to_netcdf.c
grib_to_json_SOURCES          =  grib_to_json.c
grib_dump_SOURCES             =  grib_dump.c
grib_count_SOURCES            =  codes_count.c
bufr_count_SOURCES            =  codes_count.c
codes_count_SOURCES           =  codes_count.c
grib_histogram_SOURCES        =  grib_histogram.c
codes_parser_SOURCES          =  codes_parser.c
grib_list_keys_SOURCES        =  list_keys.c
grib2ppm_SOURCES              =  grib2ppm.c
codes_info_SOURCES            =  codes_info.c
grib_ls_SOURCES               =  grib_ls.c
grib_copy_SOURCES             =  grib_copy.c
grib_repair_SOURCES           =  grib_repair.c
grib_filter_SOURCES           =  grib_filter.c
grib_set_SOURCES              =  grib_set.c
grib_index_build_SOURCES      =  grib_index_build.c
grib_get_SOURCES              =  grib_get.c
grib_get_data_SOURCES         =  grib_get_data.c
grib_compare_SOURCES          =  grib_compare.c 
big2gribex_SOURCES            =  big2gribex.c
mars_request_SOURCES          =  mars_request.c
xref_SOURCES                  =  xref.c
#compile_SOURCES              =  compile.c
gaussian_SOURCES              =  gaussian.c
gg_sub_area_check_SOURCES     =  gg_sub_area_check.c
grib1to2_SOURCES              =  grib1to2.in

codes_bufr_filter_SOURCES   = bufr_filter.c
bufr_ls_SOURCES   = bufr_ls.c
bufr_dump_SOURCES   = bufr_dump.c
bufr_set_SOURCES   = bufr_set.c
bufr_get_SOURCES   = bufr_get.c
bufr_copy_SOURCES   = bufr_copy.c
bufr_compare_SOURCES   = bufr_compare.c
bufr_index_build_SOURCES   = bufr_index_build.c

gts_copy_SOURCES = gts_copy.c
gts_dump_SOURCES = gts_dump.c
gts_filter_SOURCES = gts_filter.c
#gts_get_SOURCES = gts_get.c
gts_ls_SOURCES = gts_ls.c

metar_dump_SOURCES = metar_dump.c
metar_ls_SOURCES = metar_ls.c

INCLUDES =  -I$(top_builddir)/src

LDADD = libgrib_tools.la $(top_builddir)/src/libeccodes.la
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
