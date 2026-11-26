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
# The same test as a user of spaec::spaec builds it: without the project flags (but for the same
# CPU), so that only the usage requirements of the target apply
add_executable(encode_fp_contraction_consumer ${CMAKE_CURRENT_SOURCE_DIR}/encode_fp_contraction.cc)
target_link_libraries(encode_fp_contraction_consumer PRIVATE spaec::spaec)
target_compile_options(encode_fp_contraction_consumer PRIVATE -O2 ${SPAEC_ARCH_FLAGS})
spaec_test_fp_flags(encode_fp_contraction_consumer ${SPAEC_FP_SAFETY_FLAGS})
add_test(NAME ${SPAEC_TEST_PREFIX}encode_fp_contraction_consumer COMMAND encode_fp_contraction_consumer)
spaec_add_test(encode_large_samples)
spaec_add_test(max_encoded_size)
spaec_add_test(encode_byte_order)

# A user who compiles with -ffast-math must add -fno-fast-math after it (SPAEC_NO_FAST_MATH=ON; README,
# "Floating-point compiler options"): copies of these tests compiled so check that this suffices
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-ffast-math SPAEC_HAVE_FAST_MATH)
if(SPAEC_HAVE_FAST_MATH)
  foreach(name parameter_validation encode_value_range)
    add_executable(${name}_fast_math ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc)
    target_link_libraries(${name}_fast_math PRIVATE spaec::spaec)
    target_compile_options(${name}_fast_math PRIVATE -ffast-math -fno-fast-math ${SPAEC_OPT_FLAGS})
    spaec_test_fp_flags(${name}_fast_math -ffast-math -fno-fast-math ${SPAEC_FP_FLAGS})
    target_compile_definitions(${name}_fast_math PRIVATE NDEBUG)
    target_link_options(${name}_fast_math PRIVATE -ffast-math -fno-fast-math)
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


# A user's code gets the floating-point options that this compiler needs (usage requirements):
# compiled without the project flags, for the same CPU
add_executable(consumer_flags ${CMAKE_CURRENT_SOURCE_DIR}/consumer_flags.cc)
target_link_libraries(consumer_flags PRIVATE spaec::spaec)
target_compile_options(consumer_flags PRIVATE -O2 ${SPAEC_ARCH_FLAGS})
spaec_test_fp_flags(consumer_flags ${SPAEC_FP_SAFETY_FLAGS})
add_test(NAME ${SPAEC_TEST_PREFIX}consumer_flags COMMAND consumer_flags)

spaec_label_tests(regression)
