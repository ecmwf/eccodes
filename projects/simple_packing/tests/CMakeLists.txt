# Test names are prefixed to avoid collisions when built as part of ecCodes
set(SP_TEST_PREFIX "simple_packing_")

# sp_add_test(<name> [SOURCES <sources...>])
# Builds <name>.cc into the executable sp_<name>, compiled like the library (optimised).
function(sp_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})
  add_executable(sp_${name} ${name}.cc ${ARG_SOURCES})
  target_link_libraries(sp_${name} PRIVATE simple_packing::simple_packing)
  target_compile_options(sp_${name} PRIVATE ${SIMPLE_PACKING_OPT_FLAGS})
  target_compile_definitions(sp_${name} PRIVATE NDEBUG)
endfunction()

sp_add_test(check_array_codec)
add_test(NAME ${SP_TEST_PREFIX}check_array_codec_16 COMMAND sp_check_array_codec double 10000 16)

# bit_packing.cc includes the generated reference data bit_packing_data.cc (gen_bitpacking_data.py)
sp_add_test(bit_packing)
foreach(bpv RANGE 3 64)
  add_test(NAME ${SP_TEST_PREFIX}bit_packing_${bpv} COMMAND sp_bit_packing ${bpv})
endforeach()

sp_add_test(dec_params)
add_test(NAME ${SP_TEST_PREFIX}dec_params COMMAND sp_dec_params)

sp_add_test(u128)
add_test(NAME ${SP_TEST_PREFIX}u128 COMMAND sp_u128)

sp_add_test(u128_add_sub)
add_test(NAME ${SP_TEST_PREFIX}u128_add_sub COMMAND sp_u128_add_sub)

sp_add_test(power)
add_test(NAME ${SP_TEST_PREFIX}power COMMAND sp_power)

sp_add_test(bit_packing_element)
add_test(NAME ${SP_TEST_PREFIX}bit_packing_element COMMAND sp_bit_packing_element 10)

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

# Misaligned memory accesses are detected by the alignment sanitizer
include(CheckCXXSourceCompiles)
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
