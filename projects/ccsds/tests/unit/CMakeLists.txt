# Unit tests of the SpAec components

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
ccsds_add_test(bitbuffer_bounds)
ccsds_add_test(zero_block_decode)
ccsds_add_test(encoder_options)
ccsds_add_test(value_mapping)
ccsds_add_test(decoder_par LIBS Threads::Threads)
ccsds_add_test(parameters_stats)
ccsds_add_test(spaec_api LIBS Threads::Threads)
ccsds_add_test(encoder_exhaustive LIBS Threads::Threads)
ccsds_add_test(span_stats)
ccsds_add_test(errors LIBS Threads::Threads)
ccsds_add_test(spec_conformance LIBS Threads::Threads)

ccsds_label_tests(unit)

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
