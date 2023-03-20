if WITH_PYTHON

if CREATING_SHARED_LIBS
###############################################################################
# Case when shared libraries are enabled
###############################################################################
pkgpyexec_LTLIBRARIES = _gribapi_swig.la

_gribapi_swig_la_SOURCES = gribapi_swig_wrap.c grib_interface.c grib_interface.h
_gribapi_swig_la_LIBADD = $(top_builddir)/src/libeccodes.la $(PYTHON_LIBS)
_gribapi_swig_la_LDFLAGS = -module -avoid-version $(PYTHON_LDFLAGS)

INCLUDES = $(PYTHON_INCLUDES) -I$(NUMPY_INCLUDE)

gribapi_swig_wrap.c: swig_wrap_$(PYTHON_DATA_HANDLER).c
	ln -s gribapi/gribapi.py
	ln -s eccodes/eccodes.py
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).c gribapi_swig_wrap.c

gribapi_swig.py: swig_wrap_$(PYTHON_DATA_HANDLER).py
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).py gribapi_swig.py

# Extra stuff to go into the distribution
EXTRA_DIST = grib_interface.h \
			grib_interface.c \
			gribapi_swig.i \
			gribapi.py \
			eccodes.py \
			numpy.i \
			grib_errors.h \
			extrules.am \
			dummy.am \
			swig_wrap_array.c \
			swig_wrap_numpy.c \
			swig_wrap_array.py \
			swig_wrap_numpy.py \
			CMakeLists.txt

# What else I want installed
pkgpyexec_DATA = \
	gribapi.py \
	eccodes.py \
	gribapi_swig.py

CLEANFILES = \
			gribapi_swig_wrap.c \
			gribapi_swig.py \
			_gribapi_swig.la

include $(DEVEL_RULES)

else
###############################################################################
# When shared libraries are disabled.
# Uses the static library so must have --with-pic
###############################################################################
noinst_SCRIPTS = setup.py

CLEANFILES = $(noinst_SCRIPTS) \
			gribapi_swig_wrap.c \
			gribapi_swig.py \
			_gribapi_swig.so

# Extra stuff to go into the distribution
EXTRA_DIST = setup.py.autotools.in \
			grib_interface.h \
			grib_interface.c \
			gribapi_swig.i \
			gribapi.py \
			eccodes.py \
			numpy.i \
			grib_errors.h \
			extrules.am \
			dummy.am \
			swig_wrap_array.c \
			swig_wrap_numpy.c \
			swig_wrap_array.py \
			swig_wrap_numpy.py \
			CMakeLists.txt

# What I want installed
pdir = $(libdir)/python$(PYTHON_VERSION)/site-packages/eccodes
p_DATA = \
	_gribapi_swig.so \
	gribapi.py \
	eccodes.py \
	gribapi_swig.py

do_subst = sed -e 's,[@]LIB_JASPER[@],$(LIB_JASPER),g' \
			-e 's,[@]JASPER_DIR[@],$(JASPER_DIR),g' \
			-e 's,[@]LIB_OPENJPEG[@],$(LIB_OPENJPEG),g' \
			-e 's,[@]OPENJPEG_DIR[@],$(OPENJPEG_DIR),g' \
			-e   's,[@]LIB_AEC[@],$(LIB_AEC),g' \
			-e   's,[@]AEC_DIR[@],$(AEC_DIR),g' \
			-e 's,[@]LIB_PNG[@],$(LIB_PNG),g' \
			-e 's,[@]BUILD_DIR[@],$(top_builddir),g' \
			-e 's,[@]DATA_HANDLER[@],$(PYTHON_DATA_HANDLER),g'

clean-local:
	-rm -rf build

setup.py: setup.py.autotools.in
	$(do_subst) < setup.py.autotools.in > setup.py

gribapi_swig_wrap.c: swig_wrap_$(PYTHON_DATA_HANDLER).c
	ln -s gribapi/gribapi.py
	ln -s eccodes/eccodes.py
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).c gribapi_swig_wrap.c

gribapi_swig.py: swig_wrap_$(PYTHON_DATA_HANDLER).py
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).py gribapi_swig.py

_gribapi_swig.so: setup.py gribapi_swig_wrap.c gribapi_swig.py
	$(PYTHON) setup.py build_ext --inplace

include $(DEVEL_RULES)

endif

else
# Case when WITH_PYTHON is not defined. Must add this rule otherwise
# make dist will fail
gribapi_swig_wrap.c: swig_wrap_array.c
	cp -f swig_wrap_array.c gribapi_swig_wrap.c

endif

