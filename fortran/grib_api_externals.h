integer, external :: grib_f_open_file, grib_f_close_file, &
                     grib_f_read_file,grib_f_write_file
integer, external :: grib_f_multi_support_on, grib_f_multi_support_off
integer, external :: grib_f_keys_iterator_new, &
                     grib_f_keys_iterator_next, &
                     grib_f_keys_iterator_delete

integer, external :: codes_f_bufr_keys_iterator_new, &
                     codes_f_bufr_keys_iterator_next, &
                     codes_f_bufr_keys_iterator_get_name, &
                     codes_f_bufr_keys_iterator_rewind, &
                     codes_f_bufr_keys_iterator_delete

integer, external :: grib_f_skip_computed, &
                     grib_f_skip_coded, &
                     grib_f_skip_edition_specific, &
                     grib_f_skip_duplicates, &
                     grib_f_skip_read_only, &
                     grib_f_skip_function
integer, external :: grib_f_keys_iterator_get_name, &
                     grib_f_keys_iterator_rewind
integer, external :: grib_f_new_from_message, &
                     grib_f_new_from_message_copy, &
                     grib_f_new_from_samples, &
                     codes_bufr_f_new_from_samples, &
                     grib_f_read_any_from_file, &
                     any_f_new_from_file, &
                     any_f_load_all_from_file, &
                     any_f_new_from_loaded, &
                     any_f_scan_file, &
                     any_f_new_from_scanned_file, &
                     codes_f_clear_loaded_from_file, &
                     grib_f_new_from_file, &
                     bufr_f_new_from_file, &
                     grib_f_headers_only_new_from_file
integer, external :: grib_f_release
integer, external :: grib_f_dump, grib_f_print
integer, external :: grib_f_get_error_string
integer, external :: grib_f_get_size_int,grib_f_get_size_long
integer, external :: grib_f_get_data_real4,grib_f_get_data_real8
integer, external :: grib_f_get_int, grib_f_get_long,grib_f_get_int_array, &
                     grib_f_get_long_array,grib_f_get_real4,&
                     grib_f_get_real4_array, &
                     grib_f_get_byte_array,&
                     grib_f_get_real8, grib_f_get_real8_array, &
                     grib_f_get_real4_element, grib_f_get_real8_element, &
                     grib_f_get_real4_elements, grib_f_get_real8_elements, &
                     grib_f_get_string,grib_f_get_string_array, &
                     codes_f_bufr_copy_data, &
                     grib_f_is_missing,grib_f_is_defined
integer, external :: grib_f_new_from_index, &
                     grib_f_index_new_from_file, &
                     grib_f_index_add_file, &
                     grib_f_index_read, &
                     grib_f_index_write, &
                     grib_f_index_release, &
                     grib_f_index_get_size_long, &
                     grib_f_index_get_size_int, &
                     grib_f_index_get_int, &
                     grib_f_index_get_long, &
                     grib_f_index_get_string, &
                     grib_f_index_get_real8, &
                     grib_f_index_select_real8, &
                     grib_f_index_select_string, &
                     grib_f_index_select_int, &
                     grib_f_index_select_long
                         
integer, external :: grib_f_set_int, grib_f_set_int_array, &
                     grib_f_set_long, grib_f_set_long_array, &
                     grib_f_set_byte_array, &
                     grib_f_set_real4, grib_f_set_real4_array, &
                     grib_f_set_real8, grib_f_set_real8_array, &
                     grib_f_set_force_real4_array, grib_f_set_force_real8_array, &
                     grib_f_set_string, grib_f_set_string_array, grib_f_set_missing, &
                     grib_f_gribex_mode_on,grib_f_gribex_mode_off, &
                     grib_f_find_nearest_single,grib_f_find_nearest_four_single,grib_f_find_nearest_multiple
integer, external :: grib_f_get_message_size, grib_f_copy_message, grib_f_count_in_file
integer, external :: grib_f_write, grib_f_multi_write, grib_f_multi_append
integer, external :: grib_f_clone, grib_f_copy_namespace
external :: grib_f_check
integer, external :: grib_f_util_sections_copy
integer, external :: grib_f_set_definitions_path, grib_f_set_samples_path
integer, external :: grib_f_julian_to_datetime, grib_f_datetime_to_julian, grib_f_copy_key
