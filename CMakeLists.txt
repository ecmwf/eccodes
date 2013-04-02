############################################################################################
# cmake options:
#
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_MODULE_PATH=/path/to/ecbuild/cmake
#
#       -DCMAKE_C_COMPILER=gcc
#       -DCMAKE_C_COMPILER=g++
#
#       -DCMAKE_PREFIX_PATH=/path/to/jasper:/path/to/any/package/out/of/place
#
#       -DGRIB_API_PATH=/path/to/grib

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( grib_api C )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include( ecbuild_system )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

option( GRIB_API_PYTHON "whether or not to build the GRIB_API python interface" ON )

###############################################################################
# macro processing

find_package( CMath )

find_package( SZip )
set( HAVE_LIBSZIP ${SZIP_FOUND} ) 

find_package( SWIG )
if( SWIG_FOUND )
    include( ${SWIG_USE_FILE} )
endif()

###############################################################################
# contents

### config header

configure_file( grib_api_config.h.in grib_api_config.h )

### include directories

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}     ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )

add_definitions( -DHAVE_GRIB_API_CONFIG_H )

if( NOT DEFINED GRIB_API_DEFINITION_PATH )
    set( GRIB_API_DEFINITION_PATH ${CMAKE_INSTALL_PREFIX}/share/grib_api/definitions )
endif()
if( NOT DEFINED GRIB_API_SAMPLES_PATH )
    set( GRIB_API_SAMPLES_PATH ${CMAKE_INSTALL_PREFIX}/share/grib_api/samples )
endif()

if( IEEE_BE )
    add_definitions( -DIEEE_BE )
endif()
if( IEEE_LE )
    add_definitions( -DIEEE_LE )
endif()

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
# installation 

if( NOT ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )
    set( GRIB_API_INCLUDE_DIRS    ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src PARENT_SCOPE )
    set( GRIB_API_LIBRARIES       grib_api ${CMATH_LIBRARIES} PARENT_SCOPE )
    set( GRIB_API_FOUND           TRUE PARENT_SCOPE )
endif()

############################################################################################
# finalize

ecbuild_install_project( NAME grib_api )

ecbuild_print_summary()
