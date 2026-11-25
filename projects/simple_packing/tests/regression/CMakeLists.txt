# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Regression tests: bugs found by robustness testing (invalid parameters and values, buffer
# sizes, hangs, undefined behaviour, data races)

sp_add_test(reference_value_range)
add_test(NAME ${SP_TEST_PREFIX}reference_value_range COMMAND sp_reference_value_range)

sp_add_test(no_output)
add_test(NAME ${SP_TEST_PREFIX}no_output COMMAND sp_no_output)

sp_add_test(bits_per_value_range)
add_test(NAME ${SP_TEST_PREFIX}bits_per_value_range COMMAND sp_bits_per_value_range)

sp_add_test(zero_values)
add_test(NAME ${SP_TEST_PREFIX}zero_values COMMAND sp_zero_values)

sp_add_test(unpack_buffer_size)
add_test(NAME ${SP_TEST_PREFIX}unpack_buffer_size COMMAND sp_unpack_buffer_size)

sp_add_test(field_range)
add_test(NAME ${SP_TEST_PREFIX}field_range COMMAND sp_field_range)

sp_add_test(optimizer_parameters)
add_test(NAME ${SP_TEST_PREFIX}optimizer_parameters COMMAND sp_optimizer_parameters)

sp_add_test(scale_factor_range)
add_test(NAME ${SP_TEST_PREFIX}scale_factor_range COMMAND sp_scale_factor_range)

sp_add_test(pack_value_range)
add_test(NAME ${SP_TEST_PREFIX}pack_value_range COMMAND sp_pack_value_range)

sp_add_test(no_optimizer)
add_test(NAME ${SP_TEST_PREFIX}no_optimizer COMMAND sp_no_optimizer)

sp_add_test(optimizer_direct_range)
add_test(NAME ${SP_TEST_PREFIX}optimizer_direct_range COMMAND sp_optimizer_direct_range)

sp_add_test(optimizer_scaling_overflow)
add_test(NAME ${SP_TEST_PREFIX}optimizer_scaling_overflow COMMAND sp_optimizer_scaling_overflow)

sp_add_test(binary_scale_tiny_range)
add_test(NAME ${SP_TEST_PREFIX}binary_scale_tiny_range COMMAND sp_binary_scale_tiny_range)


# The optimiser used to convert an out-of-range double to long (undefined behaviour). The
# test needs an UBSan-instrumented copy of the library to observe it (float-cast-overflow
# is a separate sanitizer group for GCC); without sanitizers it only checks that packing
# the minimised input does not crash and stays consistent.
# Probes: optimised, instrumented code and the flags of the tests (some toolchains, e.g. zig,
# provide the sanitizer runtime only for unoptimised code or lack the non-recovering handlers)
set(CMAKE_REQUIRED_FLAGS "-O2 -fsanitize=undefined,float-cast-overflow -fno-sanitize-recover=undefined,float-cast-overflow")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined,float-cast-overflow")
check_cxx_source_compiles("int main(int argc, char** argv) { return *reinterpret_cast<const int*>(argv[0] + argc); }" SIMPLE_PACKING_HAVE_UBSAN)
unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LINK_OPTIONS)
if(SIMPLE_PACKING_HAVE_UBSAN)
  add_library(simple_packing_ubsan STATIC EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/src/Utils.cc
    ${PROJECT_SOURCE_DIR}/src/optimizer/BinDecOptimizer.cc)
  target_link_libraries(simple_packing_ubsan PUBLIC simple_packing_headers)
  target_compile_options(simple_packing_ubsan PRIVATE -fsanitize=undefined,float-cast-overflow -fno-sanitize-recover=undefined,float-cast-overflow)
  add_executable(sp_optimizer_overflow ${CMAKE_CURRENT_SOURCE_DIR}/optimizer_overflow.cc)
  target_link_libraries(sp_optimizer_overflow PRIVATE simple_packing_ubsan)
  target_compile_options(sp_optimizer_overflow PRIVATE -fsanitize=undefined,float-cast-overflow -fno-sanitize-recover=undefined,float-cast-overflow)
  target_link_options(sp_optimizer_overflow PRIVATE -fsanitize=undefined,float-cast-overflow -fno-sanitize-recover=undefined,float-cast-overflow)
else()
  sp_add_test(optimizer_overflow)
endif()
add_test(NAME ${SP_TEST_PREFIX}optimizer_overflow COMMAND sp_optimizer_overflow)

# Misaligned memory accesses are detected by the alignment sanitizer
set(CMAKE_REQUIRED_FLAGS "-O2 -fsanitize=alignment -fno-sanitize-recover=alignment")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=alignment")
check_cxx_source_compiles("int main(int argc, char** argv) { return *reinterpret_cast<const int*>(argv[0] + argc); }" SIMPLE_PACKING_HAVE_ALIGNMENT_SANITIZER)
unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LINK_OPTIONS)
if(SIMPLE_PACKING_HAVE_ALIGNMENT_SANITIZER)
  sp_add_test(unaligned_access)
  target_compile_options(sp_unaligned_access PRIVATE -fsanitize=alignment -fno-sanitize-recover=alignment)
  target_link_options(sp_unaligned_access PRIVATE -fsanitize=alignment -fno-sanitize-recover=alignment)
  add_test(NAME ${SP_TEST_PREFIX}unaligned_access COMMAND sp_unaligned_access)
endif()

sp_add_test(u128_shift)
add_test(NAME ${SP_TEST_PREFIX}u128_shift COMMAND sp_u128_shift)

# Data races are detected by the thread sanitizer (Utils.cc is compiled into the test with it)
set(CMAKE_REQUIRED_FLAGS "-fsanitize=thread")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=thread")
check_cxx_source_compiles("int main() { return 0; }" SIMPLE_PACKING_HAVE_THREAD_SANITIZER)
unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LINK_OPTIONS)
if(SIMPLE_PACKING_HAVE_THREAD_SANITIZER)
  add_executable(sp_table_init_race ${CMAKE_CURRENT_SOURCE_DIR}/table_init_race.cc ${PROJECT_SOURCE_DIR}/src/Utils.cc)
  target_link_libraries(sp_table_init_race PRIVATE simple_packing::headers Threads::Threads)
  target_compile_options(sp_table_init_race PRIVATE -fsanitize=thread -g)
  target_link_options(sp_table_init_race PRIVATE -fsanitize=thread)
  add_test(NAME ${SP_TEST_PREFIX}table_init_race COMMAND sp_table_init_race)
  set_tests_properties(${SP_TEST_PREFIX}table_init_race PROPERTIES ENVIRONMENT "TSAN_OPTIONS=halt_on_error=1:exitcode=66")
endif()

sp_add_test(unpack_float_precision)
add_test(NAME ${SP_TEST_PREFIX}unpack_float_precision COMMAND sp_unpack_float_precision)

sp_add_test(pack_fp_contraction)
add_test(NAME ${SP_TEST_PREFIX}pack_fp_contraction COMMAND sp_pack_fp_contraction)
# The same test as a user of simple_packing::simple_packing builds it: without the project flags,
# for the host CPU (fused multiply-add available), so that only the usage requirements apply
check_cxx_compiler_flag(-march=native SIMPLE_PACKING_HAVE_MARCH_NATIVE)
add_executable(sp_pack_fp_contraction_consumer ${CMAKE_CURRENT_SOURCE_DIR}/pack_fp_contraction.cc)
target_link_libraries(sp_pack_fp_contraction_consumer PRIVATE simple_packing::simple_packing)
target_compile_options(sp_pack_fp_contraction_consumer PRIVATE -O2 $<$<AND:$<BOOL:${SIMPLE_PACKING_HAVE_MARCH_NATIVE}>,$<NOT:$<BOOL:${CMAKE_CROSSCOMPILING}>>>:-march=native>)
add_test(NAME ${SP_TEST_PREFIX}pack_fp_contraction_consumer COMMAND sp_pack_fp_contraction_consumer)

sp_add_test(binary_scale_large_range)
add_test(NAME ${SP_TEST_PREFIX}binary_scale_large_range COMMAND sp_binary_scale_large_range)

sp_add_test(bit_packing_reference)
add_test(NAME ${SP_TEST_PREFIX}bit_packing_reference COMMAND sp_bit_packing_reference)

# The validation of the values and parameters must also hold when the user compiles with
# -ffast-math (CMAKE_CXX_FLAGS come before the project flags): copies of these tests that compile
# the library sources with -ffast-math first. Not the tests of denormal values: linking with
# -ffast-math makes the CPU flush them to zero, which a library cannot prevent.
check_cxx_compiler_flag(-ffast-math SIMPLE_PACKING_HAVE_FAST_MATH)
if(SIMPLE_PACKING_HAVE_FAST_MATH)
  foreach(name field_range optimizer_direct_range reference_value_range optimizer_scaling_overflow pack_value_range)
    add_executable(sp_${name}_fast_math ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc
                   ${PROJECT_SOURCE_DIR}/src/Utils.cc ${PROJECT_SOURCE_DIR}/src/optimizer/BinDecOptimizer.cc)
    target_link_libraries(sp_${name}_fast_math PRIVATE simple_packing::headers)
    target_compile_options(sp_${name}_fast_math PRIVATE -ffast-math ${SIMPLE_PACKING_OPT_FLAGS})
    target_compile_definitions(sp_${name}_fast_math PRIVATE NDEBUG)
    target_link_options(sp_${name}_fast_math PRIVATE -ffast-math)
    add_test(NAME ${SP_TEST_PREFIX}${name}_fast_math COMMAND sp_${name}_fast_math)
    set_tests_properties(${SP_TEST_PREFIX}${name}_fast_math PROPERTIES TIMEOUT 120)
  endforeach()
endif()

sp_label_tests(regression)
