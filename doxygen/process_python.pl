#!/usr/bin/env perl
use Data::Dumper;
use strict;

$|=1;
my $debug = 0;

while (<>) {

    s/\@package gribapi/\@package ecCodes/;

    s/\bGRIB_CHECK\b/CODES_CHECK/;
    s/\bGRIB_MISSING_DOUBLE\b/CODES_MISSING_DOUBLE/;
    s/\bGRIB_MISSING_LONG\b/CODES_MISSING_LONG/;

    s/\bgrib_new_from_file\b/codes_grib_new_from_file/;
    s/\bbufr_new_from_file\b/codes_bufr_new_from_file/;
    s/\bmetar_new_from_file\b/codes_metar_new_from_file/;
    s/\bgts_new_from_file\b/codes_gts_new_from_file/;
    s/\bany_new_from_file\b/codes_any_new_from_file/;

    s/\bgrib_count_in_file\b/codes_count_in_file/;
    s/\bgrib_multi_support_on\b/codes_grib_multi_support_on/;
    s/\bgrib_multi_support_off\b/codes_grib_multi_support_off/;
    s/\bgrib_release\b/codes_release/;
    s/\bgrib_get_string\b/codes_get_string/;
    s/\bgrib_set_string\b/codes_set_string/;
    s/\bgrib_gribex_mode_on\b/codes_gribex_mode_on/;
    s/\bgrib_gribex_mode_off\b/codes_gribex_mode_off/;
    s/\bgrib_write\b/codes_write/;
    s/\bgrib_multi_write\b/codes_grib_multi_write/;
    s/\bgrib_multi_append\b/codes_grib_multi_append/;
    s/\bgrib_get_size\b/codes_get_size/;
    s/\bgrib_get_string_length\b/codes_get_string_length/;
    s/\bgrib_skip_computed\b/codes_skip_computed/;
    s/\bgrib_skip_coded\b/codes_skip_coded/;
    s/\bgrib_skip_edition_specific\b/codes_skip_edition_specific/;
    s/\bgrib_skip_duplicates\b/codes_skip_duplicates/;
    s/\bgrib_skip_read_only\b/codes_skip_read_only/;
    s/\bgrib_skip_function\b/codes_skip_function/;
    s/\bgrib_iterator_new\b/codes_grib_iterator_new/;
    s/\bgrib_iterator_delete\b/codes_grib_iterator_delete/;
    s/\bgrib_iterator_next\b/codes_grib_iterator_next/;
    s/\bgrib_keys_iterator_new\b/codes_keys_iterator_new/;
    s/\bgrib_keys_iterator_next\b/codes_keys_iterator_next/;
    s/\bgrib_keys_iterator_delete\b/codes_keys_iterator_delete/;
    s/\bgrib_keys_iterator_get_name\b/codes_keys_iterator_get_name/;
    s/\bgrib_keys_iterator_rewind\b/codes_keys_iterator_rewind/;
    s/\bgrib_get_long\b/codes_get_long/;
    s/\bgrib_get_double\b/codes_get_double/;
    s/\bgrib_set_long\b/codes_set_long/;
    s/\bgrib_set_double\b/codes_set_double/;
    s/\bgrib_new_from_samples\b/codes_grib_new_from_samples/;
    s/\bgrib_clone\b/codes_clone/;
    s/\bgrib_set_double_array\b/codes_set_double_array/;
    s/\bgrib_get_double_array\b/codes_get_double_array/;
    s/\bgrib_set_long_array\b/codes_set_long_array/;
    s/\bgrib_get_long_array\b/codes_get_long_array/;
    s/\bgrib_set_string_array\b/codes_set_string_array/;
    s/\bgrib_get_string_array\b/codes_get_string_array/;
    s/\bgrib_multi_new\b/codes_grib_multi_new/;
    s/\bgrib_multi_release\b/codes_grib_multi_release/;
    s/\bgrib_copy_namespace\b/codes_copy_namespace/;
    s/\bgrib_index_new_from_file\b/codes_index_new_from_file/;
    s/\bgrib_index_add_file\b/codes_index_add_file/;
    s/\bgrib_index_release\b/codes_index_release/;
    s/\bgrib_index_get_size\b/codes_index_get_size/;
    s/\bgrib_index_get_long\b/codes_index_get_long/;
    s/\bgrib_index_get_string\b/codes_index_get_string/;
    s/\bgrib_index_get_double\b/codes_index_get_double/;
    s/\bgrib_index_select_long\b/codes_index_select_long/;
    s/\bgrib_index_select_double\b/codes_index_select_double/;
    s/\bgrib_index_select_string\b/codes_index_select_string/;
    s/\bgrib_new_from_index\b/codes_new_from_index/;
    s/\bgrib_get_message_size\b/codes_get_message_size/;
    s/\bgrib_get_message_offset\b/codes_get_message_offset/;
    s/\bgrib_get_double_element\b/codes_get_double_element/;
    s/\bgrib_get_double_elements\b/codes_get_double_elements/;
    s/\bgrib_get_elements\b/codes_get_elements/;
    s/\bgrib_set_missing\b/codes_set_missing/;
    s/\bgrib_set_key_vals\b/codes_set_key_vals/;
    s/\bgrib_is_missing\b/codes_is_missing/;
    s/\bgrib_is_defined\b/codes_is_defined/;
    s/\bgrib_find_nearest\b/codes_grib_find_nearest/;
    s/\bgrib_get_native_type\b/codes_get_native_type/;
    s/\bgrib_get\b/codes_get/;
    s/\bgrib_get_array\b/codes_get_array/;
    s/\bgrib_get_values\b/codes_get_values/;
    s/\bgrib_set_values\b/codes_set_values/;
    s/\bgrib_set\b/codes_set/;
    s/\bgrib_set_array\b/codes_set_array/;
    s/\bgrib_index_get\b/codes_index_get/;
    s/\bgrib_index_select\b/codes_index_select/;
    s/\bgrib_index_write\b/codes_index_write/;
    s/\bgrib_index_read\b/codes_index_read/;
    s/\bgrib_no_fail_on_wrong_length\b/codes_no_fail_on_wrong_length/;
    s/\bgrib_gts_header\b/codes_gts_header/;
    s/\bgrib_get_api_version\b/codes_get_api_version/;
    s/\bgrib_get_message\b/codes_get_message/;
    s/\bgrib_new_from_message\b/codes_new_from_message/;
    s/\bgrib_set_definitions_path\b/codes_set_definitions_path/;
    s/\bgrib_set_samples_path\b/codes_set_samples_path/;

    s/\bGribInternalError\b/CodesInternalError/;
    
    print;
}
