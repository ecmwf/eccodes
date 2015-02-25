dnl Process this file with autoconf to produce a configure script.

AC_DEFUN([_AM_AUTOCONF_VERSION],[])

AC_PREREQ([2.59])
AC_INIT([eccodes],[ ], [Software.Support@ecmwf.int])

AC_CONFIG_AUX_DIR([config])

LT_INIT([shared])
AC_SUBST([LIBTOOL_DEPS])
AC_CONFIG_MACRO_DIR([m4])

# Source file containing package/library versioning information.

 . ${srcdir}/version.sh

ECCODES_MAIN_VERSION="${ECCODES_MAJOR_VERSION}.${ECCODES_MINOR_VERSION}.${ECCODES_REVISION_VERSION}"
 echo $ECCODES_MAIN_VERSION

PACKAGE_VERSION="${ECCODES_MAIN_VERSION}"
ECCODES_VERSION_STR="${ECCODES_MAIN_VERSION}"
ECCODES_PATCH_VERSION="${ECCODES_REVISION_VERSION}"
AC_SUBST(ECCODES_MAIN_VERSION)
AC_SUBST(ECCODES_VERSION_STR)
AC_SUBST(ECCODES_MAJOR_VERSION)
AC_SUBST(ECCODES_MINOR_VERSION)
AC_SUBST(ECCODES_PATCH_VERSION)

AC_SUBST(ECCODES_CURRENT)
AC_SUBST(ECCODES_REVISION)
AC_SUBST(ECCODES_AGE)

echo "configuring ${PACKAGE_NAME} ${PACKAGE_VERSION}"

# Ensure that make can run correctly
AM_SANITY_CHECK

AC_CONFIG_SRCDIR([src/grib_api.h])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_FILES([src/eccodes_version.h])
AC_CONFIG_FILES([rpms/eccodes.pc rpms/eccodes.spec rpms/eccodes_f90.pc])
AM_INIT_AUTOMAKE($PACKAGE_NAME,[${PACKAGE_VERSION}],[http://www.ecmwf.int])

definition_files_path="${datadir}/eccodes/definitions"
samples_files_path="${datadir}/eccodes/samples"
ifs_samples_files_path="${datadir}/eccodes/ifs_samples"
default_perl_install="${prefix}/perl"

AC_DEFINE_UNQUOTED(ECCODES_MAIN_VERSION,$ECCODES_MAIN_VERSION,ecCodes version)
AC_DEFINE_UNQUOTED(ECCODES_MAJOR_VERSION,$ECCODES_MAJOR_VERSION,ecCodes Major release)
AC_DEFINE_UNQUOTED(ECCODES_MINOR_VERSION,$ECCODES_MINOR_VERSION,ecCodes Minor release)
AC_DEFINE_UNQUOTED(ECCODES_REVISION_VERSION,$ECCODES_REVISION_VERSION,ecCodes Revision release)

AC_DEFINE_UNQUOTED(ECCODES_CURRENT,$ECCODES_CURRENT,ecCodes Current ABI version)
AC_DEFINE_UNQUOTED(ECCODES_REVISION,$ECCODES_REVISION,ecCodes Revision ABI version)
AC_DEFINE_UNQUOTED(ECCODES_AGE,$ECCODES_AGE,ecCodes Age of ABI version)


AH_TEMPLATE([_LARGE_FILE_API],
			[Needs to be undefined on some AIX])

PERLDIR=perl
AC_SUBST(PERLDIR)


dnl Checks for programs.
AC_PROG_CC(xlc_r xlc gcc cc pgcc)
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_F77(pgf90 pgf77 xlf gfortran f77 g77 f90 ifort) 
AC_PROG_FC(pgf90 xlf90 gfortran f90 ifort) 

dnl check availability of pthreads
AC_ARG_ENABLE([pthread],
  [AS_HELP_STRING([--enable-pthread],[enable POSIX threads [by default disabled]])],
    [pthreads=${enableval}] , [pthreads=no]
)
if test "x${pthreads}" = xyes; then
   GRIB_PTHREADS=1
else
   GRIB_PTHREADS=0
fi

if test $GRIB_PTHREADS -eq 1
then
   AC_GRIB_PTHREADS
   AC_GRIB_LINUX_PTHREADS
else
   GRIB_LINUX_PTHREADS=0
fi

AC_DEFINE_UNQUOTED(GRIB_PTHREADS,$GRIB_PTHREADS,1->pthreads enabled 0->pthreads disabled)
AC_DEFINE_UNQUOTED(GRIB_LINUX_PTHREADS,$GRIB_LINUX_PTHREADS,1->pthreads enabled 0->pthreads disabled)

dnl check IBM POWER 6/7 optimisations option
AC_ARG_ENABLE([ibmpower67_opt],
  [AS_HELP_STRING([--enable-ibmpower67_opt],[enable IBM POWER 6/7 optimisations [by default disabled]])],
    [ibmpower67_opts=${enableval}] , [ibmpower67_opts=no]
)
if test "x${ibmpower67_opts}" = xyes; then
   GRIB_IBMPOWER67_OPT=1
else
   GRIB_IBMPOWER67_OPT=0
fi
AC_DEFINE_UNQUOTED(GRIB_IBMPOWER67_OPT,$GRIB_IBMPOWER67_OPT,1->IBM Power6/7 Optimisations enabled 0->IBM Power6/7 Optimisations disabled)


dnl check on uppercase fortran modules not working to be fixed
dnl some fortran compilers change the name of the .mod file in upper case!
ac_cv_prog_f90_uppercase_mod=no
AC_PROG_FC_UPPERCASE_MOD
AM_CONDITIONAL(UPPER_CASE_MOD, [test "x$ac_cv_prog_f90_uppercase_mod" = xyes])

AC_IEEE_BE
AC_DEFINE_UNQUOTED(IEEE_BE,$IS_IEEE_BE,1-> ieee big endian float/double 0->no ieee big endian float/double)

AC_IEEE_LE
AC_DEFINE_UNQUOTED(IEEE_LE,$IS_IEEE_LE,1-> ieee little endian float/double 0->no ieee little endian float/double)

dnl disable ieee native packing
AC_ARG_ENABLE([ieee-native],
  [AS_HELP_STRING([--disable-ieee-native],[disable ieee native packing])],
     without_ieee=1,without_ieee=0)

if test $without_ieee -eq 1
then
AC_DEFINE_UNQUOTED(IEEE_LE,0,1-> ieee little endian float/double 0->no ieee little endian float/double)
AC_DEFINE_UNQUOTED(IEEE_BE,0,1-> ieee big endian float/double 0->no ieee big endian float/double)
fi

AC_BIG_ENDIAN
AC_DEFINE_UNQUOTED(IS_BIG_ENDIAN,$IS_BIG_ENDIAN,1-> big endian 0->little endian)

AC_INLINE
AC_DEFINE_UNQUOTED(GRIB_INLINE,$HAS_INLINE,inline if available)

AC_ALIGN
AC_DEFINE_UNQUOTED(GRIB_MEM_ALIGN,$MEM_ALIGN,memory alignment required)

AC_CHECK_FUNC([posix_memalign],
    [AC_DEFINE_UNQUOTED(POSIX_MEMALIGN,1,posix_memalign present)])

AC_ARG_ENABLE([align-memory],
  [AS_HELP_STRING([--enable-align-memory],[enable memory alignment [by default disabled]])],
                  AC_DEFINE_UNQUOTED(GRIB_MEM_ALIGN,1,memory alignment required),
)

dnl use vectorised code
AC_ARG_ENABLE([vector],
  [AS_HELP_STRING([--enable-vector],[enable vectorised code [by default disabled]] )],
	 [vectorise=${enableval}],[vectorise=no])
if test "x${vectorise}" = xyes
then
   vectorise=1
else
   vectorise=0
fi
AC_DEFINE_UNQUOTED(VECTOR,$vectorise,vectorised code) 

dnl enable memory management
AC_ARG_ENABLE([memory-management],
  [AS_HELP_STRING([--enable-memory-management],[enable memory [by default disabled]])],
AC_DEFINE_UNQUOTED(MANAGE_MEM,1,memory management) ,
AC_DEFINE_UNQUOTED(MANAGE_MEM,0,memory management) 
	 )

dnl enable development configuration
DEVEL_RULES='' 
AC_ARG_ENABLE([development],
  [AS_HELP_STRING([--enable-development],[enable development configuration [by default disabled]])],
    [GRIB_DEVEL=${enableval}] , [GRIB_DEVEL=no]
)
if test "x${GRIB_DEVEL}" = xyes
then
   GRIB_DEVEL=1
   DEVEL_RULES='extrules.am'
else
   GRIB_DEVEL=0
   DEVEL_RULES='dummy.am'
fi
AC_SUBST(DEVEL_RULES)
AC_SUBST(GRIB_DEVEL)

AM_CONDITIONAL([WITH_MARS_TESTS], [test $GRIB_DEVEL -eq 1])

dnl Large file support
AC_FUNC_FSEEKO
CREATE_H=''
if test x"$ac_cv_func_fseeko" != xyes ; then
	CREATE_H='./create_h.sh 1'
else
	CREATE_H='./create_h.sh 0'
fi 

AC_SYS_LARGEFILE

dnl What OS are we running?
AC_CANONICAL_HOST

dnl RPM related variables
RPM_HOST_CPU=${host_cpu}
RPM_HOST_VENDOR=${host_vendor}
RPM_HOST_OS=${host_os}
RPM_CONFIGURE_ARGS=${ac_configure_args}
AC_SUBST(RPM_HOST_CPU)
AC_SUBST(RPM_HOST_VENDOR)
AC_SUBST(RPM_HOST_OS)
AC_SUBST(RPM_CONFIGURE_ARGS)
AC_ARG_WITH(rpm-release, [  --with-rpm-release=NUMBER       The rpms will use this release number  (defaults to 1)],
                RPM_RELEASE="$withval", RPM_RELEASE=1)
AC_SUBST(RPM_RELEASE)

ECCODES_SAMPLES_PATH=$samples_files_path
ECCODES_DEFINITION_PATH=$definition_files_path

AC_SUBST(ECCODES_SAMPLES_PATH)
AC_SUBST(ECCODES_DEFINITION_PATH)

dnl Fortran interface
AC_ARG_ENABLE([fortran],
  [AS_HELP_STRING([--disable-fortran],[disable fortran interface [by default enabled]])],
     [with_fortran=${enableval}], [with_fortran=yes])

if test "x${with_fortran}" = xyes; then
   without_fortran=0
else
   without_fortran=1
fi

if test "x$FC" = "x"
then
  without_fortran=1
fi

dnl check on uppercase fortran modules not working to be fixed
dnl some fortran compilers change the name of the .mod file in upper case!
ac_cv_prog_f90_uppercase_mod=no
AC_PROG_FC_UPPERCASE_MOD
AM_CONDITIONAL(UPPER_CASE_MOD, [test "x$ac_cv_prog_f90_uppercase_mod" = xyes])

dnl check if the fortran compiler has problems using modules when in debug mode
dnl Porland compilers versions 7 and 8 are known to fail here
AC_PROG_FC_DEBUG_IN_MODULE
AM_CONDITIONAL(DEBUG_IN_MOD, [test "x$ac_cv_prog_f90_debug_in_module" = xyes])

if test $without_fortran -ne 1 && test "x$ac_cv_prog_f90_debug_in_module" != xyes \
		&& test "x$enable_shared" = xyes && test "x$FCFLAGS" = "x-g"
then
	without_fortran=1
	AC_MSG_WARN([ 

  Your Fortran compiler ($FC) does not support linking to dynamic library modules when debug is enabled. This is a known 
problem with Portland compilers versions 7 and 8, for example. Possible workarounds are:

  - Use a Portland compiler version 10 or higher or a different compiler.
  - Disable shared libraries (e.g. configure --disable-shared)
  - Disable debug (e.g. configure FCFLAGS=\"\")

  Compilation of the Fortran module has been disabled

])
fi


if test $without_fortran -ne 1
then
  FORTRAN_MOD=fortran
  AC_SUBST(FORTRAN_MOD)
  F90_CHECK="examples/F90"
  AC_SUBST(F90_CHECK)

  dnl detect the Fortran 90 modules inclusion flag.
  AX_F90_MODULE_FLAG
  if test "$ax_cv_f90_modflag" = "not found" ; then
    AC_MSG_ERROR([unable to find compiler flag for modules inclusion])
  fi
  F90_MODULE_FLAG=$ax_cv_f90_modflag
  AC_SUBST([F90_MODULE_FLAG])
fi

dnl ifs_samples
AC_ARG_WITH([ifs-samples],
        [AS_HELP_STRING([--with-ifs-samples=ifs-samples-dir],[ifs_samples will be installed in ifs-samples-dir])],
        ifs_samples=$withval, ifs_samples='none')
IFS_SAMPLES_DIR=""

if test $ifs_samples != 'none'
then
	IFS_SAMPLES_DIR=$ifs_samples
else
	IFS_SAMPLES_DIR=$ifs_samples_files_path
fi
AC_SUBST([IFS_SAMPLES_DIR])


dnl EMOS
AC_ARG_WITH([emos],
        [AS_HELP_STRING([--with-emos=EMOS],[use emos for tests])],
        emos=$withval, emos='none')

EMOS_LIB=""
if test "$emos" != 'none'
then
  EMOS_LIB=$emos
  AC_DEFINE(HAVE_LIBEMOS,1,Define if you have EMOS library)
fi


dnl fortran libraries
AC_ARG_WITH([fortranlibdir],
        [AS_HELP_STRING([--with-fortranlibdir=FORTRANDIR],[fortran libraries directory ])],
        fortranlibdir=$withval, fortranlibdir='')

AC_ARG_WITH([fortranlibs],
        [AS_HELP_STRING([--with-fortranlibs=FORTRANLIBS],[fortran libraries to link from C])],
        fortranlibs=$withval, fortranlibs='none')

if test "$fortranlibs" != 'none'
then
   EMOS_LIB="$emos -L$fortranlibdir $fortranlibs -Wl,-rpath $fortranlibdir"
fi

AC_SUBST(EMOS_LIB)

dnl timer
AC_ARG_ENABLE([timer],
  [AS_HELP_STRING([--enable-timer],[enable timer [by default disabled]])],
     [with_timer=${enableval}], [with_timer=no])
if test "x${with_timer}" = xyes; then
  AC_DEFINE(GRIB_TIMER,1,1->Timer on 0->Timer off)
else
  AC_DEFINE(GRIB_TIMER,0,1->Timer on 0->Timer off)
fi


dnl multithread packing
AC_ARG_ENABLE([omp-packing],
  [AS_HELP_STRING([--enable-omp-packing],[enable OpenMP multithreaded packing [by default disabled]])],
     [with_omp=${enableval}], [with_omp=no])
if test "x${with_omp}" = xyes; then
  AC_DEFINE(OMP_PACKING,1,1->OpenMP packing 0->single thread packing)
else
  AC_DEFINE(OMP_PACKING,0,1->OpenMP packing 0->single thread packing)
fi

AC_ARG_WITH([netcdf],
[AS_HELP_STRING([--with-netcdf=NETCDF],[enable netcdf encoding/decoding using netcdf library in NETCDF])],
				netcdf_dir=$withval,netcdf_dir='none')
with_netcdf=0
if test $netcdf_dir != 'none'
then
	with_netcdf=1
	CFLAGS="$CFLAGS -I${netcdf_dir}/include"
    NETCDF_LDFLAGS="-L${netcdf_dir}/lib -lnetcdf"
    ORIG_LDFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS $NETCDF_LDFLAGS"
    AC_CHECK_LIB(netcdf,nc_open,netcdf_ok=1,netcdf_ok=0)
    LDFLAGS=$ORIG_LDFLAGS
	if test $netcdf_ok -eq 0
	then
      AC_MSG_NOTICE([
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  CONFIGURATION ERROR: netcdf test not passed.                                                               
  Please check that the path to the netcdf library given in --with-netcdf=PATH_TO_NETCDF
  is correct. Otherwise build without netcdf.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  ])
	  test 0 -eq 1
	  exit 
    fi
    AC_SUBST(NETCDF_LDFLAGS)
    AC_DEFINE(HAVE_NETCDF,1,NETCDF enabled)
fi

dnl Check for jpeg
AC_ARG_ENABLE([jpeg],
  [AS_HELP_STRING([--disable-jpeg],[disable jpeg 2000 for grib 2 decoding/encoding [by default enabled]])],
     without_jpeg=1,without_jpeg=0)

AC_ARG_WITH([jasper],
        [AS_HELP_STRING([--with-jasper=JASPER],[use specified jasper installation directory])],
        jasper_dir=$withval, jasper_dir='system')

JASPER_DIR=$jasper_dir
AC_SUBST(JASPER_DIR)

if test $jasper_dir != 'system'
then
  CFLAGS="$CFLAGS -I${jasper_dir}/include"
  LDFLAGS="$LDFLAGS -L${jasper_dir}/lib"
fi

AC_ARG_WITH([openjpeg],
        [AS_HELP_STRING([--with-openjpeg=OPENJPEG],[use specified openjpeg installation directory])],
        openjpeg_dir=$withval, openjpeg_dir='system')

OPENJPEG_DIR=$openjpeg_dir
AC_SUBST(OPENJPEG_DIR)

if test $openjpeg_dir != 'system'
then
  CFLAGS="$CFLAGS -I${openjpeg_dir}/include"
  LDFLAGS="$LDFLAGS -L${openjpeg_dir}/lib"
fi

if test $without_jpeg -ne 1
then
    AC_DEFINE(HAVE_JPEG,1,JPEG enabled)
    AC_CHECK_LIB(jasper,jas_stream_memopen,jasper_ok=1,jasper_ok=0)
    AC_CHECK_LIB(openjpeg,opj_image_create,openjpeg_ok=1,openjpeg_ok=0)

    jpeg_ok=0

    # prefer openjpeg over jasper
	if test $openjpeg_ok -eq 1
	then
	  jpeg_ok=1
      LIB_OPENJPEG='-lopenjpeg -lm'
      LIBS="$LIB_OPENJPEG $LIBS"
      AC_DEFINE(HAVE_LIBOPENJPEG,1,Define if you have JPEG version 2 "Openjpeg" library)
      AC_SUBST(LIB_OPENJPEG)
    elif test $jasper_ok -eq 1
    then
	  jpeg_ok=1
      LIB_JASPER='-ljasper'
      LIBS="$LIB_JASPER $LIBS"
      AC_DEFINE(HAVE_LIBJASPER,1,Define if you have JPEG version 2 "Jasper" library)
      AC_SUBST(LIB_JASPER)
	fi

	if test $jpeg_ok -eq 0
	then
      AC_MSG_NOTICE([
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  CONFIGURATION ERROR: jpeg library (jasper or openjpeg) required.                                                               
  jpeg library installation is not working or missing.                                 

  To fix this problem you have the following options.                                  
  1) Install without jpeg support enabled (--disable-jpeg), but you will not be able to
     decode grib2 data encoded in jpeg.
  2) Check if you have a jpeg library installed in a path different from your system path. 
     In this case you can provide your jpeg library installation path to the configure
     through the options:
        --with-jasper="jasper_lib_path"
        --with-openjpeg="openjpeg_lib_path"
  3) Download and install one of the supported jpeg libraries.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  ])
	  [ 0 -eq 1 ]
	  exit 
    fi
  JPEG_TEST="jpeg.sh"
  AC_SUBST(JPEG_TEST)
fi

dnl check for libaec
CCSDS_TEST=""
AC_ARG_WITH([aec],
            [AS_HELP_STRING([--with-aec=DIR],
                            [use specified libaec installation directory])],
            ,[with_aec=no])

if test "x$with_aec" != xno ; then
  if test "x$with_aec" != xyes ; then
    LDFLAGS="$LDFLAGS -L$with_aec/lib"
    CPPFLAGS="$CPPFLAGS -I$with_aec/include"
  fi
  AC_CHECK_LIB(aec, aec_encode, , [AC_MSG_FAILURE(
                 [aec test failed (--without-aec to disable)])])
  CCSDS_TEST="ccsds.sh"
  LIB_AEC='-laec'
  AC_SUBST(LIB_AEC)
  AEC_DIR="$with_aec"
  AC_SUBST(AEC_DIR)

fi
AC_SUBST(CCSDS_TEST)

dnl Check for png
AC_ARG_WITH([png-support],
  [AS_HELP_STRING([--with-png-support],[add support for png decoding/encoding])],
     with_png=1,with_png=0)

if test $with_png -gt 0
then
    AC_MSG_CHECKING(for PNG )
    AC_MSG_RESULT()
    AC_CHECK_HEADER(png.h,passed=1,passed=0)
     AC_CHECK_LIB(png,png_read_png,passed=1,passed=0)
    AC_MSG_CHECKING(if PNG support package is complete)

    if test $passed -gt 0
    then
      LIB_PNG='-lpng'
      LIBS="$LIB_PNG $LIBS"
      AC_DEFINE(HAVE_LIBPNG,1,Define to 1 if you have the png library (-lpng))
      AC_SUBST(LIB_PNG)
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no -- some components failed test)
    fi
fi

dnl Perl installation directory

#PERL_INSTALL_OPTIONS="PREFIX=$prefix INSTALLDIRS=perl"
PERL_INSTALL_OPTIONS="LIB=$default_perl_install"

AC_ARG_ENABLE([install-system-perl],
        [AS_HELP_STRING([--enable-install-system-perl],[perl modules will install in the standard perl installation ])],
        enable_perl_install='yes', enable_perl_install='no')

if test "$enable_perl_install" = 'yes'
then
    PERL_INSTALL_OPTIONS=""
fi

AC_SUBST(PERL_INSTALL_OPTIONS)

dnl Disable build/install of Perl grib_api.
AC_ARG_WITH([perl],
        [AS_HELP_STRING([--with-perl=PERL],[use specified Perl binary to configure Perl grib_api])],
        with_perl=$withval, with_perl='no')

dnl Look for PERL if Perl requested
if test "$with_perl" != 'no'
then
  if test "$with_perl" != 'yes'
  then
    AC_CACHE_CHECK(for perl,ac_cv_path_PERL,ac_cv_path_PERL="$with_perl");
    PERL=$ac_cv_path_PERL
    AC_SUBST(PERL)dnl
  else
    AC_PATH_PROGS(PERL,perl perl5,perl)dnl
  fi
fi

dnl Get full paths
builddir=`pwd`

dnl Options to pass when configuring Perl grib_api
GRIB_API_LIB="${builddir}/src/grib_api.a"
GRIB_API_INC="${builddir}/src"
AC_ARG_WITH([perl-options],
  [AS_HELP_STRING([--with-perl-options=[OPTIONS]], [options to pass on command-line when
                          generating Perl grib_api's Makefile from Makefile.PL])],
  PERL_MAKE_OPTIONS=$withval)
AC_SUBST(PERL_MAKE_OPTIONS)
AC_SUBST(GRIB_API_LIB)
AC_SUBST(GRIB_API_INC)

AM_CONDITIONAL(WITH_PERL, test $with_perl != no)

dnl Enable the Python interface in the build
AC_ARG_ENABLE([python],
  [AS_HELP_STRING([--enable-python],[Enable the Python interface in the build [by default disabled]])])

dnl Check if the user wants numpy disabled
AC_ARG_ENABLE([numpy],
  [AS_HELP_STRING([--disable-numpy],[Disable NumPy as the data handling package for the Python interface [by default enabled]])])

dnl Look for Python if requested
if test "x$enable_python" = "xyes"
then
  dnl search for a python interpreter on the system dnl abort if one not found
  dnl am_path_python sets many python vars - RTFM for more info
  AM_PATH_PYTHON([2.5])

  AC_ARG_VAR([PYTHON_INCLUDES], [Include flags for python])
  AC_ARG_VAR([PYTHON_LDFLAGS], [Link flags for python])
  AC_ARG_VAR([PYTHON_CFLAGS], [C flags for python])
  AC_ARG_VAR([PYTHON_LIBS], [Libraries for python])
  AC_ARG_VAR([PYTHON_CONFIG], [Path to python-config])

  AC_PATH_PROGS([PYTHON_CONFIG],
                [python$PYTHON_VERSION-config python-config],
                [no],
                [`dirname $PYTHON`])
  AS_IF([test "$PYTHON_CONFIG" = no], [AC_MSG_ERROR([cannot find python-config for $PYTHON.])])

  AC_MSG_CHECKING([python include flags])
  PYTHON_INCLUDES=`$PYTHON_CONFIG --includes`
  AC_MSG_RESULT([$PYTHON_INCLUDES])

  AC_MSG_CHECKING([python link flags])
  PYTHON_LDFLAGS=`$PYTHON_CONFIG --ldflags`
  AC_MSG_RESULT([$PYTHON_LDFLAGS])

  AC_MSG_CHECKING([python C flags])
  PYTHON_CFLAGS=`$PYTHON_CONFIG --cflags`
  AC_MSG_RESULT([$PYTHON_CFLAGS])

  AC_MSG_CHECKING([python libraries])
  PYTHON_LIBS=`$PYTHON_CONFIG --libs`
  AC_MSG_RESULT([$PYTHON_LIBS])

  # macro that gets the include path for Python.h which is used to build
  # the shared library corresponding to the GRIB API Python module.
  # AX_PYTHON_DEVEL

  # enable testing scripts if building with Python
  PYTHON_CHECK='examples/python'
  AC_SUBST(PYTHON_CHECK)

  data_handler=numpy
  if test "x$enable_numpy" != "xno"
  then
    AC_MSG_CHECKING(whether numpy is installed)

    has_numpy=`$PYTHON -c "import numpy;print numpy" 2> /dev/null`
    if test "x$has_numpy" = "x"
    then
       AC_MSG_RESULT(no)
       AC_MSG_ERROR([NumPy is not installed. Use --disable-numpy if you want to disable Numpy from the build.])
    else
       AC_MSG_RESULT(yes)
       NUMPY_INCLUDE=`$PYTHON -c "import numpy;print numpy.get_include()"`
       AC_SUBST(NUMPY_INCLUDE)
    fi
  else
    data_handler=array
  fi

  PYTHON_DATA_HANDLER=$data_handler
  AC_SUBST(PYTHON_DATA_HANDLER)
fi

AM_CONDITIONAL([WITH_PYTHON], [test x$PYTHON != x])
AM_CONDITIONAL([WITH_FORTRAN], [test x$FORTRAN_MOD != x])
AM_CONDITIONAL([CREATING_SHARED_LIBS], [test "x$enable_shared" = xyes])


dnl AC_DISABLE_SHARED
dnl LT_INIT

dnl Checks for ar and rm
AC_CHECK_PROG(RM, rm, rm)
AC_CHECK_TOOL(AR, ar, ar)

dnl Check if -pedantic available
grib_api_PROG_CC_WARNING_PEDANTIC([-Wall])

dnl Enable -Werror despite compiler version
grib_api_ENABLE_WARNINGS_ARE_ERRORS

dnl Checks for libraries
AC_CHECK_LIB(m,pow)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h sys/param.h sys/time.h unistd.h math.h stdarg.h assert.h ctype.h fcntl.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero gettimeofday])

AX_LINUX_DISTRIBUTION

AC_OUTPUT( Makefile src/Makefile fortran/Makefile tools/Makefile data/Makefile definitions/Makefile samples/Makefile ifs_samples/grib1/Makefile ifs_samples/grib1_mlgrib2/Makefile ifs_samples/grib1_mlgrib2_ieee64/Makefile tests/Makefile examples/C/Makefile examples/F90/Makefile tigge/Makefile perl/GRIB-API/Makefile.PL perl/Makefile python/Makefile examples/python/Makefile)

AC_MSG_NOTICE([

  Configuration completed.

    You can now say 'make' to compile the eccodes package, 'make check' to
	  test it  and 'make install' to install it afterwards.
	  ])
