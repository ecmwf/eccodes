############################################################################################
# cmake options:
#
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_MODULE_PATH=/path/to/ecbuild/cmake

# TODO:
#   * build python bindings

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( grib_api C )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

set( GRIB_API_INSTALL_INCLUDE_DIR include CACHE PATH "GRIB_API override installation directory for header files")

include( ecbuild_system )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

option( GRIB_API_JPG     "try to add support for JPG encoding"          ON )
option( GRIB_API_PNG     "try to add support for PNG encoding"          ON )
option( GRIB_API_NETCDF  "try to add support for NetCDF"                ON )
option( GRIB_API_SZIP    "try to add support for SZip encoding"         ON )
option( GRIB_API_PYTHON  "try to build the GRIB_API Python interface"   ON )
option( GRIB_API_FORTRAN "try to build the GRIB_API Fortran interface"  ON )

option( GRIB_API_MEMORY_MANAGEMENT "enable memory management"  OFF )
option( GRIB_API_ALIGN_MEMORY      "enable memory alignment"   OFF )

option( GRIB_TIMER       "enable timer"    OFF )
option( GRIB_THREADS     "enable threads"  OFF )

###############################################################################
# macro processing

set( GRIB_API_EXTRA_LIBRARIES    "" )
set( GRIB_API_EXTRA_INCLUDE_DIRS "" )
set( GRIB_API_EXTRA_DEFINITIONS  "" )

find_package( CMath )

### SZIP support

set( HAVE_LIBSZIP 0 )
if( GRIB_API_SZIP )
    find_package( SZip )
    if( SZIP_FOUND )
        set( HAVE_LIBSZIP 1 )
        list( APPEND GRIB_API_EXTRA_LIBRARIES    ${SZIP_LIBRARIES} )
        list( APPEND GRIB_API_EXTRA_INCLUDE_DIRS ${SZIP_INCLUDE_DIRS} )
    endif()
endif()

### JPG support

set( HAVE_JPEG 0 )
set( HAVE_LIBJASPER 0 )
set( HAVE_LIBOPENJPEG 0 )

if( GRIB_API_JPG )

    find_package( Jasper )
    find_package( OpenJPEG )

    if( JASPER_FOUND )
        set( HAVE_JPEG 1 )
        set( HAVE_LIBJASPER 1 )
        list( APPEND GRIB_API_EXTRA_LIBRARIES    ${JASPER_LIBRARIES} )
        list( APPEND GRIB_API_EXTRA_INCLUDE_DIRS ${JASPER_INCLUDE_DIR} )
    endif()

    if( OpenJPEG_FOUND )
        set( HAVE_JPEG 1 )
        set( HAVE_LIBOPENJPEG 1 )
        list( APPEND GRIB_API_EXTRA_LIBRARIES    ${OPENJPEG_LIBRARIES} )
        list( APPEND GRIB_API_EXTRA_INCLUDE_DIRS ${OPENJPEG_INCLUDE_DIR} )
    endif()

endif()

### PNG support

set( HAVE_LIBPNG 0 )

if( GRIB_API_PNG )

  find_package( PNG )

  if( PNG_FOUND )
        set( HAVE_LIBPNG 1 )
        list( APPEND GRIB_API_EXTRA_LIBRARIES    ${PNG_LIBRARIES}   )
        list( APPEND GRIB_API_EXTRA_INCLUDE_DIRS ${PNG_INCLUDE_DIR} )
        list( APPEND GRIB_API_EXTRA_DEFINITIONS  ${PNG_DEFINITIONS} )
        add_definitions( ${PNG_DEFINITIONS} )
  endif()

endif()

### NetCDF support

set( HAVE_NETCDF 0 )

if( GRIB_API_NETCDF )

  find_package( NetCDF )

  if( NETCDF_FOUND )
        set( HAVE_NETCDF 1 )
        list( APPEND GRIB_API_EXTRA_LIBRARIES    ${NETCDF_LIBRARIES}   )
        list( APPEND GRIB_API_EXTRA_INCLUDE_DIRS ${NETCDF_INCLUDE_DIR} )
  endif()

endif()

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
if( GRIB_API_MEMORY_MANAGEMENT )
    set( MANAGE_MEM 1 )
endif()

set( GRIB_PTHREADS 0 )
set( GRIB_LINUX_PTHREADS 0 )
if( GRIB_THREADS AND CMAKE_THREAD_LIBS_INIT )
    if( CMAKE_USE_PTHREADS_INIT )
        set( GRIB_PTHREADS 1 )
        if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
            set( GRIB_LINUX_PTHREADS 1 )
        endif()
    endif()
endif()

set( GRIB_MEM_ALIGN 0 )
if( GRIB_API_ALIGN_MEMORY )
  set( GRIB_MEM_ALIGN 1 )
endif()

###############################################################################
# contents

if( NOT DEFINED GRIB_API_DEFINITION_PATH )
    set( GRIB_API_DEFINITION_PATH ${CMAKE_INSTALL_PREFIX}/share/grib_api/definitions )
endif()
if( NOT DEFINED GRIB_API_SAMPLES_PATH )
    set( GRIB_API_SAMPLES_PATH ${CMAKE_INSTALL_PREFIX}/share/grib_api/samples )
endif()

### config header

configure_file( grib_api_config.h.in grib_api_config.h )

add_definitions( -DHAVE_GRIB_API_CONFIG_H )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/grib_api_config.h DESTINATION ${INSTALL_INCLUDE_DIR} )

if( CMAKE_COMPILER_IS_GNUCC )
    cmake_add_c_flags("-pedantic")
endif()

### include directories

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}     ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )

add_subdirectory( src )
add_subdirectory( tools )
add_subdirectory( fortran )
add_subdirectory( python )
add_subdirectory( definitions )
add_subdirectory( tests )
add_subdirectory( tigge )
add_subdirectory( examples )

add_subdirectory( samples )
add_subdirectory( ifs_samples ) # must come after samples

# ecbuild_dont_pack( DIRS samples DONT_PACK_REGEX "*.grib" )

############################################################################################
# export to other projects

# temporary -- for old ecbuild 1.0

if( NOT ${PROJECT_NAME} STREQUAL  ${CMAKE_PROJECT_NAME} )
    set( GRIB_API_DEFINITIONS     ${GRIB_API_EXTRA_DEFINITIONS} PARENT_SCOPE )
    set( GRIB_API_INCLUDE_DIRS    ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src ${GRIB_API_EXTRA_INCLUDE_DIRS} PARENT_SCOPE )
    set( GRIB_API_LIBRARIES       grib_api ${GRIB_API_EXTRA_LIBRARIES} ${CMATH_LIBRARIES} PARENT_SCOPE )
    set( GRIB_API_FOUND           TRUE PARENT_SCOPE )
endif()

# new -- for ecbuild >= 1.1

set( GRIB_API_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src ${GRIB_API_EXTRA_INCLUDE_DIRS} )
set( GRIB_API_DEFINITIONS    ${GRIB_API_EXTRA_DEFINITIONS} )
set( GRIB_API_LIBRARIES      grib_api ${GRIB_API_EXTRA_LIBRARIES} ${CMATH_LIBRARIES} )

############################################################################################
# finalize

ecbuild_install_project( NAME grib_api )

ecbuild_print_summary()
