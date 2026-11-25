# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Regression tests: bugs found by robustness testing (malformed input, parameters, buffers,
# threads)

spaec_add_test(decode_zero_block_run)
spaec_add_test(decode_truncated)
spaec_add_test(decode_prefix)
spaec_add_test(parameter_validation)
spaec_add_test(encode_target_buffer)
spaec_add_test(encode_value_range)
spaec_add_test(encode_fp_contraction)
# The same test as a user of spaec::spaec builds it: without the project flags, for the host CPU
# (fused multiply-add available), so that only the usage requirements of the target apply
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-march=native SPAEC_HAVE_MARCH_NATIVE)
add_executable(encode_fp_contraction_consumer ${CMAKE_CURRENT_SOURCE_DIR}/encode_fp_contraction.cc)
target_link_libraries(encode_fp_contraction_consumer PRIVATE spaec::spaec)
target_compile_options(encode_fp_contraction_consumer PRIVATE -O2 $<$<AND:$<BOOL:${SPAEC_HAVE_MARCH_NATIVE}>,$<NOT:$<BOOL:${CMAKE_CROSSCOMPILING}>>>:-march=native>)
add_test(NAME ${SPAEC_TEST_PREFIX}encode_fp_contraction_consumer COMMAND encode_fp_contraction_consumer)
spaec_add_test(encode_large_samples)
spaec_add_test(max_encoded_size)
spaec_add_test(encode_byte_order)

# The validation must also hold when the user compiles with -ffast-math (CMAKE_CXX_FLAGS come
# before the project flags): copies of these tests compiled with -ffast-math first
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-ffast-math SPAEC_HAVE_FAST_MATH)
if(SPAEC_HAVE_FAST_MATH)
  foreach(name parameter_validation encode_value_range)
    add_executable(${name}_fast_math ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc)
    target_link_libraries(${name}_fast_math PRIVATE spaec::spaec)
    target_compile_options(${name}_fast_math PRIVATE -ffast-math ${SPAEC_OPT_FLAGS})
    target_compile_definitions(${name}_fast_math PRIVATE NDEBUG)
    target_link_options(${name}_fast_math PRIVATE -ffast-math)
    add_test(NAME ${SPAEC_TEST_PREFIX}${name}_fast_math COMMAND ${name}_fast_math)
  endforeach()
endif()
spaec_add_test(encode_concurrent LIBS Threads::Threads)
spaec_add_test(decode_parallel LIBS Threads::Threads)
spaec_add_test(decode_thread_pool LIBS Threads::Threads)
spaec_add_test(encode_parallel LIBS Threads::Threads)
spaec_add_test(decode_float_precision LIBS Threads::Threads)

# Out of range access must be detected in debug builds: compiled with assertions
add_executable(span_bounds ${CMAKE_CURRENT_SOURCE_DIR}/span_bounds.cc)
target_link_libraries(span_bounds PRIVATE spaec::spaec)
target_compile_options(span_bounds PRIVATE -UNDEBUG)
add_test(NAME ${SPAEC_TEST_PREFIX}span_bounds COMMAND span_bounds)


# A user's code compiled with -ffast-math keeps the checks of the library (usage requirements)
add_executable(consumer_flags ${CMAKE_CURRENT_SOURCE_DIR}/consumer_flags.cc)
target_link_libraries(consumer_flags PRIVATE spaec::spaec)
target_compile_options(consumer_flags PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-ffast-math>)
add_test(NAME ${SPAEC_TEST_PREFIX}consumer_flags COMMAND consumer_flags)

spaec_label_tests(regression)
