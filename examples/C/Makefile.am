
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@

TESTS = iterator.sh get.sh print_data.sh set.sh keys_iterator.sh multi.sh multi_write.sh \
		precision.sh list.sh

noinst_PROGRAMS        =  nearest set_bitmap iterator get print_data set set_missing keys_iterator \
							set_data mars_param values_check box nc multi multi_write precision set_pv list sections_copy
bin_PROGRAMS = points

box_SOURCES               =   box.c
set_data_SOURCES               =   set_data.c
mars_param_SOURCES               =   mars_param.c
values_check_SOURCES               =   values_check.c
nc_SOURCES               =   nc.c
nearest_SOURCES               =   nearest.c
multi_write_SOURCES               =   multi_write.c
get_SOURCES               =   get.c
print_data_SOURCES        =   print_data.c
set_SOURCES               =   set.c
set_missing_SOURCES               =   set_missing.c
set_bitmap_SOURCES        =   set_bitmap.c
multi_SOURCES             =   multi.c
keys_iterator_SOURCES     =   keys_iterator.c
iterator_SOURCES          =   iterator.c
set_pv_SOURCES          =   set_pv.c
points_SOURCES          =   points.c
list_SOURCES          =   list.c
sections_copy_SOURCES = sections_copy.c

INCLUDES =  -I$(top_builddir)/src

LDADD = $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh

