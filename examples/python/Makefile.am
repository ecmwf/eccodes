if WITH_PYTHON
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = clone.sh count_messages.sh get.sh index.sh iterator.sh keys_iterator.sh multi_write.sh nearest.sh print_data.sh \
        samples.sh set.sh set_missing.sh binary_message.sh set_bitmap.sh
TESTS_ENVIRONMENT = TOPBUILDDIR=$(top_builddir) PYTHON=$(PYTHON)

noinst_PROGRAMS = p_keys_iterator p_print_data p_iterator p_count_messages
p_keys_iterator_SOURCES = keys_iterator.c
p_print_data_SOURCES = print_data.c
p_iterator_SOURCES = iterator.c
p_count_messages_SOURCES = count_messages.c
INCLUDES =  -I$(top_builddir)/src
LDADD = $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST = $(TESTS) include.sh clone.py count_messages.py get.py index.py iterator.py keys_iterator.py multi_write.py \
             nearest.py print_data.py samples.py set.py set_missing.py binary_message.py set_pv.py set_bitmap.py \
             CMakeLists.txt include.ctest.sh.in
endif
