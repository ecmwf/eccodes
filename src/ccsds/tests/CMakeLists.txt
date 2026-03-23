# Include directories for all ccsds tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/ccsds)

# Always compile CCSDS tests with -march=native -mtune=native -O3 (release mode)
add_compile_options(-march=native -mtune=native -O3)
add_compile_definitions(NDEBUG)

# Prefix for test names to avoid collisions when built as sub-project
set(CCSDS_TEST_PREFIX "ccsds_")

# --- Pure SpAec tests (no external dependencies) ---

add_executable(code_option_no_compression code_option_no_compression.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}code_option_no_compression COMMAND code_option_no_compression)

add_executable(code_option_second_extension code_option_second_extension.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}code_option_second_extension COMMAND code_option_second_extension)

add_executable(code_option_zero_block code_option_zero_block.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}code_option_zero_block COMMAND code_option_zero_block)

add_executable(code_option_split_sample code_option_split_sample.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}code_option_split_sample COMMAND code_option_split_sample)

add_executable(predictive_preprocessor predictive_preprocessor.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}predictive_preprocessor COMMAND predictive_preprocessor)

add_executable(bit_packing bit_packing.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}bit_packing COMMAND bit_packing)

add_executable(zb_encode_decode zb_encode_decode.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}zb_encode_decode COMMAND zb_encode_decode)

add_executable(se_encode_decode se_encode_decode.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}se_encode_decode COMMAND se_encode_decode)

add_executable(ccsds_spec_compliance ccsds_spec_compliance.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}ccsds_spec_compliance COMMAND ccsds_spec_compliance)

add_executable(encode_decode_sweep encode_decode_sweep.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}encode_decode_sweep COMMAND encode_decode_sweep)

add_executable(bitbuffer_test bitbuffer_test.cc)
add_test(NAME ${CCSDS_TEST_PREFIX}bitbuffer_test COMMAND bitbuffer_test)

# --- Tests requiring libaec (conditionally enabled) ---

if(TARGET libaec::aec)
  add_executable(libaec_cross_validation libaec_cross_validation.cc)
  target_link_libraries(libaec_cross_validation libaec::aec)
  add_test(NAME ${CCSDS_TEST_PREFIX}libaec_cross_validation COMMAND libaec_cross_validation)

  # Resolve paths relative to this sub-project
  set(CCSDS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)
  set(CCSDS_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data)

  add_executable(spaec_grib_cross_validation spaec_grib_cross_validation.cc ${CCSDS_SRC_DIR}/tools/Message.cc)
  target_include_directories(spaec_grib_cross_validation PRIVATE ${CCSDS_SRC_DIR}/tools)
  target_compile_definitions(spaec_grib_cross_validation PRIVATE DATA_DIR="${CCSDS_DATA_DIR}")
  target_link_libraries(spaec_grib_cross_validation libaec::aec)
  add_test(NAME ${CCSDS_TEST_PREFIX}spaec_grib_cross_validation COMMAND spaec_grib_cross_validation)

  add_executable(libaec_compat_test libaec_compat_test.cc libaec_real_wrapper.cc)
  target_link_libraries(libaec_compat_test ccsds_c dl)
  add_test(NAME ${CCSDS_TEST_PREFIX}libaec_compat_test COMMAND libaec_compat_test)
endif()
