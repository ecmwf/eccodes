
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = iterator.sh get.sh print_data.sh set.sh keys_iterator.sh multi.sh precision.sh \
        iterator_fortran.sh get_fortran.sh print_data_fortran.sh set_fortran.sh keys_iterator_fortran.sh \
		multi_fortran.sh precision_fortran.sh

noinst_PROGRAMS        =  nearest set_bitmap iterator get print_data set keys_iterator multi precision set_pv\
                       iterator_fortran get_fortran print_data_fortran set_fortran keys_iterator_fortran \
					   multi_fortran precision_fortran set_pv_fortran

nearest_SOURCES               =   nearest.c
get_SOURCES               =   get.c
print_data_SOURCES        =   print_data.c
set_SOURCES               =   set.c
set_bitmap_SOURCES        =   set_bitmap.c
multi_SOURCES             =   multi.c
keys_iterator_SOURCES     =   keys_iterator.c
iterator_SOURCES          =   iterator.c
set_pv_SOURCES          =   set_pv.c

get_fortran_SOURCES               =   get_fortran.F
print_data_fortran_SOURCES        =   print_data_fortran.F
set_fortran_SOURCES               =   set_fortran.F
iterator_fortran_SOURCES          =   iterator_fortran.F
keys_iterator_fortran_SOURCES     =   keys_iterator_fortran.F
multi_fortran_SOURCES             =   multi_fortran.F
precision_fortran_SOURCES         =   precision_fortran.F
set_pv_fortran_SOURCES          =   set_pv_fortran.F

INCLUDES =  -I$(top_builddir)/src

LDADD = $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh CMakeLists.txt

