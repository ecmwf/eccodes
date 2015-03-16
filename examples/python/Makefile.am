if WITH_PYTHON
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = clone.sh count_messages.sh get.sh index.sh iterator.sh keys_iterator.sh multi_write.sh nearest.sh print_data.sh \
        samples.sh set.sh set_missing.sh binary_message.sh set_bitmap.sh bufr_print_header.sh bufr_print_data.sh \
        bufr_clone.sh bufr_get_keys.sh bufr_set_keys.sh bufr_expanded.sh  bufr_keys_iterator.sh bufr_subset.sh \
        bufr_attributes.sh get_product_kind.sh bufr_read_temp.sh
TESTS_ENVIRONMENT = TOPBUILDDIR=$(top_builddir) PYTHON=$(PYTHON)

noinst_PROGRAMS = p_keys_iterator p_print_data p_iterator p_count_messages
p_keys_iterator_SOURCES = keys_iterator.c
p_print_data_SOURCES = print_data.c
p_iterator_SOURCES = iterator.c
p_count_messages_SOURCES = count_messages.c
INCLUDES =  -I$(top_builddir)/src
LDADD = $(top_builddir)/src/libeccodes.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST = $(TESTS) include.sh clone.py count_messages.py get.py index.py iterator.py keys_iterator.py multi_write.py \
             nearest.py print_data.py samples.py set.py set_missing.py binary_message.py set_pv.py set_bitmap.py \
             bufr_print_header.py  bufr_print_data.py  bufr_clone.py bufr_get_keys.py bufr_set_keys.py \
             bufr_expanded.py  bufr_keys_iterator.py bufr_subset.py bufr_attributes.py \
             get_product_kind.py bufr_read_temp.py \
             CMakeLists.txt include.ctest.sh.in 
endif
