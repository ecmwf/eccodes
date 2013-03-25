# See http://www.delorie.com/gnu/docs/automake/automake_48.html


AM_CFLAGS =  @WARN_PEDANTIC@

# -DUSE_JPEG2000
#  -DYYDEBUG


lib_LTLIBRARIES = libgrib_api.la
include_HEADERS = grib_api.h

libgrib_api_la_prototypes= \
	action.c \
         action_class_alias.c \
         action_class_gen.c \
         action_class_if.c \
		 action_class_switch.c \
		grib_accessor_class_g1fcperiod.c \
		grib_accessor_class_g1end_of_interval_monthly.c \
		grib_accessor_class_mars_param.c \
         action_class_section.c \
         action_class_list.c \
         action_class_while.c \
         action_class_put.c \
         action_class_meta.c \
         action_class_remove.c \
         action_class_assert.c \
         action_class_template.c \
         action_class_trigger.c \
         action_class_when.c \
         action_class_concept.c \
         action_class_set.c \
         action_class_set_darray.c \
         action_class_set_iarray.c \
         action_class_noop.c \
         action_class_write.c \
         action_class_print.c \
         action_class_variable.c \
         action_class_modify.c \
         grib_accessor.c \
         grib_concept.c \
         grib_darray.c \
         grib_iarray.c \
           grib_accessor_class_array.c \
           grib_accessor_class_assert.c \
           grib_accessor_class_ascii.c \
           grib_accessor_class_bit.c \
           grib_accessor_class_bitmap.c \
		   grib_accessor_class_bits.c  \
           grib_accessor_class_bits_per_value.c \
           grib_accessor_class_g1bitmap.c \
           grib_accessor_class_g2bitmap.c \
           grib_accessor_class_concept.c \
           grib_accessor_class_decimal_precision.c \
           grib_accessor_class_divdouble.c \
           grib_accessor_class_budgdate.c \
           grib_accessor_class_validity_date.c \
           grib_accessor_class_validity_time.c \
           grib_accessor_class_bytes.c \
           grib_accessor_class.c \
           grib_accessor_class_change_scanning_direction.c \
           grib_accessor_class_codeflag.c \
           grib_accessor_class_codetable.c \
           grib_accessor_class_codetable_units.c \
           grib_accessor_class_codetable_title.c \
           grib_accessor_class_count_file.c \
           grib_accessor_class_count_total.c \
           grib_accessor_class_double.c \
           grib_accessor_class_element.c \
           grib_accessor_class_evaluate.c \
           grib_accessor_class_g1area.c \
           grib_accessor_class_g1date.c \
           grib_accessor_class_g1monthlydate.c \
		   grib_accessor_class_library_version.c \
           grib_accessor_class_when.c \
           grib_accessor_class_g1verificationdate.c \
           grib_accessor_class_g1day_of_the_year_date.c \
           grib_accessor_class_g1_half_byte_codeflag.c \
           grib_accessor_class_g1forecastmonth.c \
		   grib_accessor_class_g1step_range.c \
		   grib_accessor_class_g2step_range.c \
		   grib_accessor_class_data_g22order_packing.c \
		   grib_accessor_class_mars_step.c \
           grib_accessor_class_g1param.c \
           grib_accessor_class_g1p1p2.c \
           grib_accessor_class_g1_increment.c \
           grib_accessor_class_latlon_increment.c \
           grib_accessor_class_g2date.c \
           grib_accessor_class_g2level.c \
           grib_accessor_class_g2step.c \
           grib_accessor_class_g2end_step.c \
           grib_accessor_class_g2latlon.c \
           grib_accessor_class_g2lon.c \
		   grib_accessor_class_global_gaussian.c \
           grib_accessor_class_gen.c \
		   grib_accessor_class_gts_header.c \
		   grib_accessor_class_ifs_param.c \
           grib_accessor_class_julian_day.c \
		   grib_accessor_class_latlonvalues.c \
		   grib_accessor_class_latitudes.c \
		   grib_accessor_class_longitudes.c \
           grib_accessor_class_missing.c \
           grib_accessor_class_offset_file.c \
           grib_accessor_class_scale.c \
		   grib_accessor_class_from_scale_factor_scaled_value.c \
           grib_accessor_class_times.c \
           grib_accessor_class_forward.c \
		   grib_accessor_class_g2bitmap_present.c \
           grib_accessor_class_ibmfloat.c \
           grib_accessor_class_ieeefloat.c \
           grib_accessor_class_constant.c \
           grib_accessor_class_iterator.c \
           grib_accessor_class_message.c \
           grib_accessor_class_nearest.c \
           grib_accessor_class_box.c \
           grib_accessor_class_ksec1expver.c \
		   grib_accessor_class_laplacian.c \
           grib_accessor_class_label.c \
           grib_accessor_class_long.c \
           grib_accessor_class_lookup.c \
           grib_accessor_class_octect_number.c \
           grib_accessor_class_headers_only.c \
           grib_accessor_class_padding.c \
           grib_accessor_class_pad.c \
           grib_accessor_class_padto.c \
           grib_accessor_class_padtoeven.c \
           grib_accessor_class_padtomultiple.c \
           grib_accessor_class_section_padding.c \
		   grib_accessor_class_section_pointer.c \
           grib_accessor_class_position.c \
           grib_accessor_class_signed.c \
           grib_accessor_class_signed_bits.c \
           grib_accessor_class_section.c \
		   grib_accessor_class_step_in_units.c \
           grib_accessor_class_section_length.c \
           grib_accessor_class_g1_message_length.c \
           grib_accessor_class_g1_section4_length.c \
           grib_accessor_class_size.c \
           grib_accessor_class_scale_values.c \
           grib_accessor_class_offset_values.c \
           grib_accessor_class_sprintf.c \
           grib_accessor_class_round.c \
           grib_accessor_class_spectral_truncation.c \
           grib_accessor_class_time.c \
           grib_accessor_class_transient.c \
           grib_accessor_class_values.c \
		   grib_accessor_class_simple_packing_error.c \
           grib_accessor_class_data_simple_packing.c \
	   grib_accessor_class_data_szip_packing.c \
		   grib_accessor_class_count_missing.c \
		   grib_accessor_class_data_sh_packed.c \
		   grib_accessor_class_data_sh_unpacked.c \
		   grib_accessor_class_number_of_values_data_raw_packing.c \
           grib_accessor_class_data_g1simple_packing.c \
           grib_accessor_class_data_g1shsimple_packing.c \
           grib_accessor_class_data_shsimple_packing.c \
           grib_accessor_class_data_constant_field.c \
           grib_accessor_class_data_dummy_field.c \
           grib_2order_packer_simple.c \
           grib_accessor_class_variable.c \
		   grib_accessor_class_second_order_bits_per_value.c \
           grib_accessor_class_data_g2simple_packing.c \
           grib_accessor_class_data_g2simple_packing_with_preprocessing.c \
           grib_accessor_class_data_g2shsimple_packing.c \
           grib_accessor_class_data_g2complex_packing.c \
           grib_accessor_class_data_2order_packing.c \
           grib_accessor_class_data_2order_packing_count.c \
		   grib_accessor_class_data_g1second_order_row_by_row_packing.c \
		   grib_accessor_class_data_g1second_order_constant_width_packing.c \
		   grib_accessor_class_data_g1second_order_general_packing.c \
		   grib_accessor_class_data_g1second_order_general_extended_packing.c \
           grib_accessor_class_g2grid.c \
           grib_accessor_class_data_apply_bitmap.c \
		   grib_accessor_class_data_apply_boustrophedonic.c \
           grib_accessor_class_data_secondary_bitmap.c \
           grib_accessor_class_data_g1secondary_bitmap.c \
           grib_accessor_class_data_g2secondary_bitmap.c \
           grib_accessor_class_data_jpeg2000_packing.c \
           grib_accessor_class_data_png_packing.c \
           grib_accessor_class_data_raw_packing.c \
           grib_accessor_class_data_complex_packing.c \
           grib_accessor_class_data_g1complex_packing.c \
           grib_accessor_class_gds_not_present_bitmap.c \
           grib_accessor_class_data_apply_gdsnotpresent.c \
		   grib_accessor_class_gds_is_present.c \
		   grib_accessor_class_select_step_template.c \
		   grib_accessor_class_local_definition.c \
		   grib_accessor_class_g2_eps.c \
		   grib_accessor_class_g2_mars_labeling.c \
		   grib_accessor_class_md5.c \
		   grib_jasper_encoding.c \
		   grib_openjpeg_encoding.c \
		   action_class_set_missing.c \
       grib_accessor_class_number_of_points.c \
	   grib_accessor_class_suppressed.c \
	   grib_index.c \
       grib_accessor_class_number_of_points_gaussian.c \
       grib_accessor_class_number_of_values.c \
       grib_accessor_class_number_of_coded_values.c \
       grib_accessor_class_g1number_of_coded_values_sh_complex.c \
       grib_accessor_class_g1number_of_coded_values_sh_simple.c \
       grib_accessor_class_dirty.c \
       grib_accessor_class_statistics.c \
       grib_accessor_class_statistics_spectral.c \
           grib_accessor_class_unsigned.c  \
		   grib_accessor_class_unsigned_bits.c \
		   grib_accessor_class_spd.c \
		   grib_accessor_class_sum.c \
       grib_accessor_class_vector.c \
       grib_accessor_class_long_vector.c \
       grib_gaussian_reduced.c \
       grib_accessor_class_abstract_vector.c \
       grib_accessor_class_abstract_long_vector.c \
           grib_loader_from_handle.c  \
     grib_bits.c \
     grib_timer.c \
     grib_ibmfloat.c \
     grib_ieeefloat.c \
	 grib_accessor_class_reference_value_error.c \
     grib_memory.c \
     grib_buffer.c \
     grib_dumper.c \
     grib_dumper_class_serialize.c \
     grib_dumper_class_debug.c \
     grib_dumper_class_default.c \
     grib_dumper_class_keys.c \
     grib_dumper_class_c_code.c \
     grib_dumper_class_wmo.c \
     grib_dumper_class.c \
     grib_context.c \
     grib_date.c \
     grib_fieldset.c \
     grib_filepool.c \
     grib_geography.c \
     grib_handle.c \
	 grib_header_compute.c \
	 grib_hash_keys.c \
     grib_io.c \
     grib_trie.c \
     grib_itrie.c \
     grib_rules.c \
     grib_keys_iterator.c \
     grib_parse_utils.c \
     grib_query.c \
     grib_scaling.c \
     grib_templates.c \
     grib_dependency.c \
     grib_value.c \
     grib_errors.c \
   grib_expression_class_binop.c \
   grib_expression_class_true.c \
   grib_expression_class_string_compare.c \
   grib_expression_class_unop.c \
   grib_expression_class_functor.c \
   grib_expression_class_accessor.c \
   grib_expression_class_long.c \
   grib_expression_class_double.c \
   grib_expression_class_string.c \
		   grib_box.c \
		   grib_box_class.c \
		   grib_box_class_gen.c \
		   grib_box_class_regular_gaussian.c \
		   grib_box_class_reduced_gaussian.c \
           grib_nearest.c \
           grib_nearest_class.c \
           grib_nearest_class_gen.c \
           grib_nearest_class_regular.c \
           grib_nearest_class_reduced.c \
		   grib_nearest_class_latlon_reduced.c \
           grib_nearest_class_sh.c \
		   grib_iterator_class_polar_stereographic.c \
		   grib_iterator_class_lambert_azimuthal_equal_area.c  \
	grib_iterator_class_lambert_conformal.c \
           grib_iterator.c \
           grib_iterator_class.c \
           grib_iterator_class_gaussian.c \
           grib_iterator_class_gaussian_reduced.c \
           grib_iterator_class_latlon_reduced.c \
           grib_iterator_class_gen.c \
           grib_iterator_class_latlon.c \
           grib_iterator_class_regular.c \
           grib_expression.c \
		   grib_util.c \
		   compile.c \
		   functions.c 

libgrib_api_extra_prototypes = grib_bits_any_endian.c grib_bits_any_endian_simple.c

libgrib_api_la_SOURCES = $(libgrib_api_la_prototypes) grib_yacc.c  grib_lex.c md5.c

libgrib_api_la_LIBADD = @LIBOBJS@
libgrib_api_la_DEPENDENCIES = grib_api.h $(libgrib_api_la_LIBADD)
libgrib_api_la_LDFLAGS = -release $(GRIB_API_MAIN_VERSION)

# set the include path
INCLUDES=  -I.

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libgrib_api.la

noinst_HEADERS = grib_api_internal.h \
                 md5.h \
                 grib_emoslib.h \
                 grib_api_prototypes.h \
                 grib_dumper_class.h \
                 errors.pl \
                 grib_dumper_factory.h \
                 grib_accessor_class.h \
                 grib_accessor_factory.h \
                 grib_expression.h \
                 grib_nearest_class.h \
                 grib_nearest_factory.h \
                 grib_box_class.h \
                 grib_box_factory.h \
                 grib_iterator_class.h \
                 grib_iterator_factory.h \
                 grib_templates.h \
                 grib_yacc.h \
                 grib2c.pl \
                 grib_errors_internal.txt \
                 grib_errors.txt

EXTRA_DIST= dummy.am griby.y gribl.l grib_accessor_classes_hash.c \
            grib_bits_fast_big_endian.c grib_bits_any_endian.c \
            grib_bits_fast_big_endian_vector.c grib_bits_any_endian_vector.c \
            grib_bits_fast_big_endian_simple.c grib_bits_any_endian_simple.c \
            grib_bits_fast_big_endian_omp.c grib_bits_any_endian_omp.c

include $(DEVEL_RULES)


