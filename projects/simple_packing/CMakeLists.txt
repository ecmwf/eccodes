# simple_packing: GRIB simple packing (fixed-width bit packing of scaled values) with
# optimisers that choose the packing parameters (bits per value, binary and decimal scale
# factors, reference value).
#
# Can be built and installed standalone:
#   cmake -S projects/simple_packing -B build && cmake --build build && ctest --test-dir build
#   cmake --install build --prefix <prefix>
# and consumed with find_package(simple_packing) -> target simple_packing::simple_packing.
#
# ecCodes includes it with add_subdirectory() (see the top-level CMakeLists.txt).

cmake_minimum_required(VERSION 3.19)

project(simple_packing VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(SIMPLE_PACKING_TOP_LEVEL ON)
else()
  set(SIMPLE_PACKING_TOP_LEVEL OFF)
endif()

option(SIMPLE_PACKING_BUILD_TESTS "Build the simple_packing tests" ${SIMPLE_PACKING_TOP_LEVEL})
option(SIMPLE_PACKING_BUILD_FUZZERS "Build the simple_packing libFuzzer fuzzers (requires Clang)" OFF)
option(SIMPLE_PACKING_INSTALL "Install the simple_packing library, headers and CMake package" ${SIMPLE_PACKING_TOP_LEVEL})
option(SIMPLE_PACKING_NATIVE_ARCH "Optimise for the host CPU (-march=native): faster, but not portable" ON)

# libFuzzer + ASan + UBSan instrumentation for the fuzzers and the library code they exercise
if(SIMPLE_PACKING_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "SIMPLE_PACKING_BUILD_FUZZERS requires Clang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  set(SIMPLE_PACKING_FUZZER_COMPILE_OPTIONS -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined -g -O1)
  set(SIMPLE_PACKING_FUZZER_LINK_OPTIONS -fsanitize=fuzzer,address,undefined)
endif()

if(SIMPLE_PACKING_TOP_LEVEL)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

include(GNUInstallDirs)

# Optimisation flags for the packing loops (also used for the tests)
include(CheckCXXCompilerFlag)
set(SIMPLE_PACKING_OPT_FLAGS -O3)
if(SIMPLE_PACKING_NATIVE_ARCH AND NOT CMAKE_CROSSCOMPILING)
  check_cxx_compiler_flag("-march=native" SIMPLE_PACKING_HAVE_MARCH_NATIVE)
  if(SIMPLE_PACKING_HAVE_MARCH_NATIVE)
    list(APPEND SIMPLE_PACKING_OPT_FLAGS -march=native -mtune=native)
  endif()
endif()
if(NOT SIMPLE_PACKING_TOP_LEVEL)
  set(SIMPLE_PACKING_OPT_FLAGS ${SIMPLE_PACKING_OPT_FLAGS} PARENT_SCOPE)
endif()

# --- simple_packing::headers: the public headers ---

add_library(simple_packing_headers INTERFACE)
add_library(simple_packing::headers ALIAS simple_packing_headers)
set_target_properties(simple_packing_headers PROPERTIES EXPORT_NAME headers)
target_include_directories(simple_packing_headers INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(simple_packing_headers INTERFACE cxx_std_20)

# --- Compiled sources: an object library, so that ecCodes can embed them ---

add_library(simple_packing_objects OBJECT
  src/Utils.cc
  src/optimizer/BinDecOptimizer.cc)
target_link_libraries(simple_packing_objects PUBLIC simple_packing_headers)
set_target_properties(simple_packing_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(simple_packing_objects PRIVATE ${SIMPLE_PACKING_OPT_FLAGS})
target_compile_definitions(simple_packing_objects PRIVATE NDEBUG)

if(SIMPLE_PACKING_BUILD_FUZZERS)
  # The fuzzers need the library itself instrumented (they are built standalone, with the
  # tests switched off, since the instrumented objects cannot be linked uninstrumented)
  target_compile_options(simple_packing_objects PRIVATE ${SIMPLE_PACKING_FUZZER_COMPILE_OPTIONS})
endif()

# --- simple_packing::simple_packing: the library ---

add_library(simple_packing STATIC $<TARGET_OBJECTS:simple_packing_objects>)
add_library(simple_packing::simple_packing ALIAS simple_packing)
target_link_libraries(simple_packing PUBLIC simple_packing_headers)

if(SIMPLE_PACKING_BUILD_TESTS)
  if(SIMPLE_PACKING_TOP_LEVEL)
    enable_testing()
  endif()
  add_subdirectory(tests)
endif()

# --- libFuzzer fuzzers (Clang only; see SIMPLE_PACKING_FUZZER_* options above) ---

if(SIMPLE_PACKING_BUILD_FUZZERS)
  add_subdirectory(tests/fuzz)
endif()

# --- Installation ---

if(SIMPLE_PACKING_INSTALL)
  include(CMakePackageConfigHelpers)
  set(SIMPLE_PACKING_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/simple_packing)

  install(DIRECTORY include/simple_packing DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(TARGETS simple_packing_headers simple_packing EXPORT simple_packingTargets
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(EXPORT simple_packingTargets NAMESPACE simple_packing:: DESTINATION ${SIMPLE_PACKING_CMAKE_DIR})

  configure_package_config_file(cmake/simple_packingConfig.cmake.in
    ${PROJECT_BINARY_DIR}/simple_packingConfig.cmake
    INSTALL_DESTINATION ${SIMPLE_PACKING_CMAKE_DIR})
  write_basic_package_version_file(${PROJECT_BINARY_DIR}/simple_packingConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)
  install(FILES
    ${PROJECT_BINARY_DIR}/simple_packingConfig.cmake
    ${PROJECT_BINARY_DIR}/simple_packingConfigVersion.cmake
    DESTINATION ${SIMPLE_PACKING_CMAKE_DIR})
endif()
