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

# Define all the source files to build for the C++ accessors
# Note the PARENT_SCOPE option sets the variable in the calling CMakeLists.txt
# file - it remains undefined in this file: https://cmake.org/cmake/help/latest/command/set.html
set(eccodes_grib_accessor_impl_src_files
    ${CMAKE_CURRENT_LIST_DIR}/Accessor_Utils.h
    ${CMAKE_CURRENT_LIST_DIR}/Accessor.h
    ${CMAKE_CURRENT_LIST_DIR}/AccessorConcept.h
    ${CMAKE_CURRENT_LIST_DIR}/AccessorFactory.h
    ${CMAKE_CURRENT_LIST_DIR}/AccessorMaker.h
    ${CMAKE_CURRENT_LIST_DIR}/AccessorPtr.h
    ${CMAKE_CURRENT_LIST_DIR}/Generic.h
    ${CMAKE_CURRENT_LIST_DIR}/ProjString.h
    ${CMAKE_CURRENT_LIST_DIR}/Accessor_Utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Accessor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AccessorFactory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AccessorMaker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AccessorPtr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Generic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ProjString.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pack_buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/proj_string_helper.cpp
    PARENT_SCOPE
)
