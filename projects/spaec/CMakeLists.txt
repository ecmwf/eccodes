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

project(spaec VERSION 1.0.0 LANGUAGES CXX)

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

# Optimisation flags for the coder (tests; ecCodes uses them for its CCSDS accessor)
include(CheckCXXCompilerFlag)
set(SPAEC_OPT_FLAGS -O3)
# Undo a -ffast-math of the user (CMAKE_CXX_FLAGS come first): it removes the checks for NaN and
# infinite values (std::isfinite is assumed true), which then pass invalid data or loop forever
check_cxx_compiler_flag("-fno-fast-math" SPAEC_HAVE_NO_FAST_MATH)
if(SPAEC_HAVE_NO_FAST_MATH)
  list(PREPEND SPAEC_OPT_FLAGS -fno-fast-math)
endif()
# Lets GCC vectorize loops with floating-point comparisons (e.g. the range check of the values
# to encode); SpAec does not rely on floating-point exception flags
check_cxx_compiler_flag("-fno-trapping-math" SPAEC_HAVE_NO_TRAPPING_MATH)
if(SPAEC_HAVE_NO_TRAPPING_MATH)
  list(APPEND SPAEC_OPT_FLAGS -fno-trapping-math)
endif()
# Every floating-point operation rounded separately: with fused multiply-adds (available with
# -march=native) the encoded values would depend on the build machine
check_cxx_compiler_flag("-ffp-contract=off" SPAEC_HAVE_FP_CONTRACT_OFF)
if(SPAEC_HAVE_FP_CONTRACT_OFF)
  list(APPEND SPAEC_OPT_FLAGS -ffp-contract=off)
endif()
if(SPAEC_NATIVE_ARCH AND NOT CMAKE_CROSSCOMPILING)
  check_cxx_compiler_flag("-march=native" SPAEC_HAVE_MARCH_NATIVE)
  if(SPAEC_HAVE_MARCH_NATIVE)
    list(APPEND SPAEC_OPT_FLAGS -march=native -mtune=native)
  endif()
endif()
if(NOT SPAEC_TOP_LEVEL)
  set(SPAEC_OPT_FLAGS ${SPAEC_OPT_FLAGS} PARENT_SCOPE)
endif()

# --- spaec::spaec: the header-only SpAec library ---

add_library(spaec INTERFACE)
add_library(spaec::spaec ALIAS spaec)
set_target_properties(spaec PROPERTIES EXPORT_NAME spaec)
target_include_directories(spaec INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(spaec INTERFACE cxx_std_17)
# Users get the results of the project's own builds: no fused multiply-adds in the header code they
# instantiate (they round differently, so the encoded data would depend on the CPU)
# Nor the unsafe floating-point optimisations of -ffast-math (they remove the checks for NaN and
# infinity) or of the default floating-point model of icx/icpx
target_compile_options(spaec INTERFACE
  $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>:-fno-fast-math>
  $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,IntelLLVM>:-ffp-contract=off>
  $<$<COMPILE_LANG_AND_ID:CXX,IntelLLVM>:-fp-model=precise>)

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
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT)
  install(FILES
    ${PROJECT_BINARY_DIR}/spaecConfig.cmake
    ${PROJECT_BINARY_DIR}/spaecConfigVersion.cmake
    DESTINATION ${SPAEC_CMAKE_DIR})
endif()
