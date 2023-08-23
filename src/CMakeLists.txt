#
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
list( APPEND eccodes_src_files
    grib_api.h
    eccodes.h
    action.cc
    action_class_alias.cc
    action_class_gen.cc
    action_class_if.cc
    action_class_switch.cc
    grib_accessor_class_g1fcperiod.cc
    grib_accessor_class_g1end_of_interval_monthly.cc
    grib_accessor_class_mars_param.cc
    action_class_section.cc
    action_class_list.cc
    action_class_while.cc
    action_class_put.cc
    action_class_meta.cc
    action_class_remove.cc
    action_class_rename.cc
    action_class_assert.cc
    action_class_template.cc
    action_class_trigger.cc
    action_class_when.cc
    action_class_concept.cc
    action_class_hash_array.cc
    action_class_set.cc
    action_class_set_sarray.cc
    action_class_set_darray.cc
    action_class_noop.cc
    action_class_write.cc
    action_class_print.cc
    action_class_close.cc
    action_class_variable.cc
    action_class_modify.cc
    action_class_transient_darray.cc
    eccodes.cc
    grib_accessor.cc
    grib_concept.cc
    grib_hash_array.cc
    grib_bufr_descriptor.cc
    grib_bufr_descriptors_array.cc
    grib_darray.cc
    grib_sarray.cc
    grib_oarray.cc
    grib_vdarray.cc
    grib_vsarray.cc
    grib_iarray.cc
    grib_viarray.cc
    codes_memfs.cc
    grib_accessor_class_ascii.cc
    grib_accessor_class_bit.cc
    grib_accessor_class_bitmap.cc
    grib_accessor_class_bits.cc
    grib_accessor_class_bits_per_value.cc
    grib_accessor_class_bufr_elements_table.cc
    grib_accessor_class_bufr_data_array.cc
    grib_accessor_class_bufr_data_element.cc
    grib_accessor_class_bufr_group.cc
    grib_accessor_class_bufr_string_values.cc
    grib_accessor_class_pack_bufr_values.cc
    grib_accessor_class_unpack_bufr_values.cc
    grib_accessor_class_group.cc
    grib_accessor_class_non_alpha.cc
    grib_accessor_class_g1bitmap.cc
    grib_accessor_class_g2bitmap.cc
    grib_accessor_class_concept.cc
    grib_accessor_class_hash_array.cc
    grib_accessor_class_decimal_precision.cc
    grib_accessor_class_divdouble.cc
    grib_accessor_class_budgdate.cc
    grib_accessor_class_validity_date.cc
    grib_accessor_class_validity_time.cc
    grib_accessor_class_bytes.cc
    grib_accessor_class.cc
    grib_accessor_class_change_alternative_row_scanning.cc
    grib_accessor_class_change_scanning_direction.cc
    grib_accessor_class_check_internal_version.cc
    grib_accessor_class_codeflag.cc
    grib_accessor_class_smart_table.cc
    grib_accessor_class_smart_table_column.cc
    grib_accessor_class_codetable.cc
    grib_accessor_class_codetable_units.cc
    grib_accessor_class_codetable_title.cc
    grib_accessor_class_step_human_readable.cc
    grib_accessor_class_count_file.cc
    grib_accessor_class_count_total.cc
    grib_accessor_class_double.cc
    grib_accessor_class_element.cc
    grib_accessor_class_evaluate.cc
    grib_accessor_class_g1date.cc
    grib_accessor_class_g1monthlydate.cc
    grib_accessor_class_library_version.cc
    grib_accessor_class_when.cc
    grib_accessor_class_g1verificationdate.cc
    grib_accessor_class_g1day_of_the_year_date.cc
    grib_accessor_class_g1_half_byte_codeflag.cc
    grib_accessor_class_g1forecastmonth.cc
    grib_accessor_class_g1step_range.cc
    grib_accessor_class_g2step_range.cc
    grib_accessor_class_data_g22order_packing.cc
    grib_accessor_class_mars_step.cc
    grib_accessor_class_message_copy.cc
    grib_accessor_class_dictionary.cc
    grib_accessor_class_latlon_increment.cc
    grib_accessor_class_g2date.cc
    grib_accessor_class_g2level.cc
    grib_accessor_class_g2end_step.cc
    grib_accessor_class_g2latlon.cc
    grib_accessor_class_g2lon.cc
    grib_accessor_class_global_gaussian.cc
    grib_accessor_class_gaussian_grid_name.cc
    grib_accessor_class_gen.cc
    grib_accessor_class_getenv.cc
    grib_accessor_class_gts_header.cc
    grib_accessor_class_ifs_param.cc
    grib_accessor_class_julian_day.cc
    grib_accessor_class_julian_date.cc
    grib_accessor_class_closest_date.cc
    grib_accessor_class_latlonvalues.cc
    grib_accessor_class_latitudes.cc
    grib_accessor_class_longitudes.cc
    grib_accessor_class_offset_file.cc
    grib_accessor_class_scale.cc
    grib_accessor_class_from_scale_factor_scaled_value.cc
    grib_accessor_class_g2bitmap_present.cc
    grib_accessor_class_ibmfloat.cc
    grib_accessor_class_ieeefloat.cc
    grib_accessor_class_constant.cc
    grib_accessor_class_iterator.cc
    grib_accessor_class_message.cc
    grib_accessor_class_nearest.cc
    grib_accessor_class_ksec1expver.cc
    grib_accessor_class_label.cc
    grib_accessor_class_long.cc
    grib_accessor_class_lookup.cc
    grib_accessor_class_octahedral_gaussian.cc
    grib_accessor_class_octet_number.cc
    grib_accessor_class_headers_only.cc
    grib_accessor_class_padding.cc
    grib_accessor_class_pad.cc
    grib_accessor_class_padto.cc
    grib_accessor_class_padtoeven.cc
    grib_accessor_class_padtomultiple.cc
    grib_accessor_class_section_padding.cc
    grib_accessor_class_packing_type.cc
    grib_accessor_class_section_pointer.cc
    grib_accessor_class_position.cc
    grib_accessor_class_signed.cc
    grib_accessor_class_signed_bits.cc
    grib_accessor_class_section.cc
    grib_accessor_class_step_in_units.cc
    grib_accessor_class_section_length.cc
    grib_accessor_class_g1_message_length.cc
    grib_accessor_class_g1_section4_length.cc
    grib_accessor_class_size.cc
    grib_accessor_class_rdbtime_guess_date.cc
    grib_accessor_class_scale_values.cc
    grib_accessor_class_offset_values.cc
    grib_accessor_class_sprintf.cc
    grib_accessor_class_round.cc
    grib_accessor_class_spectral_truncation.cc
    grib_accessor_class_time.cc
    grib_accessor_class_transient.cc
    grib_accessor_class_trim.cc
    grib_accessor_class_transient_darray.cc
    grib_accessor_class_values.cc
    grib_accessor_class_simple_packing_error.cc
    grib_accessor_class_data_simple_packing.cc
    grib_accessor_class_count_missing.cc
    grib_accessor_class_data_sh_packed.cc
    grib_accessor_class_data_sh_unpacked.cc
    grib_accessor_class_number_of_values_data_raw_packing.cc
    grib_accessor_class_data_g1simple_packing.cc
    grib_accessor_class_data_g1shsimple_packing.cc
    grib_accessor_class_data_shsimple_packing.cc
    grib_accessor_class_data_dummy_field.cc
    grib_accessor_class_variable.cc
    grib_accessor_class_second_order_bits_per_value.cc
    grib_accessor_class_data_g2simple_packing.cc
    grib_accessor_class_data_g2simple_packing_with_preprocessing.cc
    grib_accessor_class_data_g2shsimple_packing.cc
    grib_accessor_class_data_g2complex_packing.cc
    grib_accessor_class_data_g1second_order_row_by_row_packing.cc
    grib_accessor_class_data_g1second_order_constant_width_packing.cc
    grib_accessor_class_data_g1second_order_general_packing.cc
    grib_accessor_class_data_g1second_order_general_extended_packing.cc
    grib_accessor_class_g2grid.cc
    grib_accessor_class_unexpanded_descriptors.cc
    grib_accessor_class_expanded_descriptors.cc
    grib_accessor_class_bufrdc_expanded_descriptors.cc
    grib_accessor_class_data_apply_bitmap.cc
    grib_accessor_class_data_apply_boustrophedonic.cc
    grib_accessor_class_data_apply_boustrophedonic_bitmap.cc
    grib_accessor_class_data_secondary_bitmap.cc
    grib_accessor_class_data_g1secondary_bitmap.cc
    grib_accessor_class_data_g2secondary_bitmap.cc
    grib_accessor_class_data_jpeg2000_packing.cc
    grib_accessor_class_data_png_packing.cc
    grib_accessor_class_data_ccsds_packing.cc
    grib_accessor_class_data_raw_packing.cc
    grib_accessor_class_data_complex_packing.cc
    grib_accessor_class_data_g1complex_packing.cc
    grib_accessor_class_gds_not_present_bitmap.cc
    grib_accessor_class_data_apply_gdsnotpresent.cc
    grib_accessor_class_gds_is_present.cc
    grib_accessor_class_select_step_template.cc
    grib_accessor_class_local_definition.cc
    grib_accessor_class_g2_eps.cc
    grib_accessor_class_g2_aerosol.cc
    grib_accessor_class_g2_chemical.cc
    grib_accessor_class_g2_mars_labeling.cc
    grib_accessor_class_md5.cc
    grib_accessor_class_proj_string.cc
    grib_jasper_encoding.cc
    grib_openjpeg_encoding.cc
    action_class_set_missing.cc
    grib_accessor_class_number_of_points.cc
    grib_accessor_class_suppressed.cc
    grib_index.cc
    grib_accessor_class_number_of_points_gaussian.cc
    grib_accessor_class_number_of_values.cc
    grib_accessor_class_number_of_coded_values.cc
    grib_accessor_class_g1number_of_coded_values_sh_complex.cc
    grib_accessor_class_g1number_of_coded_values_sh_simple.cc
    grib_accessor_class_dirty.cc
    grib_accessor_class_statistics.cc
    grib_accessor_class_statistics_spectral.cc
    grib_accessor_class_unsigned.cc
    grib_accessor_class_unsigned_bits.cc
    grib_accessor_class_raw.cc
    grib_accessor_class_bufr_simple_thinning.cc
    grib_accessor_class_bufr_extract_subsets.cc
    grib_accessor_class_bufr_extract_area_subsets.cc
    grib_accessor_class_bufr_extract_datetime_subsets.cc
    grib_accessor_class_spd.cc
    grib_accessor_class_sum.cc
    grib_accessor_class_to_integer.cc
    grib_accessor_class_to_double.cc
    grib_accessor_class_to_string.cc
    grib_accessor_class_sexagesimal2decimal.cc
    grib_accessor_class_vector.cc
    grib_accessor_class_long_vector.cc
    grib_gaussian_reduced.cc
    grib_accessor_class_abstract_vector.cc
    grib_accessor_class_abstract_long_vector.cc
    grib_loader_from_handle.cc
    grib_bits.cc
    grib_timer.cc
    grib_ibmfloat.cc
    grib_ieeefloat.cc
    grib_accessor_class_reference_value_error.cc
    grib_memory.cc
    grib_buffer.cc
    grib_dumper.cc
    grib_dumper_class_serialize.cc
    grib_dumper_class_debug.cc
    grib_dumper_class_default.cc
    grib_dumper_class_bufr_encode_C.cc
    grib_dumper_class_bufr_encode_filter.cc
    grib_dumper_class_bufr_encode_fortran.cc
    grib_dumper_class_bufr_encode_python.cc
    grib_dumper_class_bufr_decode_C.cc
    grib_dumper_class_bufr_decode_filter.cc
    grib_dumper_class_bufr_decode_fortran.cc
    grib_dumper_class_bufr_decode_python.cc
    grib_dumper_class_bufr_simple.cc
    grib_dumper_class_json.cc
    grib_dumper_class_grib_encode_C.cc
    grib_dumper_class_wmo.cc
    grib_dumper_class.cc
    grib_context.cc
    grib_date.cc
    grib_fieldset.cc
    grib_filepool.cc
    grib_geography.cc
    grib_handle.cc
    grib_hash_keys.cc
    grib_io.cc
    grib_trie.cc
    grib_trie_with_rank.cc
    grib_itrie.cc
    grib_rules.cc
    grib_keys_iterator.cc
    bufr_keys_iterator.cc
    grib_parse_utils.cc
    grib_query.cc
    grib_scaling.cc
    grib_templates.cc
    grib_dependency.cc
    grib_value.cc
    grib_errors.cc
    grib_expression_class_binop.cc
    grib_expression_class_logical_and.cc
    grib_expression_class_logical_or.cc
    grib_expression_class_is_in_dict.cc
    grib_expression_class_true.cc
    grib_expression_class_string_compare.cc
    grib_expression_class_unop.cc
    grib_expression_class_functor.cc
    grib_expression_class_accessor.cc
    grib_expression_class_is_in_list.cc
    grib_expression_class_is_integer.cc
    grib_expression_class_length.cc
    grib_expression_class_long.cc
    grib_expression_class_double.cc
    grib_expression_class_string.cc
    grib_expression_class_sub_string.cc
    grib_nearest.cc
    grib_nearest_class.cc
    grib_nearest_class_gen.cc
    grib_nearest_class_regular.cc
    grib_nearest_class_reduced.cc
    grib_nearest_class_latlon_reduced.cc
    grib_nearest_class_lambert_conformal.cc
    grib_nearest_class_lambert_azimuthal_equal_area.cc
    grib_nearest_class_mercator.cc
    grib_nearest_class_polar_stereographic.cc
    grib_nearest_class_space_view.cc
    grib_iterator_class_polar_stereographic.cc
    grib_iterator_class_lambert_azimuthal_equal_area.cc
    grib_iterator_class_lambert_conformal.cc
    grib_iterator_class_mercator.cc
    grib_iterator.cc
    grib_iterator_class.cc
    grib_iterator_class_gaussian.cc
    grib_iterator_class_gaussian_reduced.cc
    grib_iterator_class_latlon_reduced.cc
    grib_iterator_class_gen.cc
    grib_iterator_class_latlon.cc
    grib_iterator_class_regular.cc
    grib_iterator_class_space_view.cc
    grib_expression.cc
    codes_util.cc
    grib_util.cc
    bufr_util.cc
    string_util.cc
    functions.cc
    grib_accessor_class.h
    grib_accessor_factory.h
    grib_api_internal.h
    eccodes_prototypes.h
    grib_dumper_class.h
    grib_dumper_factory.h
    grib_emoslib.h
    grib_iterator_class.h
    grib_iterator_factory.h
    grib_nearest_class.h
    grib_nearest_factory.h
    grib_yacc.h
    md5.h
    md5.cc
    grib_accessor_class_int16_little_endian.cc
    grib_accessor_class_int64_little_endian.cc
    grib_accessor_class_uint16.cc
    grib_accessor_class_uint16_little_endian.cc
    grib_accessor_class_uint32.cc
    grib_accessor_class_uint32_little_endian.cc
    grib_accessor_class_uint64.cc
    grib_accessor_class_uint64_little_endian.cc
    grib_accessor_class_uint8.cc
    grib_accessor_class_blob.cc
    grib_optimize_decimal_factor.cc
    grib_accessor_class_data_g2bifourier_packing.cc
    grib_accessor_class_data_run_length_packing.cc
    eccodes_windef.h
    # We ship our own generated lex/yacc C files
    grib_yacc.cc  grib_lex.cc )

list( APPEND eccodes_extra_src_files
    grib_bits_fast_big_endian.cc grib_bits_any_endian.cc
    grib_bits_fast_big_endian_vector.cc grib_bits_any_endian_vector.cc
    grib_bits_fast_big_endian_simple.cc grib_bits_any_endian_simple.cc
    grib_bits_fast_big_endian_omp.cc grib_bits_any_endian_omp.cc
    encode_double_array.cc
    grib_bits_ibmpow.cc
    grib_bits_ibmpow_opt.cc )

if(UNIX)
    ecbuild_get_date(eccodes_BUILD_DATE)
endif()
configure_file( grib_api_version.cc.in grib_api_version.cc @ONLY )
configure_file( eccodes_version.h.in eccodes_version.h @ONLY )

# We ship our own generated lex/yacc C files
#ecbuild_generate_yy(YYPREFIX  grib_yy
#                    YACC      griby
#                    LEX       gribl
#                    FLEX_FLAGS " "
#                    LEX_FLAGS  " "
#                    BISON_FLAGS "-y"
#                    YACC_FLAGS  "-y"
#                    DEPENDANT action.cc )

if( HAVE_MEMFS )
    list(APPEND ECCODES_EXTRA_LIBRARIES eccodes_memfs)
endif()

ecbuild_add_library( TARGET    eccodes
                     SOURCES   ${CMAKE_CURRENT_BINARY_DIR}/grib_api_version.cc
                              # griby.cc gribl.cc
                              ${eccodes_src_files}
                     #PRIVATE_LIBS      ${ECCODES_EXTRA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMATH_LIBRARIES}
                     PRIVATE_INCLUDES "${AEC_INCLUDE_DIRS}" "${PNG_INCLUDE_DIRS}"
                     PRIVATE_LIBS ${ECCODES_EXTRA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${AEC_LIBRARIES} ${PNG_LIBRARIES}
                     PUBLIC_LIBS  ${CMATH_LIBRARIES} ${THREADS_LIBRARIES}
                     TEMPLATES ${eccodes_extra_src_files}
                     INSTALL_HEADERS_LIST
                              grib_api.h
                              eccodes.h
                              eccodes_windef.h
                              ${CMAKE_CURRENT_BINARY_DIR}/eccodes_version.h
                              ${PROJECT_BINARY_DIR}/eccodes_config.h
                    PUBLIC_INCLUDES
                              $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
                              $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src> )
