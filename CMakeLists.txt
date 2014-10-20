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

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

set( GRIB_API_INSTALL_INCLUDE_DIR include CACHE PATH "GRIB_API override installation directory for header files")

include( ecbuild_system )

ecbuild_requires_macro_version( 1.3 )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

ecbuild_add_option( FEATURE JPG
	DESCRIPTION "support for JPG encoding"
	DEFAULT ON
)

ecbuild_add_option( FEATURE PNG
	DESCRIPTION "support for PNG encoding"
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
	DESCRIPTION "support for NetCDF"
	DEFAULT ON
	REQUIRED_PACKAGES NetCDF
)

ecbuild_add_option( FEATURE AEC
	DESCRIPTION "support for Adaptive Entropy Coding"
	DEFAULT OFF
	REQUIRED_PACKAGES AEC
)

ecbuild_add_option( FEATURE PYTHON
	DESCRIPTION "build the GRIB_API Python interface"
	DEFAULT ON
)

ecbuild_add_option( FEATURE FORTRAN
	DESCRIPTION "build the GRIB_API Fortran interface"
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

set( GRIB_API_EXTRA_LIBRARIES    "" )
set( GRIB_API_EXTRA_INCLUDE_DIRS "" )
set( GRIB_API_EXTRA_DEFINITIONS  "" )

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

if( NOT DEFINED GRIB_API_DEFINITION_PATH )
    set( GRIB_API_DEFINITION_PATH ${CMAKE_INSTALL_PREFIX}/share/grib_api/definitions )
endif()
if( NOT DEFINED GRIB_API_SAMPLES_PATH )
    set( GRIB_API_SAMPLES_PATH ${CMAKE_INSTALL_PREFIX}/share/grib_api/samples )
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

set( GRIB_API_TPLS            AEC PNG NetCDF Jasper OpenJPEG CMath )
set( GRIB_API_INCLUDE_DIRS    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( GRIB_API_LIBRARIES       grib_api )

get_directory_property( COMPILE_DEFINITIONS GRIB_API_DEFINITIONS )

foreach( _tpl ${GRIB_API_TPLS} )
    string( TOUPPER ${_tpl} TPL )
    if( ${TPL}_FOUND )
        list( APPEND GRIB_API_EXTRA_DEFINITIONS   ${${TPL}_DEFINITIONS} )
        list( APPEND GRIB_API_EXTRA_INCLUDE_DIRS  ${${TPL}_INCLUDE_DIRS} )
        list( APPEND GRIB_API_EXTRA_LIBRARIES     ${${TPL}_LIBRARIES} )
    endif()
endforeach()

### include directories

include_directories( ${GRIB_API_INCLUDE_DIRS}  ${GRIB_API_EXTRA_INCLUDE_DIRS} )

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
     concepts html tests.ecmwf doxygen examples.dev templates sms parameters java
     perl/GRIB-API/lib perl/GRIB-API/t gaussian_experimental gribex examples/F77
     examples/extra bamboo definitions/bufr use fortran/fortranCtypes tigge/tools
)

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

############################################################################################
# export to other projects

# temporary -- add support for ecbuild 1.0.x sub-project inclusion
# to remove once mars server & client use eckit & ecbuild >= 1.1

if( NOT ${PROJECT_NAME} STREQUAL  ${CMAKE_PROJECT_NAME} )
    set( GRIB_API_DEFINITIONS     ${GRIB_API_DEFINITIONS}  PARENT_SCOPE )# includes already TPL definitions
    set( GRIB_API_INCLUDE_DIRS    ${GRIB_API_INCLUDE_DIRS} ${GRIB_API_EXTRA_INCLUDE_DIRS} PARENT_SCOPE )
    set( GRIB_API_LIBRARIES       ${GRIB_API_LIBRARIES}    ${GRIB_API_EXTRA_LIBRARIES} ${CMATH_LIBRARIES} PARENT_SCOPE )
    set( GRIB_API_FOUND           TRUE PARENT_SCOPE )
endif()

############################################################################################
# finalize

ecbuild_install_project( NAME ${CMAKE_PROJECT_NAME} )

ecbuild_print_summary()
