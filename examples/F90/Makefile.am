
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = grib_copy_message.sh grib_get_keys.sh grib_get_data.sh get_pl.sh get_pv.sh grib_keys_iterator.sh \
        grib_nearest.sh grib_precision.sh grib_multi_write.sh grib_multi.sh \
        grib_print_data.sh grib_set_keys.sh \
        grib_set_bitmap.sh grib_set_missing.sh grib_set_pv.sh grib_samples.sh grib_count_messages.sh \
        read_message.sh grib_count_messages_multi.sh \
        read_from_file.sh grib_index.sh get_set_uuid.sh \
        bufr_attributes.sh grib_clone.sh bufr_clone.sh \
        bufr_expanded.sh bufr_get_keys.sh bufr_get_string_array.sh \
        bufr_read_header.sh bufr_read_synop.sh \
        bufr_set_keys.sh bufr_keys_iterator.sh bufr_subset.sh \
        get_product_kind.sh bufr_read_temp.sh \
        bufr_read_tropical_cyclone.sh bufr_read_scatterometer.sh

noinst_PROGRAMS  = eccodes_f_grib_index \
                   eccodes_f_grib_copy_message \
                   eccodes_f_grib_get_keys \
                   eccodes_f_grib_get_data \
                   eccodes_f_get_pl \
                   eccodes_f_get_pv \
                   eccodes_f_grib_keys_iterator \
                   eccodes_f_grib_multi_write \
                   eccodes_f_grib_multi \
                   eccodes_f_grib_nearest \
                   eccodes_f_grib_precision \
                   eccodes_f_grib_print_data \
                   eccodes_f_grib_print_data_static \
                   eccodes_f_grib_set_keys \
                   eccodes_f_grib_set_bitmap \
                   eccodes_f_grib_set_missing \
                   eccodes_f_grib_set_pv \
                   eccodes_f_grib_samples \
                   eccodes_f_grib_count_messages \
                   eccodes_f_grib_count_messages_multi \
                   eccodes_f_read_message \
                   eccodes_f_read_from_file \
                   eccodes_f_new_from_file \
                   eccodes_f_grib_copy_namespace \
                   eccodes_f_get_set_uuid \
                   eccodes_f_grib_set_gvc \
                   eccodes_f_grib_clone \
                   eccodes_f_bufr_clone \
                   eccodes_f_bufr_expanded \
                   eccodes_f_bufr_get_keys \
                   eccodes_f_bufr_get_string_array \
                   eccodes_f_bufr_read_header \
                   eccodes_f_bufr_read_synop \
                   eccodes_f_bufr_set_keys \
                   eccodes_f_bufr_keys_iterator \
                   eccodes_f_bufr_subset \
                   eccodes_f_bufr_attributes \
                   eccodes_f_get_product_kind \
                   eccodes_f_bufr_read_temp \
                   eccodes_f_bufr_read_tropical_cyclone \
                   eccodes_f_bufr_read_scatterometer

eccodes_f_grib_index_SOURCES=grib_index.f90
eccodes_f_grib_copy_message_SOURCES=grib_copy_message.f90
eccodes_f_grib_get_keys_SOURCES=grib_get_keys.f90
eccodes_f_grib_get_data_SOURCES=grib_get_data.f90
eccodes_f_get_pl_SOURCES=get_pl.f90
eccodes_f_get_pv_SOURCES=get_pv.f90
eccodes_f_grib_keys_iterator_SOURCES=grib_keys_iterator.f90
eccodes_f_grib_multi_write_SOURCES=grib_multi_write.f90
eccodes_f_grib_multi_SOURCES=grib_multi.f90
eccodes_f_grib_nearest_SOURCES=grib_nearest.f90
eccodes_f_grib_precision_SOURCES=grib_precision.f90
eccodes_f_grib_print_data_SOURCES=grib_print_data.f90
eccodes_f_grib_print_data_static_SOURCES=grib_print_data_static.f90
eccodes_f_grib_set_keys_SOURCES=grib_set_keys.f90
eccodes_f_grib_set_bitmap_SOURCES=grib_set_bitmap.f90
eccodes_f_grib_set_missing_SOURCES=grib_set_missing.f90
eccodes_f_grib_set_pv_SOURCES=grib_set_pv.f90
eccodes_f_grib_samples_SOURCES=grib_samples.f90
eccodes_f_grib_count_messages_SOURCES=grib_count_messages.f90
eccodes_f_grib_count_messages_multi_SOURCES=grib_count_messages_multi.f90
eccodes_f_read_message_SOURCES=read_message.f90
eccodes_f_read_from_file_SOURCES=read_from_file.f90
eccodes_f_new_from_file_SOURCES=new_from_file.f90
eccodes_f_grib_copy_namespace_SOURCES=grib_copy_namespace.f90
eccodes_f_get_set_uuid_SOURCES=get_set_uuid.f90
eccodes_f_grib_set_gvc_SOURCES=grib_set_gvc.f90
eccodes_f_grib_clone_SOURCES=grib_clone.f90
eccodes_f_bufr_attributes_SOURCES=bufr_attributes.f90
eccodes_f_bufr_clone_SOURCES=bufr_clone.f90
eccodes_f_bufr_expanded_SOURCES=bufr_expanded.f90
eccodes_f_bufr_get_keys_SOURCES=bufr_get_keys.f90
eccodes_f_bufr_get_string_array_SOURCES=bufr_get_string_array.f90
eccodes_f_bufr_keys_iterator_SOURCES=bufr_keys_iterator.f90
eccodes_f_bufr_read_header_SOURCES=bufr_read_header.f90
eccodes_f_bufr_read_scatterometer_SOURCES=bufr_read_scatterometer.f90
eccodes_f_bufr_read_synop_SOURCES=bufr_read_synop.f90
eccodes_f_bufr_read_temp_SOURCES=bufr_read_temp.f90
eccodes_f_bufr_set_keys_SOURCES=bufr_set_keys.f90
eccodes_f_bufr_subset_SOURCES=bufr_subset.f90
eccodes_f_get_product_kind_SOURCES=get_product_kind.f90
eccodes_f_bufr_read_tropical_cyclone_SOURCES=bufr_read_tropical_cyclone.f90

INCLUDES =  -I$(top_builddir)/src

AM_FCFLAGS = $(F90_MODULE_FLAG)$(top_builddir)/fortran

LDADD = $(top_builddir)/fortran/libeccodes_f90.la $(top_builddir)/src/libeccodes.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh CMakeLists.txt include.ctest.sh.in

