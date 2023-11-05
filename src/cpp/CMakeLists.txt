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

set(eccodes_dir				${CMAKE_CURRENT_LIST_DIR}/eccodes)
set(accessor_dir			${eccodes_dir}/accessor)
set(accessor_data_dir		${accessor_dir}/AccessorData)
set(accessor_utils_dir		${accessor_dir}/AccessorUtils)
set(accessor_grib_cpp_dir	${accessor_dir}/GribCpp)
set(accessor_test_dir		${accessor_dir}/Test)

include_directories(${accessor_dir})

set(accessor_src_files
	${accessor_dir}/Accessor.cc
	${accessor_dir}/Accessor.h
	${accessor_dir}/AccessorBuffer.h
	${accessor_dir}/AccessorDefs.h
	${accessor_dir}/AccessorFactory.cc
	${accessor_dir}/AccessorFactory.h
	${accessor_dir}/AccessorStore.cc
	${accessor_dir}/AccessorStore.h
	${accessor_dir}/AccessorTraits.h
)

add_subdirectory(converted)

set(accessor_data_src_files
	${accessor_data_dir}/AccessorData.cc
	${accessor_data_dir}/AccessorData.h
	${accessor_data_dir}/AccessorInitData.cc
	${accessor_data_dir}/AccessorInitData.h
	${converted_accessor_data_src_files}
)

set(accessor_utils_src_files
	${accessor_utils_dir}/AccessorException.h
	${accessor_utils_dir}/AccessorLogger.cc
	${accessor_utils_dir}/AccessorLogger.h
	${accessor_utils_dir}/ConversionHelper.cc
	${accessor_utils_dir}/ConversionHelper.h
	${accessor_utils_dir}/NamedType.h
	${accessor_utils_dir}/TypeString.h
)

set(accessor_grib_cpp_src_files
	${accessor_grib_cpp_dir}/GribQuery.cc
	${accessor_grib_cpp_dir}/GribQuery.h
	${accessor_grib_cpp_dir}/GribStatus.h
	${accessor_grib_cpp_dir}/GribType.cc
	${accessor_grib_cpp_dir}/GribType.h
	${accessor_grib_cpp_dir}/GribUtils.cc
	${accessor_grib_cpp_dir}/GribUtils.h
	${accessor_grib_cpp_dir}/GribValue.cc
	${accessor_grib_cpp_dir}/GribValue.h
	${accessor_grib_cpp_dir}/StringUtil.cc
	${accessor_grib_cpp_dir}/StringUtil.h
)


# Variables to be used in the parent CMakeLists.txt
set(cpp_accessor_src_files
	${accessor_src_files}
	${accessor_data_src_files}
	${accessor_data_funcs_src_files}
	${accessor_utils_src_files}
	${accessor_grib_cpp_src_files}
	PARENT_SCOPE
)

set(cpp_accessor_include_dir ${accessor_dir} PARENT_SCOPE)

ecbuild_add_executable( TARGET cpp_accessor_test
    NOINSTALL
    SOURCES		${all_accessor_src_files}
				${accessor_test_dir}/DateData.h
				${accessor_test_dir}/DoubleData.h
				${accessor_test_dir}/LongData.h
				${accessor_test_dir}/StringData.h
				${accessor_test_dir}/GribLayout.h
				${accessor_test_dir}/GribFile.h
				${accessor_test_dir}/GribFileTests.h
				${accessor_test_dir}/DateData.cc
				${accessor_test_dir}/DoubleData.cc
				${accessor_test_dir}/LongData.cc
				${accessor_test_dir}/StringData.cc
				${accessor_test_dir}/Test.cc
				${accessor_test_dir}/GribLayout.cc
				${accessor_test_dir}/GribFile.cc
				${accessor_test_dir}/GribFileTests.cc
    LIBS        eccodes ${CMAKE_THREAD_LIBS_INIT} )

#add_executable(cpp_accessor_test)
#target_link_libraries(cpp_accessor_test PUBLIC eccodes)
