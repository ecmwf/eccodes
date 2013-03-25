if WITH_PYTHON

if CREATING_SHARED_LIBS
###############################################################################
# Case when shared libraries are enabled
###############################################################################
pkgpyexec_LTLIBRARIES = _eccode_swig.la

_eccode_swig_la_SOURCES = eccode_swig_wrap.c grib_interface.c grib_interface.h
_eccode_swig_la_LIBADD = $(top_builddir)/src/libgrib_api.la $(PYTHON_LIBS)
_eccode_swig_la_LDFLAGS = -module -avoid-version $(PYTHON_LDFLAGS)

INCLUDES = $(PYTHON_INCLUDES) -I$(NUMPY_INCLUDE)

eccode_swig_wrap.c: swig_wrap_$(PYTHON_DATA_HANDLER).c
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).c eccode_swig_wrap.c

eccode_swig.py: swig_wrap_$(PYTHON_DATA_HANDLER).py
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).py eccode_swig.py

# Extra stuff to go into the distribution
EXTRA_DIST = grib_interface.h \
			grib_interface.c \
			eccode_swig.i \
			eccode.py \
			numpy.i \
			grib_errors.h \
			extrules.am \
			dummy.am \
			swig_wrap_array.c \
			swig_wrap_numpy.c \
			swig_wrap_array.py \
			swig_wrap_numpy.py

# What else I want installed
pkgpyexec_DATA = \
	eccode.py \
	eccode_swig.py

CLEANFILES = \
			eccode_swig_wrap.c \
			eccode_swig.py \
			_eccode_swig.la

include $(DEVEL_RULES)

else
###############################################################################
# When shared libraries are disabled.
# Uses the static library so must have --with-pic
###############################################################################
noinst_SCRIPTS = setup.py

CLEANFILES = $(noinst_SCRIPTS) \
			eccode_swig_wrap.c \
			eccode_swig.py \
			_eccode_swig.so

# Extra stuff to go into the distribution
EXTRA_DIST = setup.py.in \
			grib_interface.h \
			grib_interface.c \
			eccode_swig.i \
			eccode.py \
			numpy.i \
			grib_errors.h \
			extrules.am \
			dummy.am \
			swig_wrap_array.c \
			swig_wrap_numpy.c \
			swig_wrap_array.py \
			swig_wrap_numpy.py

# What I want installed
pdir = $(libdir)/python$(PYTHON_VERSION)/site-packages/eccode
p_DATA = \
	_eccode_swig.so \
	eccode.py \
	eccode_swig.py

do_subst = sed -e 's,[@]LIB_JASPER[@],$(LIB_JASPER),g' \
			-e 's,[@]JASPER_DIR[@],$(JASPER_DIR),g' \
			-e 's,[@]LIB_OPENJPEG[@],$(LIB_OPENJPEG),g' \
			-e 's,[@]OPENJPEG_DIR[@],$(OPENJPEG_DIR),g' \
			-e 's,[@]LIB_PNG[@],$(LIB_PNG),g' \
			-e 's,[@]BUILD_DIR[@],$(top_builddir),g' \
			-e 's,[@]DATA_HANDLER[@],$(PYTHON_DATA_HANDLER),g'

clean-local:
	-rm -rf build

setup.py: setup.py.in
	$(do_subst) < setup.py.in > setup.py

eccode_swig_wrap.c: swig_wrap_$(PYTHON_DATA_HANDLER).c
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).c eccode_swig_wrap.c

eccode_swig.py: swig_wrap_$(PYTHON_DATA_HANDLER).py
	cp -f swig_wrap_$(PYTHON_DATA_HANDLER).py eccode_swig.py

_eccode_swig.so: setup.py eccode_swig_wrap.c eccode_swig.py
	$(PYTHON) setup.py build_ext --inplace

include $(DEVEL_RULES)

endif

endif

