
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = grib_iterator.sh grib_get_keys.sh grib_print_data.sh grib_set_keys.sh \
        grib_keys_iterator.sh grib_multi.sh grib_multi_write.sh grib_precision.sh \
        list.sh large_grib1.sh get_data.sh sections_copy.sh \
        set_missing.sh grib_clone.sh grib_set_pv.sh \
        check_gaussian_grids.sh bufr_attributes.sh bufr_clone.sh bufr_expanded.sh \
        bufr_get_keys.sh bufr_read_header.sh bufr_read_synop.sh \
        bufr_set_keys.sh bufr_subset.sh bufr_keys_iterator.sh bufr_missing.sh \
        bufr_read_temp.sh get_product_kind.sh

noinst_PROGRAMS  = grib_nearest grib_set_bitmap grib_iterator grib_get_keys grib_print_data \
                   grib_set_keys set_missing grib_keys_iterator \
                   grib_set_data mars_param values_check box grib_multi multi2 grib_multi_write grib_precision \
                   grib_set_pv list sections_copy large_grib1 get_data grib_iterator_bitmap grib_clone new_sample \
                   check_gaussian_grid grib_ensemble_index points \
                   bufr_attributes bufr_clone bufr_expanded  bufr_get_keys bufr_read_header bufr_read_synop \
                   bufr_get_keys bufr_subset bufr_keys_iterator bufr_set_keys bufr_missing bufr_read_temp \
                   get_product_kind

#bin_PROGRAMS            = points

box_SOURCES                  = box.c
grib_set_data_SOURCES        = grib_set_data.c
mars_param_SOURCES           = mars_param.c
values_check_SOURCES         = values_check.c
grib_nearest_SOURCES         = grib_nearest.c
grib_multi_write_SOURCES     = grib_multi_write.c
grib_get_keys_SOURCES        = grib_get_keys.c
grib_print_data_SOURCES      = grib_print_data.c
grib_set_keys_SOURCES        = grib_set_keys.c
set_missing_SOURCES          = set_missing.c
grib_set_bitmap_SOURCES      = grib_set_bitmap.c
grib_multi_SOURCES           = grib_multi.c
multi2_SOURCES               = multi2.c
grib_keys_iterator_SOURCES   = grib_keys_iterator.c
grib_iterator_SOURCES        = grib_iterator.c
grib_iterator_bitmap_SOURCES = grib_iterator_bitmap.c
grib_set_pv_SOURCES          = grib_set_pv.c
points_SOURCES               = points.c
list_SOURCES                 = list.c
sections_copy_SOURCES        = sections_copy.c
large_grib1_SOURCES          = large_grib1.c
get_data_SOURCES             = get_data.c
grib_clone_SOURCES           = grib_clone.c
new_sample_SOURCES           = new_sample.c
check_gaussian_grid_SOURCES  = check_gaussian_grid.c
grib_ensemble_index_SOURCES  = grib_ensemble_index.c
bufr_attributes_SOURCES      = bufr_attributes.c
bufr_clone_SOURCES           = bufr_clone.c
bufr_expanded_SOURCES        = bufr_expanded.c
bufr_get_keys_SOURCES        = bufr_get_keys.c
bufr_keys_iterator           = bufr_keys_iterator.c
bufr_missing                 = bufr_missing.c
bufr_read_header_SOURCES     = bufr_read_header.c
bufr_read_synop_SOURCES      = bufr_read_synop.c
bufr_read_temp_SOURCES       = bufr_read_temp.c
bufr_set_keys_SOURCES        = bufr_set_keys.c
bufr_subset_SOURCES          = bufr_subset.c
get_product_kind_SOURCES     = get_product_kind.c

INCLUDES =  -I$(top_builddir)/src

LDADD = $(top_builddir)/src/libeccodes.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh CMakeLists.txt include.ctest.sh.in

