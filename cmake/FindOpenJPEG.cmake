# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# - Try to find the OpenJPEG includes and library (version 1.5.x or 2.1.x)
# This module defines
#
#  OPENJPEG_FOUND         - System has OpenJPEG
#  OPENJPEG_INCLUDE_DIRS  - the OpenJPEG include directories
#  OPENJPEG_LIBRARIES     - the libraries needed to use OpenJPEG
#
# The following paths will be searched with priority if set in CMake or env
#
#  OPENJPEG_DIR   - root folder of the OpenJPEG installation
#  OPENJPEG_PATH  - root folder of the OpenJPEG installation

# Note: OpenJPEG has a version-specific subdirectory in the include
# e.g. include/openjpeg-2.0 or include/openjpeg-2.1.
# Only version 1.5.x and 2.1.x are supported.
# The library name is different for 1.x (libopenjpeg) and 2.x (libopenjp2).

set( _suff include include/openjpeg include/openjpeg-1.5 include/openjpeg-2.1 )
find_path( OPENJPEG_INCLUDE_DIR openjpeg.h
           PATHS ${OPENJPEG_PATH} ENV OPENJPEG_PATH
                 ${OPENJPEG_DIR}  ENV OPENJPEG_DIR
           PATH_SUFFIXES ${_suff}
           NO_DEFAULT_PATH )
find_path( OPENJPEG_INCLUDE_DIR  openjpeg.h
           PATH_SUFFIXES ${_suff} )
unset( _suff )

find_library( OPENJPEG_LIBRARY NAMES openjpeg openjp2
              PATHS ${OPENJPEG_PATH} ENV OPENJPEG_PATH
                    ${OPENJPEG_DIR}  ENV OPENJPEG_DIR
              PATH_SUFFIXES lib lib/openjpeg
              NO_DEFAULT_PATH )
find_library( OPENJPEG_LIBRARY NAMES openjpeg openjp2
              PATH_SUFFIXES lib lib/openjpeg )

set( OPENJPEG_LIBRARIES    ${OPENJPEG_LIBRARY} )
set( OPENJPEG_INCLUDE_DIRS ${OPENJPEG_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set OPENJPEG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OpenJPEG  DEFAULT_MSG
                                  OPENJPEG_LIBRARY OPENJPEG_INCLUDE_DIR)

mark_as_advanced( OPENJPEG_INCLUDE_DIR OPENJPEG_LIBRARY )
