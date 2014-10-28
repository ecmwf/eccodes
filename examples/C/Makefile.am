
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = iterator.sh get.sh print_data.sh set.sh keys_iterator.sh multi.sh multi_write.sh \
        precision.sh list.sh large_grib1.sh get_data.sh sections_copy.sh set_missing.sh clone.sh set_pv.sh \
        check_gaussian_grids.sh

noinst_PROGRAMS  = nearest set_bitmap iterator get print_data set set_missing keys_iterator \
                   set_data mars_param values_check box multi multi2 multi_write precision \
                   set_pv list sections_copy large_grib1 get_data iterator_bitmap clone new_sample \
                   check_gaussian_grid ensemble_index
bin_PROGRAMS            = points

box_SOURCES             = box.c
set_data_SOURCES        = set_data.c
mars_param_SOURCES      = mars_param.c
values_check_SOURCES    = values_check.c
nearest_SOURCES         = nearest.c
multi_write_SOURCES     = multi_write.c
get_SOURCES             = get.c
print_data_SOURCES      = print_data.c
set_SOURCES             = set.c
set_missing_SOURCES     = set_missing.c
set_bitmap_SOURCES      = set_bitmap.c
multi_SOURCES           = multi.c
multi2_SOURCES          = multi2.c
keys_iterator_SOURCES   = keys_iterator.c
iterator_SOURCES        = iterator.c
iterator_bitmap_SOURCES = iterator_bitmap.c
set_pv_SOURCES          = set_pv.c
points_SOURCES          = points.c
list_SOURCES            = list.c
sections_copy_SOURCES   = sections_copy.c
large_grib1_SOURCES     = large_grib1.c
get_data_SOURCES        = get_data.c
clone_SOURCES           = clone.c
new_sample_SOURCES      = new_sample.c
check_gaussian_grid_SOURCES = check_gaussian_grid.c
ensemble_index_SOURCES = ensemble_index.c

INCLUDES =  -I$(top_builddir)/src

LDADD = $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh CMakeLists.txt include.ctest.sh.in

