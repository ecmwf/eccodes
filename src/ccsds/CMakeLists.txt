cmake_minimum_required(VERSION 3.19.6)
project(Tutorial)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-march=native)


# Find libaec
set(libaec_USE_STATIC_LIBS ON)
find_package(libaec 1.1.6 CONFIG REQUIRED)

message(STATUS "libaec_VERSION: ${libaec_VERSION}")
message(STATUS "libaec_FOUND: ${libaec_FOUND}")

include_directories(
  ${PROJECT_SOURCE_DIR}/src/ccsds
  )

add_subdirectory(src)
add_subdirectory(tests)

