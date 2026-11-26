# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Test names are prefixed to avoid collisions when built as part of ecCodes
set(SP_TEST_PREFIX "simple_packing_")

# sp_add_test(<name> [SOURCES <sources...>])
# Builds <name>.cc into the executable sp_<name>, compiled like the library (optimised).
# sp_test_fp_flags(<target> <flags>...): tells the test (tests/fp_diagnostics.h) the floating-point
# options it is compiled with (CMAKE_CXX_FLAGS and the given options)
function(sp_test_fp_flags target)
  # with the flags of the user (e.g. -fp-model=precise of icx, -ffast-math)
  string(JOIN " " flags ${CMAKE_CXX_FLAGS} ${ARGN})
  string(STRIP "${flags}" flags)
  string(REPLACE "\"" "\\\"" flags "${flags}")
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

# Install test: install into a temporary prefix, then find_package(simple_packing <version>) from a user project
# (only for native builds that install: the user program runs on the build machine)
if(SIMPLE_PACKING_INSTALL AND NOT CMAKE_CROSSCOMPILING)
  add_test(NAME ${SP_TEST_PREFIX}install
           COMMAND ${CMAKE_COMMAND}
                   -DBUILD_DIR=${PROJECT_BINARY_DIR}
                   -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_test
                   -DCONSUMER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/install/consumer
                   -DGENERATOR=${CMAKE_GENERATOR}
                   -DCXX_COMPILER=${CMAKE_CXX_COMPILER}
                   -DVERSION=${PROJECT_VERSION}
                   -DCONFIG=$<CONFIG>
                   -P ${CMAKE_CURRENT_SOURCE_DIR}/install/install_test.cmake)
  set_tests_properties(${SP_TEST_PREFIX}install PROPERTIES LABELS "install;simple_packing" TIMEOUT 300)
endif()
