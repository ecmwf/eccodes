
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = grib_iterator.sh \
        grib_get_keys.sh \
        grib_print_data.sh \
        grib_set_keys.sh \
        grib_keys_iterator.sh \
        grib_multi.sh \
        grib_multi_write.sh \
        grib_precision.sh \
        grib_list.sh \
        large_grib1.sh \
        grib_get_data.sh \
        sections_copy.sh \
        set_missing.sh \
        grib_clone.sh \
        grib_set_pv.sh \
        grib_set_bitmap.sh \
        bufr_attributes.sh \
        bufr_clone.sh \
        bufr_expanded.sh \
        bufr_get_keys.sh \
        bufr_read_header.sh \
        bufr_read_scatterometer.sh \
        bufr_read_synop.sh \
        bufr_set_keys.sh \
        bufr_subset.sh \
        bufr_keys_iterator.sh \
        bufr_missing.sh \
        bufr_read_temp.sh \
        get_product_kind.sh

noinst_PROGRAMS  = c_grib_nearest \
                   c_grib_set_bitmap \
                   c_grib_iterator \
                   c_grib_get_keys \
                   c_grib_print_data \
                   c_grib_set_keys \
                   c_set_missing \
                   c_grib_keys_iterator \
                   c_grib_set_data \
                   c_grib_index \
                   c_mars_param \
                   c_values_check \
                   c_box \
                   c_grib_multi \
                   c_multi2 \
                   c_grib_multi_write \
                   c_grib_precision \
                   c_grib_set_pv \
                   c_grib_list \
                   c_sections_copy \
                   c_large_grib1 \
                   c_grib_get_data \
                   c_grib_iterator_bitmap \
                   c_grib_clone \
                   c_new_sample \
                   c_grib_ensemble_index \
                   c_points \
                   c_bufr_attributes \
                   c_bufr_clone \
                   c_bufr_expanded \
                   c_bufr_get_keys \
                   c_bufr_read_header \
                   c_bufr_read_scatterometer \
                   c_bufr_read_synop \
                   c_bufr_get_keys \
                   c_bufr_subset \
                   c_bufr_keys_iterator \
                   c_bufr_set_keys \
                   c_bufr_missing \
                   c_bufr_read_temp \
                   c_get_product_kind

#bin_PROGRAMS            = points

c_box_SOURCES                  = box.c
c_grib_set_data_SOURCES        = grib_set_data.c
c_mars_param_SOURCES           = mars_param.c
c_values_check_SOURCES         = values_check.c
c_grib_nearest_SOURCES         = grib_nearest.c
c_grib_multi_write_SOURCES     = grib_multi_write.c
c_grib_get_keys_SOURCES        = grib_get_keys.c
c_grib_print_data_SOURCES      = grib_print_data.c
c_grib_set_keys_SOURCES        = grib_set_keys.c
c_set_missing_SOURCES          = set_missing.c
c_grib_index_SOURCES           = grib_index.c
c_grib_set_bitmap_SOURCES      = grib_set_bitmap.c
c_grib_multi_SOURCES           = grib_multi.c
c_multi2_SOURCES               = multi2.c
c_grib_keys_iterator_SOURCES   = grib_keys_iterator.c
c_grib_iterator_SOURCES        = grib_iterator.c
c_grib_iterator_bitmap_SOURCES = grib_iterator_bitmap.c
c_grib_set_pv_SOURCES          = grib_set_pv.c
c_points_SOURCES               = points.c
c_grib_list_SOURCES            = grib_list.c
c_sections_copy_SOURCES        = sections_copy.c
c_large_grib1_SOURCES          = large_grib1.c
c_grib_precision_SOURCES       = grib_precision.c
c_grib_get_data_SOURCES        = grib_get_data.c
c_grib_clone_SOURCES           = grib_clone.c
c_new_sample_SOURCES           = new_sample.c
c_grib_ensemble_index_SOURCES  = grib_ensemble_index.c
c_bufr_attributes_SOURCES      = bufr_attributes.c
c_bufr_clone_SOURCES           = bufr_clone.c
c_bufr_expanded_SOURCES        = bufr_expanded.c
c_bufr_get_keys_SOURCES        = bufr_get_keys.c
c_bufr_keys_iterator_SOURCES   = bufr_keys_iterator.c
c_bufr_missing_SOURCES         = bufr_missing.c
c_bufr_read_header_SOURCES     = bufr_read_header.c
c_bufr_read_scatterometer_SOURCES = bufr_read_scatterometer.c
c_bufr_read_synop_SOURCES      = bufr_read_synop.c
c_bufr_read_temp_SOURCES       = bufr_read_temp.c
c_bufr_set_keys_SOURCES        = bufr_set_keys.c
c_bufr_subset_SOURCES          = bufr_subset.c
c_get_product_kind_SOURCES     = get_product_kind.c

INCLUDES =  -I$(top_builddir)/src

LDADD = $(top_builddir)/src/libeccodes.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh CMakeLists.txt include.ctest.sh.in

