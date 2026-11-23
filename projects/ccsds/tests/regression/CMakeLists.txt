# Regression tests: bugs found by robustness testing (malformed input, parameters, buffers,
# threads)

ccsds_add_test(decode_zero_block_run)
ccsds_add_test(decode_truncated)
ccsds_add_test(decode_prefix)
ccsds_add_test(parameter_validation)
ccsds_add_test(encode_target_buffer)
ccsds_add_test(encode_value_range)
ccsds_add_test(encode_concurrent LIBS Threads::Threads)
ccsds_add_test(decode_parallel LIBS Threads::Threads)
ccsds_add_test(encode_parallel LIBS Threads::Threads)
ccsds_add_test(stats_empty)

# Out of range access must be detected in debug builds: compiled with assertions
add_executable(span_bounds ${CMAKE_CURRENT_SOURCE_DIR}/span_bounds.cc)
target_link_libraries(span_bounds PRIVATE ccsds::spaec)
target_compile_options(span_bounds PRIVATE -UNDEBUG)
add_test(NAME ${CCSDS_TEST_PREFIX}span_bounds COMMAND span_bounds)

ccsds_label_tests(regression)
