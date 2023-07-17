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
set(eccodes_accessor_cplusplus_files
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/Accessor.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorConcept.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorException.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorFactory.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorMaker.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorPtr.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorUtils.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/Generic.h
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/Accessor.cc
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorFactory.cc
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorMaker.cc
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorPtr.cc
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/AccessorUtils.cc
    ${CMAKE_CURRENT_LIST_DIR}/eccodes/accessor/Generic.cc
    PARENT_SCOPE
)
