# Test names are prefixed to avoid collisions when built as part of ecCodes
set(CCSDS_TEST_PREFIX "ccsds_")

# ccsds_add_test(<name> [LIBS <libs...>])
# Builds <name>.cc into an executable and registers it as test ccsds_<name>.
# Always compiled with -march=native -mtune=native -O3 (release mode).
function(ccsds_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
  add_executable(${name} ${name}.cc ${ARG_SOURCES})
  target_link_libraries(${name} PRIVATE ccsds::spaec ${ARG_LIBS})
  target_compile_options(${name} PRIVATE -march=native -mtune=native -O3)
  target_compile_definitions(${name} PRIVATE NDEBUG)
  add_test(NAME ${CCSDS_TEST_PREFIX}${name} COMMAND ${name})
endfunction()

# --- Pure SpAec tests (no external dependencies) ---

ccsds_add_test(code_option_no_compression)
ccsds_add_test(code_option_second_extension)
ccsds_add_test(code_option_zero_block)
ccsds_add_test(code_option_split_sample)
ccsds_add_test(predictive_preprocessor)
ccsds_add_test(bit_packing)
ccsds_add_test(zb_encode_decode)
ccsds_add_test(se_encode_decode)
ccsds_add_test(ccsds_spec_compliance)
ccsds_add_test(encode_decode_sweep)
ccsds_add_test(bitbuffer_test)
ccsds_add_test(decode_zero_block_run)
ccsds_add_test(decode_truncated)
ccsds_add_test(parameter_validation)

