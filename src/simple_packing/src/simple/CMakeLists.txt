# Header-only simple packing library (INTERFACE target for eccodes linkage)
add_library(simple_packing_sp INTERFACE)
target_include_directories(simple_packing_sp INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/codec
  ${CMAKE_CURRENT_SOURCE_DIR}/optimizer
  ${CMAKE_CURRENT_SOURCE_DIR}/simple
)
target_compile_features(simple_packing_sp INTERFACE cxx_std_20)

# Compiled library (for tests and standalone use)
add_library(simple_packing_lib STATIC
    LibEccodes.cc
    Utils.cc
    optimizer/DecOptimizer.cc
    optimizer/BinDecOptimizer.cc
)
target_include_directories(simple_packing_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/codec
  ${CMAKE_CURRENT_SOURCE_DIR}/optimizer
  ${CMAKE_CURRENT_SOURCE_DIR}/simple
)
target_compile_features(simple_packing_lib PUBLIC cxx_std_23)
target_compile_options(simple_packing_lib PRIVATE -march=native -mtune=native -O3)
target_compile_definitions(simple_packing_lib PRIVATE NDEBUG)
target_link_libraries(simple_packing_lib pthread)

