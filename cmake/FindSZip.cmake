# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# - Try to find SZip
# Once done this will define
#  SZIP_FOUND - System has SZip
#  SZIP_INCLUDE_DIRS - The SZip include directories
#  SZIP_LIBRARIES - The libraries needed to use SZip

if( DEFINED SZIP_PATH )
    find_path( SZIP_INCLUDE_DIR szlib.h       PATHS ${SZIP_PATH}/include PATH_SUFFIXES szip NO_DEFAULT_PATH )
    find_library( SZIP_LIBRARY  NAMES szip sz PATHS ${SZIP_PATH}/lib     PATH_SUFFIXES szip NO_DEFAULT_PATH )
endif()

find_path( SZIP_INCLUDE_DIR szlib.h PATH_SUFFIXES szip )
find_library( SZIP_LIBRARY NAMES szip sz  PATH_SUFFIXES szip )

set( SZIP_LIBRARIES    ${SZIP_LIBRARY} )
set( SZIP_INCLUDE_DIRS ${SZIP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SZip  DEFAULT_MSG SZIP_LIBRARY SZIP_INCLUDE_DIR)

mark_as_advanced(SZIP_INCLUDE_DIR SZIP_LIBRARY )
