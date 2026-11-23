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

sp_add_test(dec64_optimizer)

# The optimiser used to convert an out-of-range double to long (undefined behaviour). The
# test needs an UBSan-instrumented copy of the library to observe it (float-cast-overflow
# is a separate sanitizer group for GCC); without sanitizers it only checks that packing
# the minimised input does not crash and stays consistent.
set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined,float-cast-overflow")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined,float-cast-overflow")
check_cxx_source_compiles("int main() { return 0; }" SIMPLE_PACKING_HAVE_UBSAN)
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
  target_link_options(sp_optimizer_overflow PRIVATE -fsanitize=undefined,float-cast-overflow)
else()
  sp_add_test(optimizer_overflow)
endif()
add_test(NAME ${SP_TEST_PREFIX}optimizer_overflow COMMAND sp_optimizer_overflow)
add_test(NAME ${SP_TEST_PREFIX}dec64_optimizer COMMAND sp_dec64_optimizer)

# Misaligned memory accesses are detected by the alignment sanitizer
set(CMAKE_REQUIRED_FLAGS "-fsanitize=alignment")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=alignment")
check_cxx_source_compiles("int main() { return 0; }" SIMPLE_PACKING_HAVE_ALIGNMENT_SANITIZER)
unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LINK_OPTIONS)
if(SIMPLE_PACKING_HAVE_ALIGNMENT_SANITIZER)
  sp_add_test(unaligned_access)
  target_compile_options(sp_unaligned_access PRIVATE -fsanitize=alignment -fno-sanitize-recover=alignment)
  target_link_options(sp_unaligned_access PRIVATE -fsanitize=alignment)
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

sp_label_tests(regression)
