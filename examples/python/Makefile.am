if WITH_PYTHON
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = grib_clone.sh \
        grib_count_messages.sh \
        grib_get_keys.sh \
        grib_index.sh \
        grib_iterator.sh \
        grib_keys_iterator.sh \
        grib_multi_write.sh \
        grib_nearest.sh \
        grib_print_data.sh \
        grib_samples.sh \
        grib_set_keys.sh \
        grib_set_missing.sh \
        binary_message.sh \
        grib_set_bitmap.sh \
        bufr_read_header.sh \
        bufr_read_synop.sh \
        bufr_clone.sh \
        bufr_get_keys.sh \
        bufr_set_keys.sh \
        bufr_expanded.sh \
        bufr_keys_iterator.sh \
        bufr_subset.sh \
        bufr_attributes.sh \
        get_product_kind.sh \
        bufr_read_temp.sh \
        bufr_read_scatterometer.sh \
        gts_get_keys.sh \
        metar_get_keys.sh

TESTS_ENVIRONMENT = TOPBUILDDIR=$(top_builddir) PYTHON=$(PYTHON)

noinst_PROGRAMS = p_grib_keys_iterator p_grib_print_data p_grib_iterator p_grib_count_messages
p_grib_keys_iterator_SOURCES = grib_keys_iterator.c
p_grib_print_data_SOURCES = grib_print_data.c
p_grib_iterator_SOURCES = grib_iterator.c
p_grib_count_messages_SOURCES = grib_count_messages.c
INCLUDES =  -I$(top_builddir)/src
LDADD = $(top_builddir)/src/libeccodes.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST = $(TESTS) include.sh CMakeLists.txt include.ctest.sh.in \
             grib_clone.py \
             grib_count_messages.py \
             grib_get_keys.py \
             grib_index.py \
             grib_iterator.py \
             grib_keys_iterator.py \
             grib_multi_write.py \
             grib_nearest.py \
             grib_print_data.py \
             grib_samples.py \
             grib_set_keys.py \
             grib_set_missing.py \
             binary_message.py \
             grib_set_pv.py \
             grib_set_bitmap.py \
             bufr_read_header.py \
             bufr_read_synop.py \
             bufr_clone.py \
             bufr_get_keys.py \
             bufr_set_keys.py \
             bufr_expanded.py \
             bufr_keys_iterator.py \
             bufr_subset.py \
             bufr_attributes.py \
             get_product_kind.py \
             bufr_read_temp.py \
             bufr_read_scatterometer.py \
             gts_get_keys.py \
             metar_get_keys.py

endif

