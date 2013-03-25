
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = iterator_fortran.sh get_fortran.sh print_data_fortran.sh set_fortran.sh keys_iterator_fortran.sh \
		multi_fortran.sh precision_fortran.sh

noinst_PROGRAMS        = iterator_fortran get_fortran print_data_fortran set_fortran set_missing_fortran keys_iterator_fortran \
					   multi_fortran precision_fortran set_pv_fortran

get_fortran_SOURCES               =   get_fortran.F
print_data_fortran_SOURCES        =   print_data_fortran.F
set_fortran_SOURCES               =   set_fortran.F
set_missing_fortran_SOURCES               =   set_missing_fortran.F
iterator_fortran_SOURCES          =   iterator_fortran.F
keys_iterator_fortran_SOURCES     =   keys_iterator_fortran.F
multi_fortran_SOURCES             =   multi_fortran.F
precision_fortran_SOURCES         =   precision_fortran.F
set_pv_fortran_SOURCES          =   set_pv_fortran.F

INCLUDES =  -I$(top_builddir)/src -I$(top_builddir)/fortran

LDADD = $(top_builddir)/fortran/libgrib_api_f77.a $(top_builddir)/src/libgrib_api.a
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh

