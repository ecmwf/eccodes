# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# spaec: SpAec, a header-only C++ implementation of CCSDS 121.0-B (Adaptive
# Entropy Coding).
#
# Can be built and installed standalone:
#   cmake -S projects/spaec -B build && cmake --build build && ctest --test-dir build
#   cmake --install build --prefix <prefix>
# and consumed with find_package(spaec) -> target spaec::spaec.
#
# ecCodes includes it with add_subdirectory() (see the top-level CMakeLists.txt).

cmake_minimum_required(VERSION 3.19)

# The version is defined in include/spaec/version.h
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/spaec/version.h" spaec_version_line
     REGEX "^#define SPAEC_VERSION \"[0-9]+\\.[0-9]+\\.[0-9]+\"")
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" spaec_version "${spaec_version_line}")
if(NOT spaec_version)
  message(FATAL_ERROR "spaec: no version in include/spaec/version.h")
endif()
project(spaec VERSION ${spaec_version} LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(SPAEC_TOP_LEVEL ON)
else()
  set(SPAEC_TOP_LEVEL OFF)
endif()

option(SPAEC_BUILD_TESTS "Build the spaec tests" ${SPAEC_TOP_LEVEL})
option(SPAEC_BUILD_FUZZERS "Build the spaec libFuzzer fuzzers (requires Clang)" OFF)
option(SPAEC_INSTALL "Install the spaec headers and CMake package" ${SPAEC_TOP_LEVEL})
option(SPAEC_NATIVE_ARCH "Optimise for the host CPU (-march=native): faster, but not portable" ON)

# libFuzzer + ASan + UBSan instrumentation for the fuzzers and the code they exercise
if(SPAEC_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "SPAEC_BUILD_FUZZERS requires Clang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  set(SPAEC_FUZZER_COMPILE_OPTIONS
    -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined -g -O1)
  set(SPAEC_FUZZER_LINK_OPTIONS -fsanitize=fuzzer,address,undefined)
endif()

if(SPAEC_TOP_LEVEL)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

include(GNUInstallDirs)

# Optimisation flags for the coder (tests; ecCodes uses them for its CCSDS accessor): -O3, the
# floating-point options that this compiler needs (cmake/spaecFloatingPoint.cmake: AUTO, ON or OFF
# per option, with their measured performance impact) and -march=native (SPAEC_NATIVE_ARCH)
include(CheckCXXCompilerFlag)
set(SPAEC_ARCH_FLAGS "")
if(SPAEC_NATIVE_ARCH AND NOT CMAKE_CROSSCOMPILING)
  check_cxx_compiler_flag("-march=native" SPAEC_HAVE_MARCH_NATIVE)
  if(SPAEC_HAVE_MARCH_NATIVE)
    set(SPAEC_ARCH_FLAGS -march=native -mtune=native)
  endif()
endif()
include(${PROJECT_SOURCE_DIR}/cmake/spaecFloatingPoint.cmake)
spaec_floating_point_flags(${SPAEC_ARCH_FLAGS})
# MSVC has no -O3: /O2 (its highest optimisation level)
if(MSVC)
  set(SPAEC_OPT_LEVEL /O2)
else()
  set(SPAEC_OPT_LEVEL -O3)
endif()
set(SPAEC_OPT_FLAGS ${SPAEC_OPT_LEVEL} ${SPAEC_FP_FLAGS} ${SPAEC_ARCH_FLAGS})
if(NOT SPAEC_TOP_LEVEL)
  set(SPAEC_OPT_FLAGS ${SPAEC_OPT_FLAGS} PARENT_SCOPE)
  set(SPAEC_FP_SAFETY_FLAGS ${SPAEC_FP_SAFETY_FLAGS} PARENT_SCOPE)
  set(SPAEC_FP_LINK_FLAGS ${SPAEC_FP_LINK_FLAGS} PARENT_SCOPE)
endif()

# --- spaec::spaec: the header-only SpAec library ---

add_library(spaec INTERFACE)
add_library(spaec::spaec ALIAS spaec)
set_target_properties(spaec PROPERTIES EXPORT_NAME spaec)
target_include_directories(spaec INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(spaec INTERFACE cxx_std_17)
# Users get the results of the project's own builds: the floating-point options that this compiler
# needs for correct results (SPAEC_FP_SAFETY_FLAGS; the header code is compiled with the user's
# flags). Not -fno-trapping-math, which only affects the speed.
foreach(spaec_flag IN LISTS SPAEC_FP_SAFETY_FLAGS)
  target_compile_options(spaec INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${spaec_flag}>)
endforeach()
target_link_options(spaec INTERFACE ${SPAEC_FP_LINK_FLAGS})

if(SPAEC_BUILD_TESTS)
  if(SPAEC_TOP_LEVEL)
    enable_testing()
  endif()
  add_subdirectory(tests)
endif()

# --- libFuzzer fuzzers (Clang only; see SPAEC_FUZZER_* options above) ---

if(SPAEC_BUILD_FUZZERS)
  add_subdirectory(tests/fuzz)
endif()

# --- Installation ---

if(SPAEC_INSTALL)
  include(CMakePackageConfigHelpers)
  set(SPAEC_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/spaec)

  install(DIRECTORY include/spaec DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(TARGETS spaec EXPORT spaecTargets)
  install(EXPORT spaecTargets NAMESPACE spaec:: DESTINATION ${SPAEC_CMAKE_DIR})

  configure_package_config_file(cmake/spaecConfig.cmake.in
    ${PROJECT_BINARY_DIR}/spaecConfig.cmake
    INSTALL_DESTINATION ${SPAEC_CMAKE_DIR})
  write_basic_package_version_file(${PROJECT_BINARY_DIR}/spaecConfigVersion.cmake
    COMPATIBILITY SameMinorVersion  # before 1.0.0 a new minor version may change the interface
    ARCH_INDEPENDENT)
  install(FILES
    ${PROJECT_BINARY_DIR}/spaecConfig.cmake
    ${PROJECT_BINARY_DIR}/spaecConfigVersion.cmake
    DESTINATION ${SPAEC_CMAKE_DIR})
  install(FILES LICENSE CHANGELOG.md README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()
