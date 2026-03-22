add_executable(code_option_no_compression code_option_no_compression.cc )
add_test(NAME code_option_no_compression COMMAND code_option_no_compression)

add_executable(code_option_second_extension code_option_second_extension.cc )
add_test(NAME code_option_second_extension COMMAND code_option_second_extension)

add_executable(code_option_zero_block code_option_zero_block.cc )
add_test(NAME code_option_zero_block COMMAND code_option_zero_block)

add_executable(code_option_split_sample code_option_split_sample.cc )
add_test(NAME code_option_split_sample COMMAND code_option_split_sample)

add_executable(predictive_preprocessor predictive_preprocessor.cc )
add_test(NAME predictive_preprocessor COMMAND predictive_preprocessor)

add_executable(bit_packing bit_packing.cc )
add_test(NAME bit_packing COMMAND bit_packing)

add_executable(zb_encode_decode zb_encode_decode.cc )
add_test(NAME zb_encode_decode COMMAND zb_encode_decode)

add_executable(se_encode_decode se_encode_decode.cc )
add_test(NAME se_encode_decode COMMAND se_encode_decode)

#add_executable(se_encode_decode_parallel se_encode_decode_parallel.cc )
#add_test(NAME se_encode_decode_parallel COMMAND se_encode_decode_parallel)

add_executable(ccsds_spec_compliance ccsds_spec_compliance.cc )
add_test(NAME ccsds_spec_compliance COMMAND ccsds_spec_compliance)

add_executable(encode_decode_sweep encode_decode_sweep.cc )
add_test(NAME encode_decode_sweep COMMAND encode_decode_sweep)

add_executable(bitbuffer_test bitbuffer_test.cc )
add_test(NAME bitbuffer_test COMMAND bitbuffer_test)

# add_executable(ss_encode_decode ss_encode_decode.cc )
# add_test(NAME ss_encode_decode COMMAND ss_encode_decode)

# add_executable(ss_encode_decode_parallel ss_encode_decode_parallel.cc )
# add_test(NAME ss_encode_decode_parallel COMMAND ss_encode_decode_parallel)

# add_executable(nc_encode_decode_parallel nc_encode_decode_parallel.cc )
# add_test(NAME nc_encode_decode_parallel COMMAND nc_encode_decode_parallel)

# )

