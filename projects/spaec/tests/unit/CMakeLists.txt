# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Unit tests of the SpAec components

spaec_add_test(code_option_no_compression)
spaec_add_test(code_option_second_extension)
spaec_add_test(code_option_zero_block)
spaec_add_test(code_option_split_sample)
spaec_add_test(predictive_preprocessor)
spaec_add_test(bit_packing)
spaec_add_test(zb_encode_decode)
spaec_add_test(se_encode_decode)
spaec_add_test(ccsds_spec_compliance)
spaec_add_test(encode_decode_sweep)
spaec_add_test(bitbuffer_test)
spaec_add_test(bitbuffer_bounds)
spaec_add_test(zero_block_decode)
spaec_add_test(encoder_options)
spaec_add_test(value_mapping)
spaec_add_test(decoder_par LIBS Threads::Threads)
spaec_add_test(parameters)
spaec_add_test(spaec_api LIBS Threads::Threads)
spaec_add_test(encoder_exhaustive LIBS Threads::Threads)
spaec_add_test(span)
spaec_add_test(errors LIBS Threads::Threads)
spaec_add_test(spec_conformance LIBS Threads::Threads)

spaec_label_tests(unit)

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
