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

set(converted_accessor_data_dir	${CMAKE_CURRENT_LIST_DIR})

# Define all the source files to build for the C++ accessors
# Note the PARENT_SCOPE option sets the variable in the calling CMakeLists.txt
# file - it remains undefined in this file: https://cmake.org/cmake/help/latest/command/set.html
set(converted_accessor_data_src_files
    ${converted_accessor_data_dir}/CodeflagData.cc
    ${converted_accessor_data_dir}/CodeflagData.h
    ${converted_accessor_data_dir}/UnsignedData.cc
    ${converted_accessor_data_dir}/UnsignedData.h
    ${converted_accessor_data_dir}/LongData.cc
    ${converted_accessor_data_dir}/LongData.h
    #${converted_accessor_data_dir}/CodetableData.cc
    #${converted_accessor_data_dir}/CodetableData.h
    PARENT_SCOPE
)
