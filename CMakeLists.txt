#
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
###############################################################################
# cmake options:
#
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_MODULE_PATH=/path/to/ecbuild/cmake

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.7 HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild)
if(NOT ecbuild_FOUND)
    message(STATUS "Fetching ecbuild...")
    include(FetchContent)
    FetchContent_Populate(ecbuild
        GIT_REPOSITORY https://github.com/ecmwf/ecbuild.git
        GIT_TAG        3.8.5
    )
    find_package( ecbuild 3.7 REQUIRED HINTS ${ecbuild_SOURCE_DIR})
endif()

# Initialise project
project( eccodes LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


###############################################################################
# system checks needed for eccodes_config.h and some options like MEMFS

check_type_size( int     ECCODES_SIZEOF_INT )
check_type_size( long    ECCODES_SIZEOF_LONG )
check_type_size( size_t  ECCODES_SIZEOF_SIZE_T )

check_include_files( assert.h      ECCODES_HAVE_ASSERT_H )
check_include_files( string.h      ECCODES_HAVE_STRING_H )
check_include_files( sys/types.h   ECCODES_HAVE_SYS_TYPES_H )
check_include_files( sys/stat.h    ECCODES_HAVE_SYS_STAT_H )
check_include_files( fcntl.h       ECCODES_HAVE_FCNTL_H )
check_include_files( unistd.h      ECCODES_HAVE_UNISTD_H )

check_symbol_exists( fseeko          "stdio.h"    ECCODES_HAVE_FSEEKO )
check_symbol_exists( posix_memalign  "stdlib.h"   ECCODES_HAVE_POSIX_MEMALIGN )
check_symbol_exists( fmemopen        "stdio.h"    ECCODES_HAVE_FMEMOPEN )
check_symbol_exists( funopen         "stdio.h"    ECCODES_HAVE_FUNOPEN )
check_symbol_exists( realpath        "stdlib.h"   ECCODES_HAVE_REALPATH )
check_symbol_exists( fsync           "unistd.h"   ECCODES_HAVE_FSYNC)
check_symbol_exists( fdatasync       "unistd.h"   ECCODES_HAVE_FDATASYNC)

check_c_source_compiles(
      " typedef int foo_t;
      static inline foo_t static_foo(void){return 0;}
      foo_t foo(void){return 0;}
      int main(int argc, char *argv[]){ return static_foo(); }
      " ECCODES_HAVE_C_INLINE
)

include(eccodes_test_endiness)
if( EC_OS_NAME MATCHES "windows" )
    include(eccodes_find_linux_utils)
endif()

ecbuild_debug("ECCODES_BIG_ENDIAN=${ECCODES_BIG_ENDIAN}")
ecbuild_debug("ECCODES_LITTLE_ENDIAN=${ECCODES_LITTLE_ENDIAN}")
ecbuild_info("Operating system=${CMAKE_SYSTEM} (${EC_OS_BITS} bits)")

# Only support 64 bit operating systems
if( NOT EC_OS_BITS EQUAL "64" )
    ecbuild_critical( "Operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) -- ecCodes only supports 64 bit platforms" )
endif()

###############################################################################
# some variables/options of this project

# ecbuild_add_cxx_flags("-Wno-write-strings" NO_FAIL)
# ecbuild_add_cxx_flags("-Wno-writable-strings" NO_FAIL)
# ecbuild_add_cxx_flags("-Wno-deprecated" NO_FAIL)

if( CMAKE_CXX_COMPILER_ID STREQUAL "Cray" )
    set(CMAKE_CXX_FLAGS "-hstd=c++11 ${CMAKE_CXX_FLAGS}")
endif()

ecbuild_add_option( FEATURE PRODUCT_GRIB
    DESCRIPTION "Support for the product GRIB"
    DEFAULT ON )
ecbuild_add_option( FEATURE PRODUCT_BUFR
    DESCRIPTION "Support for the product BUFR"
    DEFAULT ON )
if( NOT HAVE_PRODUCT_GRIB AND NOT HAVE_PRODUCT_BUFR )
    ecbuild_critical("Cannot disable both GRIB and BUFR! Please specify just one option")
endif()

ecbuild_add_option( FEATURE EXAMPLES
    DESCRIPTION "Build the examples"
    DEFAULT ON )

ecbuild_add_option( FEATURE BUILD_TOOLS
    DESCRIPTION "Build the command line tools"
    DEFAULT ON )

ecbuild_add_option( FEATURE GEOGRAPHY
    DESCRIPTION "Support for Geoiterator and nearest neighbour"
    DEFAULT ON )

ecbuild_add_option( FEATURE ECKIT_GEO
    DESCRIPTION "Support for Geoiterator and nearest neighbour (additional backend)"
    CONDITION ENABLE_GEOGRAPHY
    DEFAULT OFF )

if( eccodes_HAVE_ECKIT_GEO AND NOT TARGET eckit_geo )
    ecbuild_find_package(NAME eckit VERSION 1.27 REQUIRED)
    if( NOT TARGET eckit_geo )
        ecbuild_critical("eckit has not been built with ECKIT_GEO enabled")
    endif()
endif()

ecbuild_add_option( FEATURE JPG
    DESCRIPTION "Support for JPG decoding/encoding"
    DEFAULT ON )
# Options related to JPG. The JasPer and OpenJPEG libraries
ecbuild_add_option( FEATURE JPG_LIBJASPER
    DESCRIPTION "Support for JPG decoding/encoding with the JasPer library"
    CONDITION ENABLE_JPG
    DEFAULT ON )
ecbuild_add_option( FEATURE JPG_LIBOPENJPEG
    DESCRIPTION "Support for JPG decoding/encoding with the OpenJPEG library"
    CONDITION ENABLE_JPG
    DEFAULT ON )

ecbuild_add_option( FEATURE PNG
    DESCRIPTION "Support for PNG decoding/encoding"
    DEFAULT OFF
    REQUIRED_PACKAGES PNG )

if( HAVE_PNG )
    set( HAVE_LIBPNG 1 ) # compatibility with autotools
    add_definitions( ${PNG_DEFINITIONS} )
else()
    set( HAVE_LIBPNG 0 )
endif()

ecbuild_add_option( FEATURE NETCDF
    DESCRIPTION "Support for GRIB to NetCDF conversion"
    DEFAULT ON
    REQUIRED_PACKAGES NetCDF
    NO_TPL )

find_package( AEC )
if(NOT DEFINED ENABLE_AEC AND NOT AEC_FOUND)
  ecbuild_critical("AEC library was not found.\n"
                   "AEC (Adaptive Entropy Coding) provides the WMO GRIB CCSDS compression and decompression of data. "
                   "This is highly recommended from ecCodes >= 2.25.0"
                   "\nTo force the build without it, use -DENABLE_AEC=OFF")
endif()
ecbuild_add_option( FEATURE AEC
    DESCRIPTION "Support for Adaptive Entropy Coding (WMO GRIB CCSDS)"
    DEFAULT ON
    CONDITION AEC_FOUND )

ecbuild_find_python( VERSION 3.6 NO_LIBS )
# find_package( NumPy )

## TODO REQUIRED_LANGUAGES Fortran
ecbuild_add_option( FEATURE FORTRAN
    DESCRIPTION "Build the ecCodes Fortran interface"
    DEFAULT ON )

# TODO Remove this after REQUIRED_LANGUAGES
if( ENABLE_FORTRAN )
    # will set EC_HAVE_FORTRAN with the result
    set( EC_HAVE_FORTRAN 0 )
    ecbuild_enable_fortran( MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/fortran/modules )
    set( HAVE_FORTRAN ${EC_HAVE_FORTRAN} )
else()
    set( HAVE_FORTRAN 0 )
endif()

# memfs requires only Python executable and not its libraries
ecbuild_add_option( FEATURE MEMFS
    DESCRIPTION "Memory based access to definitions/samples"
    DEFAULT OFF
    REQUIRED_PACKAGES Python3 )

#if( HAVE_MEMFS AND "${CMAKE_C_COMPILER_ID}" STREQUAL "Cray")
#  set( HAVE_MEMFS OFF )
#  ecbuild_warn("MEMFS not supported with Cray C compiler")
#endif()

if(HAVE_MEMFS)
  set( _will_install_defs_samples OFF )
else()
  set( _will_install_defs_samples ON )
endif()


# controls installation of files in definitions/ -- note that it still creates the symlinks in the build dir
# by default, if memfs is available, then we don't need to install definitions
ecbuild_add_option( FEATURE INSTALL_ECCODES_DEFINITIONS
    DESCRIPTION "Install the ecCodes definitions"
    DEFAULT ${_will_install_defs_samples} )

# controls installation of files in samples/ and ifs_samples/ -- note that it still creates the symlinks in the build dir
# by default, if memfs is available, then we don't need to install samples
ecbuild_add_option( FEATURE INSTALL_ECCODES_SAMPLES
    DESCRIPTION "Install the ecCodes samples, including IFS samples"
    DEFAULT ${_will_install_defs_samples} )

# advanced options (not visible in cmake-gui )

ecbuild_add_option( FEATURE MEMORY_MANAGEMENT   DESCRIPTION "Enable memory management" DEFAULT OFF ADVANCED )
ecbuild_add_option( FEATURE ALIGN_MEMORY        DESCRIPTION "Enable memory alignment"  DEFAULT OFF ADVANCED )
ecbuild_add_option( FEATURE ECCODES_THREADS     DESCRIPTION "Enable thread-safety using POSIX threads" DEFAULT OFF ADVANCED )
ecbuild_add_option( FEATURE ECCODES_OMP_THREADS DESCRIPTION "Enable thread-safety using OpenMP threads" DEFAULT OFF ADVANCED )
ecbuild_add_option( FEATURE EXTRA_TESTS         DESCRIPTION "Enable extended regression testing" DEFAULT OFF ADVANCED )

###############################################################################
# macro processing

set( ECCODES_EXTRA_LIBRARIES    "" )
set( ECCODES_EXTRA_INCLUDE_DIRS "" )
set( ECCODES_EXTRA_DEFINITIONS  "" )

find_package( CMath )
list( APPEND ECCODES_TPLS CMath )

### JPG support

set( HAVE_JPEG 0 )
set( HAVE_LIBJASPER 0 )
set( HAVE_LIBOPENJPEG 0 )

if( ENABLE_JPG )
    # Note: The function ecbuild_add_extra_search_paths is deprecated but we need it to find JasPer at ECMWF.
    #       It modifies CMAKE_PREFIX_PATH
    #       which can affect future package discovery if not undone by the caller.
    #       The current CMAKE_PREFIX_PATH is backed up as _CMAKE_PREFIX_PATH
    #
    #set(CMAKE_WARN_DEPRECATED OFF) # Suppress deprecation message
    #ecbuild_add_extra_search_paths( jasper )
    find_package( Jasper )
    #set(CMAKE_PREFIX_PATH ${_CMAKE_PREFIX_PATH})    # Restore CMAKE_PREFIX_PATH
    #set(CMAKE_WARN_DEPRECATED ON)  # Remove suppression

    find_package( OpenJPEG )

    if( JASPER_FOUND AND ENABLE_JPG_LIBJASPER )
        list( APPEND ECCODES_TPLS Jasper )
        set( HAVE_JPEG 1 )
        set( HAVE_LIBJASPER 1 )
        # Extract JasPer's major version number to enable conditional code. See ECC-396
        string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" JASPER_VERSION_MAJOR "${JASPER_VERSION_STRING}")
    endif()

    if( OPENJPEG_FOUND AND ENABLE_JPG_LIBOPENJPEG )
        list( APPEND ECCODES_TPLS OpenJPEG )
        set( HAVE_JPEG 1 )
        set( HAVE_LIBOPENJPEG 1 )
    endif()

    ecbuild_info(" HAVE_JPEG=${HAVE_JPEG}")
    ecbuild_info(" HAVE_LIBJASPER=${HAVE_LIBJASPER}")
    ecbuild_info(" HAVE_LIBOPENJPEG=${HAVE_LIBOPENJPEG}")

endif()


###############################################################################
# other options

set( IS_BIG_ENDIAN 0 )
if( ECCODES_BIG_ENDIAN )
    set( IS_BIG_ENDIAN 1 )
endif()

set( MANAGE_MEM 0 )
if( ENABLE_MEMORY_MANAGEMENT )
    set( MANAGE_MEM 1 )
endif()

set( CMAKE_THREAD_PREFER_PTHREAD 1 ) # find thread library, but prefer pthreads
find_package(Threads REQUIRED)

# debug
ecbuild_info(" CMAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT}")
ecbuild_info(" CMAKE_USE_PTHREADS_INIT=${CMAKE_USE_PTHREADS_INIT}")
ecbuild_info(" HAVE_ECCODES_THREADS=${HAVE_ECCODES_THREADS}")


set( GRIB_PTHREADS 0 )
set( GRIB_OMP_THREADS 0 )
set( GRIB_LINUX_PTHREADS 0 )
#if( HAVE_ECCODES_THREADS AND CMAKE_THREAD_LIBS_INIT )
if( HAVE_ECCODES_THREADS )
    if( NOT ${CMAKE_USE_PTHREADS_INIT} )
        ecbuild_critical("Pthreads is not supported on your system: thread library found=[${CMAKE_THREAD_LIBS_INIT}]")
    endif()
    set( GRIB_PTHREADS 1 )
    set( ECCODES_PTHREADS_LIBRARIES Threads::Threads ) # ECC-1268
    if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
        set( GRIB_LINUX_PTHREADS 1 )
    endif()
elseif(HAVE_ECCODES_OMP_THREADS)
    ecbuild_enable_omp()
    set( GRIB_OMP_THREADS 1 )
endif()

# Cannot have both options
if( HAVE_ECCODES_THREADS AND HAVE_ECCODES_OMP_THREADS )
    ecbuild_critical("Cannot enable both POSIX threads and OpenMP! Please specify just one option")
endif()

ecbuild_info(" POSIX THREADS = ${GRIB_PTHREADS}")
ecbuild_info(" OpenMP THREADS= ${GRIB_OMP_THREADS}")

set( GRIB_MEM_ALIGN 0 )
if( ENABLE_ALIGN_MEMORY )
  set( GRIB_MEM_ALIGN 1 )
endif()

# fix for #if IEEE_LE or IEE_BE instead of #ifdef

if( IEEE_BE )
    set( IEEE_LE 0 )
endif()

if( IEEE_LE )
    set( IEEE_BE 0 )
endif()

set( ECCODES_ON_WINDOWS 0 )
if( EC_OS_NAME MATCHES "windows" )
    # Symbols need to be explicitly exported on Windows so we can link to dlls.
    set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE )
    set( ECCODES_ON_WINDOWS 1 )

    # Suppress compliler warnings - see ECC-850
    # Suppress warnings about using 'insecure' functions. Fixing this would require changes all over
    # the codebase which would damage portability.
    ecbuild_add_c_flags("/D_CRT_SECURE_NO_WARNINGS")
    # Suppress warnings about using well-known C functions.
    ecbuild_add_c_flags("/D_CRT_NONSTDC_NO_DEPRECATE")
    # Suppress C4267: warns about possible loss of data when converting 'size_t' to 'int'.
    ecbuild_add_c_flags("/wd4267")
endif()

###############################################################################
# contents
set( the_default_data_prefix ${CMAKE_INSTALL_PREFIX} )
if( ${DEVELOPER_MODE} )
    ecbuild_info("DEVELOPER_MODE is defined")
    set( the_default_data_prefix ${CMAKE_BINARY_DIR} )
endif()

if( NOT DEFINED ECCODES_DEFINITION_SUFF )
  set( ECCODES_DEFINITION_SUFF  ${INSTALL_DATA_DIR}/definitions )
endif()
if( NOT DEFINED ECCODES_SAMPLES_SUFF )
  set( ECCODES_SAMPLES_SUFF     ${INSTALL_DATA_DIR}/samples )
endif()
if( NOT DEFINED ECCODES_IFS_SAMPLES_SUFF )
  set( ECCODES_IFS_SAMPLES_SUFF ${INSTALL_DATA_DIR}/ifs_samples )
endif()

set( ECCODES_DEFINITION_PATH  ${the_default_data_prefix}/${ECCODES_DEFINITION_SUFF} )
set( ECCODES_SAMPLES_PATH     ${the_default_data_prefix}/${ECCODES_SAMPLES_SUFF} )
set( ECCODES_IFS_SAMPLES_PATH ${the_default_data_prefix}/${ECCODES_IFS_SAMPLES_SUFF} )

###############################################################################
### config header

ecbuild_generate_config_headers()

configure_file( eccodes_config.h.in eccodes_config.h )

add_definitions( -DHAVE_ECCODES_CONFIG_H )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    ecbuild_add_c_flags("-pedantic")
    ecbuild_add_cxx_flags("-pedantic")
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES IntelLLVM )
  # Turn off -ffinite-math-only which gets included by some optimisation levels which assume values can never be NaN.
  # This results in std::isnan(value) always returning false.
  ecbuild_add_cxx_flags("-fno-finite-math-only")
endif()


# gfortran 10 has become stricter with argument matching
if( HAVE_FORTRAN AND CMAKE_Fortran_COMPILER_ID MATCHES "GNU" AND NOT CMAKE_Fortran_COMPILER_VERSION VERSION_LESS 10 )
    ecbuild_add_fortran_flags("-fallow-argument-mismatch")
endif()

if(GIT_FOUND AND NOT ${GIT_EXECUTABLE} STREQUAL "")
    ecbuild_info("Found git: ${GIT_EXECUTABLE} (found version \"${GIT_VERSION_STRING}\")")
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                    OUTPUT_VARIABLE eccodes_GIT_BRANCH
                    RESULT_VARIABLE nok ERROR_VARIABLE error
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" )
    ecbuild_info("ecCodes branch = ${eccodes_GIT_BRANCH}" )
endif()

###############################################################################
# contents

### export package to other ecbuild packages

set( ECCODES_INCLUDE_DIRS
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eccodes
  ${CMAKE_CURRENT_BINARY_DIR}/src/eccodes
)
set( ECCODES_LIBRARIES       eccodes )

get_directory_property( COMPILE_DEFINITIONS ECCODES_DEFINITIONS )

foreach( _tpl ${ECCODES_TPLS} )
    string( TOUPPER ${_tpl} TPL )
    if( ${TPL}_FOUND )
        if( REPLACE_TPL_ABSOLUTE_PATHS )
            # replace TPL absolute paths with their library names
            # this helps make ecCodes relocatable
            set( _TMP "" )

            foreach( _lib ${${TPL}_LIBRARIES} )
                get_filename_component( _lib_name ${_lib} NAME_WE )
                string( REGEX REPLACE "^lib" "" _name ${_lib_name} )
                list( APPEND _TMP "-l${_name}" )
            endforeach()

            set( ${TPL}_LIBRARIES ${_TMP} )
            set( _TMP "" )
        endif()

        list( APPEND ECCODES_EXTRA_DEFINITIONS   ${${TPL}_DEFINITIONS} )
        list( APPEND ECCODES_EXTRA_INCLUDE_DIRS  ${${TPL}_INCLUDE_DIRS} ${${TPL}_INCLUDE_DIR} )
        list( APPEND ECCODES_EXTRA_LIBRARIES     ${${TPL}_LIBRARIES} )
    endif()
endforeach()

### include directories

include_directories(
    ${ECCODES_INCLUDE_DIRS}
    ${ECCODES_EXTRA_INCLUDE_DIRS}
    )

add_subdirectory( definitions ) # must be before memfs
add_subdirectory( memfs )
add_subdirectory( src )
if( HAVE_BUILD_TOOLS )
    add_subdirectory( tools )
endif()
add_subdirectory( fortran )

add_subdirectory( tests )
add_subdirectory( examples )
add_subdirectory( data )
add_subdirectory( samples )
add_subdirectory( ifs_samples ) # must come after samples

# ecbuild_dont_pack( DIRS samples DONT_PACK_REGEX "*.grib" )
ecbuild_dont_pack( FILES .cproject .project eccodes.code-workspace )
ecbuild_dont_pack( DIRS
    experimental deprecated doxygen confluence tests/deprecated tests/tests.ecmwf
    src/eccodes/deprecated tools/deprecated ifs_samples/grib1_mlgrib2_ieee32
    examples/examples.dev examples/extra examples/deprecated bamboo
    fortran/fortranCtypes share/eccodes  .settings )
#ecbuild_dont_pack( DIRS data/bufr  DONT_PACK_REGEX "*.bufr" )
#ecbuild_dont_pack( DIRS data/tigge DONT_PACK_REGEX "*.grib" )

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

###############################################################################
# export to other projects

# temporary -- add support for ecbuild 1.0.x sub-project inclusion
# to remove once mars server & client use eckit & ecbuild >= 1.1

if( HAVE_FORTRAN )
    list( APPEND ECCODES_INCLUDE_DIRS  ${CMAKE_Fortran_MODULE_DIRECTORY} )
    list( APPEND ECCODES_LIBRARIES eccodes_f90 )
endif()

# pkg-config
ecbuild_pkgconfig(
  NAME           eccodes
  URL            "https://confluence.ecmwf.int/display/ECC/"
  DESCRIPTION    "The ecCodes library"
  LIBRARIES      eccodes
  IGNORE_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIRS} ${NETCDF_INCLUDE_DIRS}
  VARIABLES      HAVE_MEMFS HAVE_GEOGRAPHY HAVE_JPEG HAVE_LIBJASPER HAVE_LIBOPENJPEG
                 HAVE_ECCODES_THREADS HAVE_ECCODES_OMP_THREADS
                 HAVE_NETCDF HAVE_FORTRAN HAVE_PNG HAVE_AEC HAVE_ECKIT_GEO
)
if( HAVE_FORTRAN )
  ecbuild_pkgconfig(
    NAME                eccodes_f90
    URL                 "https://confluence.ecmwf.int/display/ECC/"
    LIBRARIES           eccodes_f90 eccodes
    DESCRIPTION         "The ecCodes library for Fortran 90"
    IGNORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fortran ${PROJECT_BINARY_DIR}/fortran
                        ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIRS} ${NETCDF_INCLUDE_DIRS}
    VARIABLES           HAVE_MEMFS HAVE_GEOGRAPHY HAVE_JPEG HAVE_LIBJASPER HAVE_LIBOPENJPEG
                        HAVE_ECCODES_THREADS HAVE_ECCODES_OMP_THREADS
                        HAVE_NETCDF HAVE_PNG HAVE_AEC HAVE_ECKIT_GEO
  )
endif()

###############################################################################
# Debugging aid. Print all known CMake variables:
# get_cmake_property(_variableNames VARIABLES)
# foreach( _variableName ${_variableNames} )
#   ecbuild_info("  ${_variableName}=${${_variableName}}")
# endforeach()
###############################################################################
# finalize

ecbuild_install_project( NAME ${CMAKE_PROJECT_NAME} )

ecbuild_print_summary()

ecbuild_info("")
ecbuild_info("   +--------------------------+")
ecbuild_info("   |  ecCodes version ${eccodes_VERSION}  |")
ecbuild_info("   +--------------------------+")
ecbuild_info("")

ecbuild_info("For further installation instructions, please see:")
ecbuild_info("  https://confluence.ecmwf.int/display/ECC/ecCodes+installation")
ecbuild_info("")
