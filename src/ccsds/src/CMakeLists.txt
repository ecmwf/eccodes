# Header-only SpAec library (INTERFACE target)
add_library(ccsds_spaec INTERFACE)
target_include_directories(ccsds_spaec INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/ccsds
)
target_compile_features(ccsds_spaec INTERFACE cxx_std_20)

# Compiled C wrapper library (libaec-compatible API, for tests)
add_library(ccsds_c STATIC ccsds_c/libaec.cpp)
target_include_directories(ccsds_c PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/ccsds_c
  ${CMAKE_CURRENT_SOURCE_DIR}/ccsds
)
target_compile_features(ccsds_c PUBLIC cxx_std_20)
target_compile_options(ccsds_c PRIVATE -march=native -mtune=native -O3)
target_compile_definitions(ccsds_c PRIVATE NDEBUG)
