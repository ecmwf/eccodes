cmake_minimum_required(VERSION 3.19.6)
project(
  simple_packing
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src/simple)

enable_testing()
add_subdirectory(tests)

