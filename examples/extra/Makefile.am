
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = multi_support.sh message_count.sh f_clone.sh c_clone.sh ens_mean.sh std_dev.sh

noinst_PROGRAMS        =  message_count_multi c_clone f_clone ens_mean std_dev

c_clone_SOURCES                     =   c_clone.c
f_clone_SOURCES                     =   f_clone.f90
message_count_multi_SOURCES         =   message_count_multi.f90
ens_mean_SOURCES                    =   ens_mean.f90
std_dev_SOURCES                     =   std_dev.f90

INCLUDES =  -I$(top_builddir)/src

AM_FCFLAGS = $(F90_MODULE_FLAG)$(top_builddir)/fortran

LDADD = $(top_builddir)/fortran/libgrib_api_f90.a $(top_builddir)/src/libgrib_api.a
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh

