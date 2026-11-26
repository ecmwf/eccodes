# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Test names are prefixed to avoid collisions when built as part of ecCodes
set(SPAEC_TEST_PREFIX "spaec_")

# spaec_add_test(<name> [LIBS <libs...>])
# Builds <name>.cc into an executable and registers it as test spaec_<name>.
# Always compiled with ${SPAEC_OPT_FLAGS} (release mode).
# spaec_test_fp_flags(<target> <flags>...): tells the test (tests/fp_diagnostics.h) the
# floating-point options it is compiled with (CMAKE_CXX_FLAGS and the given options)
function(spaec_test_fp_flags target)
  # with the flags of the user (e.g. -fp-model=precise of icx, -ffast-math)
  string(JOIN " " flags ${CMAKE_CXX_FLAGS} ${ARGN})
  string(STRIP "${flags}" flags)
  string(REPLACE "\"" "\\\"" flags "${flags}")
  if(flags STREQUAL "")
    set(flags "(none)")
  endif()
  target_compile_definitions(${target} PRIVATE "SPAEC_TEST_FP_FLAGS=\"${flags}\"")
endfunction()

function(spaec_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
  add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc ${ARG_SOURCES})
  target_link_libraries(${name} PRIVATE spaec::spaec ${ARG_LIBS})
  target_compile_options(${name} PRIVATE ${SPAEC_OPT_FLAGS})
  spaec_test_fp_flags(${name} ${SPAEC_FP_FLAGS})
  target_link_options(${name} PRIVATE ${SPAEC_FP_LINK_FLAGS})
  target_compile_definitions(${name} PRIVATE NDEBUG)
  add_test(NAME ${SPAEC_TEST_PREFIX}${name} COMMAND ${name})
endfunction()

find_package(Threads REQUIRED)

# spaec_label_tests(<label>)
# Labels all tests registered in the current directory with <label> and "spaec"
# (select them with ctest -L <label>).
function(spaec_label_tests label)
  get_property(tests DIRECTORY PROPERTY TESTS)
  if(tests)
    set_tests_properties(${tests} PROPERTIES LABELS "${label};spaec")
  endif()
endfunction()

# Unit tests of the components; regression tests for bugs that were found
add_subdirectory(unit)
add_subdirectory(regression)

# Install test: install into a temporary prefix, then find_package(spaec <version>) from a user project
# (only for native builds that install: the user program runs on the build machine)
if(SPAEC_INSTALL AND NOT CMAKE_CROSSCOMPILING)
  add_test(NAME ${SPAEC_TEST_PREFIX}install
           COMMAND ${CMAKE_COMMAND}
                   -DBUILD_DIR=${PROJECT_BINARY_DIR}
                   -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_test
                   -DCONSUMER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/install/consumer
                   -DGENERATOR=${CMAKE_GENERATOR}
                   -DCXX_COMPILER=${CMAKE_CXX_COMPILER}
                   -DVERSION=${PROJECT_VERSION}
                   -DCONFIG=$<CONFIG>
                   -P ${CMAKE_CURRENT_SOURCE_DIR}/install/install_test.cmake)
  set_tests_properties(${SPAEC_TEST_PREFIX}install PROPERTIES LABELS "install;spaec" TIMEOUT 300)
endif()
