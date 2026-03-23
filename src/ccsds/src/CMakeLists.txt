# Header-only SpAec library (INTERFACE target)
add_library(ccsds_spaec INTERFACE)
target_include_directories(ccsds_spaec INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/ccsds
)
target_compile_features(ccsds_spaec INTERFACE cxx_std_20)

