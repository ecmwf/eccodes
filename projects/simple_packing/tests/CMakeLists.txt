# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Test names are prefixed to avoid collisions when built as part of ecCodes
set(SP_TEST_PREFIX "simple_packing_")

# sp_add_test(<name> [SOURCES <sources...>])
# Builds <name>.cc into the executable sp_<name>, compiled like the library (optimised).
# sp_test_fp_flags(<target> <flags>...): tells the test (tests/fp_diagnostics.h) the floating-point
# options it is compiled with
function(sp_test_fp_flags target)
  string(JOIN " " flags ${ARGN})
  if(flags STREQUAL "")
    set(flags "(none)")
  endif()
  target_compile_definitions(${target} PRIVATE "SIMPLE_PACKING_TEST_FP_FLAGS=\"${flags}\"")
endfunction()

function(sp_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})
  add_executable(sp_${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc ${ARG_SOURCES})
  target_link_libraries(sp_${name} PRIVATE simple_packing::simple_packing)
  target_compile_options(sp_${name} PRIVATE ${SIMPLE_PACKING_OPT_FLAGS})
  sp_test_fp_flags(sp_${name} ${SIMPLE_PACKING_FP_FLAGS})
  target_link_options(sp_${name} PRIVATE ${SIMPLE_PACKING_FP_LINK_FLAGS})
  target_compile_definitions(sp_${name} PRIVATE NDEBUG)
endfunction()

# sp_label_tests(<label>)
# Labels all tests registered in the current directory with <label> and "simple_packing"
# (select them with ctest -L <label>).
function(sp_label_tests label)
  get_property(tests DIRECTORY PROPERTY TESTS)
  if(tests)
    set_tests_properties(${tests} PROPERTIES LABELS "${label};simple_packing")
  endif()
endfunction()

include(CheckCXXSourceCompiles)
find_package(Threads REQUIRED)

# Unit tests of the components; regression tests for bugs that were found
add_subdirectory(unit)
add_subdirectory(regression)
