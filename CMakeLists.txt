############################################################################################
# cmake options:
#
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_MODULE_PATH=/path/to/ecbuild/cmake

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( eccodes C )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

set( ECCODES_INSTALL_INCLUDE_DIR include CACHE PATH "ecCodes override installation directory for header files")

include( ecbuild_system )

ecbuild_requires_macro_version( 1.3 )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

ecbuild_add_option( FEATURE JPG
    DESCRIPTION "support for JPG decoding/encoding"
    DEFAULT ON
)

ecbuild_add_option( FEATURE PNG
    DESCRIPTION "support for PNG decoding/encoding"
    DEFAULT OFF
    REQUIRED_PACKAGES PNG
)

if( HAVE_PNG )
    set( HAVE_LIBPNG 1 ) # compatibility with autotools
    add_definitions( ${PNG_DEFINITIONS} )
else()
    set( HAVE_LIBPNG 0 )
endif()

ecbuild_add_option( FEATURE NETCDF
    DESCRIPTION "support for GRIB to NetCDF conversion"
    DEFAULT ON
    REQUIRED_PACKAGES NetCDF
)

ecbuild_add_option( FEATURE AEC
    DESCRIPTION "support for Adaptive Entropy Coding"
    DEFAULT OFF
    REQUIRED_PACKAGES AEC
)

ecbuild_add_option( FEATURE PYTHON
    DESCRIPTION "build the ecCodes Python interface"
    DEFAULT ON
)

ecbuild_add_option( FEATURE FORTRAN
    DESCRIPTION "build the ecCodes Fortran interface"
    DEFAULT ON
)

if( ENABLE_FORTRAN )
    # will set EC_HAVE_FORTRAN with the result
    ecbuild_enable_fortran( MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/fortran/modules )
    set( HAVE_FORTRAN ${EC_HAVE_FORTRAN} )
endif()

# advanced options (not visible in cmake-gui )

ecbuild_add_option( FEATURE MEMORY_MANAGEMENT DESCRIPTION "enable memory management" DEFAULT OFF ADVANCED )
ecbuild_add_option( FEATURE ALIGN_MEMORY      DESCRIPTION "enable memory alignment"  DEFAULT OFF ADVANCED )

ecbuild_add_option( FEATURE GRIB_TIMER       DESCRIPTION "enable timer"   DEFAULT OFF ADVANCED )
ecbuild_add_option( FEATURE GRIB_THREADS     DESCRIPTION "enable threads" DEFAULT OFF ADVANCED )

###############################################################################
# macro processing

set( ECCODES_EXTRA_LIBRARIES    "" )
set( ECCODES_EXTRA_INCLUDE_DIRS "" )
set( ECCODES_EXTRA_DEFINITIONS  "" )

find_package( CMath )

### JPG support

set( HAVE_JPEG 0 )
set( HAVE_LIBJASPER 0 )
set( HAVE_LIBOPENJPEG 0 )

if( ENABLE_JPG )

    ecbuild_add_extra_search_paths( jasper ) # help standard cmake macro with ecmwf paths
    find_package( Jasper )

    find_package( OpenJPEG )

    if( JASPER_FOUND )
        set( HAVE_JPEG 1 )
        set( HAVE_LIBJASPER 1 )
    endif()

    if( OpenJPEG_FOUND )
        set( HAVE_JPEG 1 )
        set( HAVE_LIBOPENJPEG 1 )
    endif()

endif()


###############################################################################
# other options

if( GRIB_TIMER )
    set( GRIB_TIMER 1 )
else()
    set( GRIB_TIMER 0 )
endif()

set( IS_BIG_ENDIAN 0 )
if( EC_BIG_ENDIAN )
    set( IS_BIG_ENDIAN 1 )
endif()

set( MANAGE_MEM 0 )
if( ENABLE_MEMORY_MANAGEMENT )
    set( MANAGE_MEM 1 )
endif()

set( CMAKE_THREAD_PREFER_PTHREAD 1 ) # find thread library, but prefer pthreads
find_package(Threads REQUIRED)

# debug
message(STATUS " CMAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT}")
message(STATUS " CMAKE_USE_PTHREADS_INIT=${CMAKE_USE_PTHREADS_INIT}")
message(STATUS " GRIB_THREADS=${GRIB_THREADS}, HAVE_GRIB_THREADS=${HAVE_GRIB_THREADS}")

if( NOT ${CMAKE_USE_PTHREADS_INIT} )
    message( FATAL_ERROR "Only pthreads supported - thread library found is [${CMAKE_THREAD_LIBS_INIT}]" )
endif()

set( GRIB_PTHREADS 0 )
set( GRIB_LINUX_PTHREADS 0 )
#if( HAVE_GRIB_THREADS AND CMAKE_THREAD_LIBS_INIT )
if( HAVE_GRIB_THREADS )
    if( CMAKE_USE_PTHREADS_INIT )
        set( GRIB_PTHREADS 1 )
        if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
            set( GRIB_LINUX_PTHREADS 1 )
        endif()
    endif()
endif()
message(STATUS " GRIB_PTHREADS=${GRIB_PTHREADS}")

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

###############################################################################
# contents

if( NOT ${DEVELOPER_MODE} )
    set( grib_api_default_data_prefix ${CMAKE_INSTALL_PREFIX} )
else()
    set( grib_api_default_data_prefix ${CMAKE_BINARY_DIR} )
endif()

if( NOT DEFINED ECCODES_DEFINITION_PATH )
    set( ECCODES_DEFINITION_PATH ${grib_api_default_data_prefix}/share/eccodes/definitions )
endif()
if( NOT DEFINED ECCODES_SAMPLES_PATH )
    set( ECCODES_SAMPLES_PATH ${grib_api_default_data_prefix}/share/eccodes/samples )
endif()
if( NOT DEFINED ECCODES_IFS_SAMPLES_PATH )
    set( ECCODES_IFS_SAMPLES_PATH ${grib_api_default_data_prefix}/share/eccodes/ifs_samples )
endif()

### config header

ecbuild_generate_config_headers()

configure_file( grib_api_config.h.in grib_api_config.h )

add_definitions( -DHAVE_GRIB_API_CONFIG_H )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/grib_api_config.h DESTINATION ${INSTALL_INCLUDE_DIR} )

if( CMAKE_COMPILER_IS_GNUCC )
    cmake_add_c_flags("-pedantic")
endif()

############################################################################################
# contents

### export package to other ecbuild packages

set( ECCODES_TPLS            AEC PNG Jasper OpenJPEG CMath )
set( ECCODES_INCLUDE_DIRS    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECCODES_LIBRARIES       eccodes )

get_directory_property( COMPILE_DEFINITIONS ECCODES_DEFINITIONS )

foreach( _tpl ${ECCODES_TPLS} )
    string( TOUPPER ${_tpl} TPL )
    if( ${TPL}_FOUND )
        list( APPEND ECCODES_EXTRA_DEFINITIONS   ${${TPL}_DEFINITIONS} )
        list( APPEND ECCODES_EXTRA_INCLUDE_DIRS  ${${TPL}_INCLUDE_DIRS} ${${TPL}_INCLUDE_DIR} )
        list( APPEND ECCODES_EXTRA_LIBRARIES     ${${TPL}_LIBRARIES} )
    endif()
endforeach()

### include directories

include_directories( ${ECCODES_INCLUDE_DIRS}  ${ECCODES_EXTRA_INCLUDE_DIRS} )

add_subdirectory( src )
add_subdirectory( tools )
add_subdirectory( fortran )
add_subdirectory( python )
add_subdirectory( definitions )
add_subdirectory( tests )
add_subdirectory( tigge )
add_subdirectory( examples )
add_subdirectory( data )

add_subdirectory( samples )
add_subdirectory( ifs_samples ) # must come after samples

# ecbuild_dont_pack( DIRS samples DONT_PACK_REGEX "*.grib" )
ecbuild_dont_pack( DIRS
     concepts html tests.ecmwf doxygen examples.dev templates parameters java
     perl/GRIB-API/lib perl/GRIB-API/t gaussian_experimental gribex examples/F77
     examples/extra bamboo fortran/fortranCtypes tigge/tools share/grib_api grib_api_for_mars
)
#ecbuild_dont_pack( DIRS data/bufr  DONT_PACK_REGEX "*.bufr" )
#ecbuild_dont_pack( DIRS data/tigge DONT_PACK_REGEX "*.grib" )

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

############################################################################################
# export to other projects

# temporary -- add support for ecbuild 1.0.x sub-project inclusion
# to remove once mars server & client use eckit & ecbuild >= 1.1

if( EC_HAVE_FORTRAN )
    list( APPEND ECCODES_INCLUDE_DIRS  ${CMAKE_Fortran_MODULE_DIRECTORY} )
    list( APPEND ECCODES_LIBRARIES eccodes_f77 eccodes_f90 )
endif()

if( NOT PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME )
    set( ECCODES_DEFINITIONS       ${ECCODES_DEFINITIONS}  PARENT_SCOPE )# includes already TPL definitions
    set( ECCODES_INCLUDE_DIRS     ${ECCODES_INCLUDE_DIRS} ${ECCODES_EXTRA_INCLUDE_DIRS} PARENT_SCOPE )
    set( ECCODES_LIBRARIES        ${ECCODES_LIBRARIES}    ${ECCODES_EXTRA_LIBRARIES} ${CMATH_LIBRARIES} PARENT_SCOPE )
    set( ECCODES_FOUND             TRUE PARENT_SCOPE )
    set( ECCODES_SAMPLES_PATH      ${ECCODES_SAMPLES_PATH} PARENT_SCOPE )
    set( ECCODES_IFS_SAMPLES_PATH  ${ECCODES_IFS_SAMPLES_PATH} PARENT_SCOPE )
    set( ECCODES_DEFINITION_PATH   ${ECCODES_DEFINITION_PATH} PARENT_SCOPE )
    set( ECCODES_INSTALL_ECMWF_TOOLS  ${ECCODES_INSTALL_ECMWF_TOOLS} PARENT_SCOPE )
endif()

# pkg-config
ecbuild_pkgconfig(
  NAME                eccodes
  URL                 "https://software.ecmwf.int/wiki/display/ECC/"
  DESCRIPTION         "The ecCodes library"
  LIBRARIES           eccodes
  VARIABLES           HAVE_JPG HAVE_LIBJASPER HAVE_GRIB_THREADS HAVE_NETCDF
                      HAVE_PYTHON HAVE_FORTRAN HAVE_PNG HAVE_AEC
)
if( EC_HAVE_FORTRAN )
  ecbuild_pkgconfig(
    NAME                eccodes_f90
    URL                 "https://software.ecmwf.int/wiki/display/ECC/"
    LIBRARIES           eccodes_f90 eccodes
    DESCRIPTION         "The ecCodes library for Fortran 90"
    IGNORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fortran ${PROJECT_BINARY_DIR}/fortran
    VARIABLES           HAVE_JPG HAVE_LIBJASPER HAVE_GRIB_THREADS HAVE_NETCDF
                        HAVE_PYTHON HAVE_PNG HAVE_AEC
  )
  ecbuild_pkgconfig(
    NAME                eccodes_f77
    URL                 "https://software.ecmwf.int/wiki/display/ECC/"
    LIBRARIES           eccodes_f77 eccodes
    DESCRIPTION         "The ecCodes library for Fortran 77"
    IGNORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fortran ${PROJECT_BINARY_DIR}/fortran
    VARIABLES           HAVE_JPG HAVE_LIBJASPER HAVE_GRIB_THREADS HAVE_NETCDF
                        HAVE_PYTHON HAVE_PNG HAVE_AEC
  )
endif()

############################################################################################
# finalize

ecbuild_install_project( NAME ${CMAKE_PROJECT_NAME} )

ecbuild_print_summary()
