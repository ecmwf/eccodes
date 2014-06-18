
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = copy_message.sh get.sh get_data.sh get_pl.sh get_pv.sh keys_iterator.sh \
        nearest.sh precision.sh multi_write.sh multi.sh print_data.sh set.sh set_bitmap.sh set_missing.sh \
        set_pv.sh samples.sh count_messages.sh read_message.sh read_from_file.sh index.sh get_set_uuid.sh

noinst_PROGRAMS  = f_index f_copy_message f_get f_get_data f_get_pl f_get_pv f_keys_iterator \
                   f_multi_write f_multi f_nearest f_precision f_print_data f_set f_set_bitmap f_set_missing \
                   f_set_pv f_samples f_count_messages f_read_message f_read_from_file f_new_from_file \
                   f_copy_namespace f_get_set_uuid f_set_gvc

f_index_SOURCES=index.f90
f_copy_message_SOURCES=copy_message.f90
f_get_SOURCES=get.f90
f_get_data_SOURCES=get_data.f90
f_get_pl_SOURCES=get_pl.f90
f_get_pv_SOURCES=get_pv.f90
f_keys_iterator_SOURCES=keys_iterator.f90
f_multi_write_SOURCES=multi_write.f90
f_multi_SOURCES=multi.f90
f_nearest_SOURCES=nearest.f90
f_precision_SOURCES=precision.f90
f_print_data_SOURCES=print_data.f90
f_set_SOURCES=set.f90
f_set_bitmap_SOURCES=set_bitmap.f90
f_set_missing_SOURCES=set_missing.f90
f_set_pv_SOURCES=set_pv.f90
f_samples_SOURCES=samples.f90
f_count_messages_SOURCES=count_messages.f90
f_read_message_SOURCES=read_message.f90
f_read_from_file_SOURCES=read_from_file.f90
f_new_from_file_SOURCES=new_from_file.f90
f_copy_namespace_SOURCES=copy_namespace.f90
f_get_set_uuid_SOURCES=get_set_uuid.f90
f_set_gvc_SOURCES=set_gvc.f90

INCLUDES =  -I$(top_builddir)/src

AM_FCFLAGS = $(F90_MODULE_FLAG)$(top_builddir)/fortran

LDADD = $(top_builddir)/fortran/libgrib_api_f90.la $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh CMakeLists.txt include.ctest.sh.in

