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
    ${CMAKE_CURRENT_LIST_DIR}/ga_impl_creators.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl.h
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl_abstract_long_vector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl_factory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl_g1step_range.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl_gen.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl_helper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_accessor_impl_proj_string.cpp
    ${CMAKE_CURRENT_LIST_DIR}/grib_cpp_utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pack_buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/proj_string_helper.cpp
    PARENT_SCOPE
)
