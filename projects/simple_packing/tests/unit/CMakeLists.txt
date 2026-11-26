# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Unit tests of the simple packing components

sp_add_test(check_array_codec)
add_test(NAME ${SP_TEST_PREFIX}check_array_codec_16 COMMAND sp_check_array_codec double 10000 16)

# bit_packing.cc includes the generated reference data bit_packing_data.cc (gen_bitpacking_data.py)
sp_add_test(bit_packing)
foreach(bpv RANGE 3 64)
  add_test(NAME ${SP_TEST_PREFIX}bit_packing_${bpv} COMMAND sp_bit_packing ${bpv})
endforeach()


sp_add_test(u128)
add_test(NAME ${SP_TEST_PREFIX}u128 COMMAND sp_u128)

sp_add_test(u128_add_sub)
add_test(NAME ${SP_TEST_PREFIX}u128_add_sub COMMAND sp_u128_add_sub)

sp_add_test(power)
add_test(NAME ${SP_TEST_PREFIX}power COMMAND sp_power)

sp_add_test(bit_packing_element)
add_test(NAME ${SP_TEST_PREFIX}bit_packing_element COMMAND sp_bit_packing_element 10)

sp_add_test(codec_binary)
add_test(NAME ${SP_TEST_PREFIX}codec_binary COMMAND sp_codec_binary)

sp_add_test(float_encoding)
add_test(NAME ${SP_TEST_PREFIX}float_encoding COMMAND sp_float_encoding)

sp_add_test(utils_ranges)
add_test(NAME ${SP_TEST_PREFIX}utils_ranges COMMAND sp_utils_ranges)

sp_add_test(binary_exhaustive)
add_test(NAME ${SP_TEST_PREFIX}binary_exhaustive COMMAND sp_binary_exhaustive)

sp_add_test(optimizers)
add_test(NAME ${SP_TEST_PREFIX}optimizers COMMAND sp_optimizers)

sp_add_test(float_formats)
add_test(NAME ${SP_TEST_PREFIX}float_formats COMMAND sp_float_formats)

sp_add_test(errors)
add_test(NAME ${SP_TEST_PREFIX}errors COMMAND sp_errors)

sp_add_test(api)
add_test(NAME ${SP_TEST_PREFIX}api COMMAND sp_api)

sp_add_test(fp_environment)
add_test(NAME ${SP_TEST_PREFIX}fp_environment COMMAND sp_fp_environment)

sp_add_test(u128_reference)
add_test(NAME ${SP_TEST_PREFIX}u128_reference COMMAND sp_u128_reference)

sp_label_tests(unit)

# The unit tests check with assert(): compile them without NDEBUG (the test functions add it, and
# the Release flags contain -DNDEBUG), so that the assertions of the tests and of the header code
# they instantiate are active
get_property(unit_test_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(target IN LISTS unit_test_targets)
  get_target_property(definitions ${target} COMPILE_DEFINITIONS)
  if(definitions)
    list(REMOVE_ITEM definitions NDEBUG)
    set_target_properties(${target} PROPERTIES COMPILE_DEFINITIONS "${definitions}")
  endif()
  target_compile_options(${target} PRIVATE -UNDEBUG)
endforeach()
