# ccsds: SpAec, a header-only C++ implementation of CCSDS 121.0-B (Adaptive
# Entropy Coding).
#
# Can be built and installed standalone:
#   cmake -S projects/ccsds -B build && cmake --build build && ctest --test-dir build
#   cmake --install build --prefix <prefix>
# and consumed with find_package(ccsds) -> target ccsds::spaec.
#
# ecCodes includes it with add_subdirectory() (see the top-level CMakeLists.txt).

cmake_minimum_required(VERSION 3.19)

project(ccsds VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(CCSDS_TOP_LEVEL ON)
else()
  set(CCSDS_TOP_LEVEL OFF)
endif()

option(CCSDS_BUILD_TESTS "Build the ccsds tests" ${CCSDS_TOP_LEVEL})
option(CCSDS_BUILD_FUZZERS "Build the ccsds libFuzzer fuzzers (requires Clang)" OFF)
option(CCSDS_INSTALL "Install the ccsds headers and CMake package" ${CCSDS_TOP_LEVEL})

# libFuzzer + ASan + UBSan instrumentation for the fuzzers and the code they exercise
if(CCSDS_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "CCSDS_BUILD_FUZZERS requires Clang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  set(CCSDS_FUZZER_COMPILE_OPTIONS
    -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined -g -O1)
  set(CCSDS_FUZZER_LINK_OPTIONS -fsanitize=fuzzer,address,undefined)
endif()

if(CCSDS_TOP_LEVEL)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

include(GNUInstallDirs)

# --- ccsds::spaec: the header-only SpAec library ---

add_library(ccsds_spaec INTERFACE)
add_library(ccsds::spaec ALIAS ccsds_spaec)
set_target_properties(ccsds_spaec PROPERTIES EXPORT_NAME spaec)
target_include_directories(ccsds_spaec INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(ccsds_spaec INTERFACE cxx_std_20)

if(CCSDS_BUILD_TESTS)
  if(CCSDS_TOP_LEVEL)
    enable_testing()
  endif()
  add_subdirectory(tests)
endif()

# --- libFuzzer fuzzers (Clang only; see CCSDS_FUZZER_* options above) ---

if(CCSDS_BUILD_FUZZERS)
  add_subdirectory(tests/fuzz)
endif()

# --- Installation ---

if(CCSDS_INSTALL)
  include(CMakePackageConfigHelpers)
  set(CCSDS_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/ccsds)

  install(DIRECTORY include/ccsds DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(TARGETS ccsds_spaec EXPORT ccsdsTargets)
  install(EXPORT ccsdsTargets NAMESPACE ccsds:: DESTINATION ${CCSDS_CMAKE_DIR})

  configure_package_config_file(cmake/ccsdsConfig.cmake.in
    ${PROJECT_BINARY_DIR}/ccsdsConfig.cmake
    INSTALL_DESTINATION ${CCSDS_CMAKE_DIR})
  write_basic_package_version_file(${PROJECT_BINARY_DIR}/ccsdsConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT)
  install(FILES
    ${PROJECT_BINARY_DIR}/ccsdsConfig.cmake
    ${PROJECT_BINARY_DIR}/ccsdsConfigVersion.cmake
    DESTINATION ${CCSDS_CMAKE_DIR})
endif()
