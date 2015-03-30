# See http://www.delorie.com/gnu/docs/automake/automake_48.html

AM_CFLAGS =  @WARN_PEDANTIC@

lib_LTLIBRARIES = libeccodes_f90.la

libeccodes_f90_la_SOURCES= grib_fortran.c grib_f90.f90 eccodes_f90.f90
libeccodes_f90_la_DEPENDENCIES = $(top_builddir)/src/libeccodes.la grib_api_externals.h grib_api_visibility.h grib_api_constants.h grib_kinds.h
libeccodes_f90_la_LDFLAGS = -version-info $(ECCODES_CURRENT):$(ECCODES_REVISION):$(ECCODES_AGE)

libeccodes_fortran_prototypes= grib_fortran.c

if  UPPER_CASE_MOD_FALSE
nodist_include_HEADERS = grib_api.mod eccodes.mod
grib_api.mod: grib_f90.o
eccodes.mod: eccodes_f90.o
else
nodist_include_HEADERS = GRIB_API.mod ECCODES.mod
GRIB_API.mod: grib_f90.o
ECCODES.mod: eccodes_f90.o
endif

# set the include path
INCLUDES=  -I$(top_builddir)/src

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libeccodes_f90.la grib_f90.f90 eccodes_f90.f90 *.mod grib_types grib_kinds.h same_int_long same_int_size_t

#noinst_HEADERS = 

EXTRA_DIST= grib_fortran_prototypes.h grib_api_constants.h grib_api_externals.h \
            grib_api_visibility.h grib_types.f90 create_grib_f90.sh \
            grib_f90_head.f90 grib_f90_tail.f90 grib_f90_int.f90 grib_f90_long_int.f90 \
            grib_f90_int_size_t.f90 grib_f90_long_size_t.f90 \
            \
            eccodes_f90_head.f90 eccodes_f90_tail.f90 eccodes_f90_int.f90 eccodes_f90_long_int.f90 \
            eccodes_f90_int_size_t.f90 eccodes_f90_long_size_t.f90 \
            same_int_long.f90 same_int_size_t.f90 grib_fortran_kinds.c \
            CMakeLists.txt

grib_f90.f90: grib_f90_head.f90 grib_f90_tail.f90 grib_f90_int.f90 grib_f90_long_int.f90 grib_f90_int_size_t.f90 grib_f90_long_size_t.f90 same_int_long same_int_size_t grib_kinds.h
	./create_grib_f90.sh

eccodes_f90.f90: eccodes_f90_head.f90 eccodes_f90_tail.f90 eccodes_f90_int.f90 eccodes_f90_long_int.f90 eccodes_f90_int_size_t.f90 eccodes_f90_long_size_t.f90 same_int_long same_int_size_t grib_kinds.h
	./create_grib_f90.sh


grib_f90.o : grib_kinds.h

eccodes_f90.o : grib_kinds.h

grib_kinds.h: grib_types
	./grib_types > grib_kinds.h

grib_types: grib_types.o grib_fortran_kinds.o
	$(FC) $(FCFLAGS) -o grib_types grib_types.o grib_fortran_kinds.o

same_int_long: same_int_long.o grib_fortran_kinds.o
	$(FC) $(FCFLAGS) -o same_int_long same_int_long.o grib_fortran_kinds.o

same_int_size_t: same_int_size_t.o grib_fortran_kinds.o
	$(FC) $(FCFLAGS) -o same_int_size_t same_int_size_t.o grib_fortran_kinds.o

# Note: hooks are always executed after the main rule's work is done
#install-data-hook:
#	( some commands in here .... )

include extrules.am

