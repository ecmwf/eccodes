/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/* grib_fortran.c */
#ifdef __cplusplus
extern "C" {
#endif

int grib_f_read_any_headers_only_from_file_(int *fid, char *buffer, size_t *nbytes);
int grib_f_read_any_headers_only_from_file__(int *fid, char *buffer, size_t *nbytes);
int grib_f_read_any_headers_only_from_file(int *fid, char *buffer, size_t *nbytes);
int grib_f_read_any_from_file_(int *fid, void *buffer, size_t *nbytes);
int grib_f_read_any_from_file__(int *fid, void *buffer, size_t *nbytes);
int grib_f_read_any_from_file(int *fid, void *buffer, size_t *nbytes);
int grib_f_write_file_(int *fid, void *buffer, size_t *nbytes);
int grib_f_write_file__(int *fid, void *buffer, size_t *nbytes);
int grib_f_write_file(int *fid, void *buffer, size_t *nbytes);
int grib_f_read_file_(int *fid, void *buffer, size_t *nbytes);
int grib_f_read_file__(int *fid, void *buffer, size_t *nbytes);
int grib_f_read_file(int *fid, void *buffer, size_t *nbytes);
int grib_f_open_file_(int *fid, char *name, char *op, int lname, int lop);
int grib_f_open_file__(int *fid, char *name, char *op, int lname, int lop);
int grib_f_open_file(int *fid, char *name, char *op, int lname, int lop);
int grib_f_close_file_(int *fid);
int grib_f_close_file__(int *fid);
int grib_f_close_file(int *fid);
void grib_f_write_on_fail(int *gid);
void grib_f_write_on_fail_(int* gid);
void grib_f_write_on_fail__(int* gid);
int grib_f_multi_support_on_(void);
int grib_f_multi_support_on__(void);
int grib_f_multi_support_on(void);
int grib_f_multi_support_off_(void);
int grib_f_multi_support_off__(void);
int grib_f_multi_support_off(void);
int grib_f_iterator_new_(int *gid, int *iterid, int *mode);
int grib_f_iterator_new__(int *gid, int *iterid, int *mode);
int grib_f_iterator_new(int *gid, int *iterid, int *mode);
int grib_f_iterator_next_(int *iterid, double *lat, double *lon, double *value);
int grib_f_iterator_next__(int *iterid, double *lat, double *lon, double *value);
int grib_f_iterator_next(int *iterid, double *lat, double *lon, double *value);
int grib_f_iterator_delete_(int *iterid);
int grib_f_iterator_delete__(int *iterid);
int grib_f_iterator_delete(int *iterid);

int grib_f_keys_iterator_new_(int *gid, int *iterid, char *name_space, int len);
int grib_f_keys_iterator_new__(int *gid, int *iterid, char *name_space, int len);
int grib_f_keys_iterator_new(int *gid, int *iterid, char *name_space, int len);
int grib_f_keys_iterator_next_(int *iterid);
int grib_f_keys_iterator_next__(int *iterid);
int grib_f_keys_iterator_next(int *iterid);
int grib_f_keys_iterator_delete_(int *iterid);
int grib_f_keys_iterator_delete__(int *iterid);
int grib_f_keys_iterator_delete(int *iterid);

/*BUFR keys iterator*/
int codes_f_bufr_keys_iterator_new_(int *gid, int *iterid);
int codes_f_bufr_keys_iterator_new__(int *gid, int *iterid);
int codes_f_bufr_keys_iterator_new(int *gid, int *iterid);
int codes_f_bufr_keys_iterator_next_(int *iterid);
int codes_f_bufr_keys_iterator_next__(int *iterid);
int codes_f_bufr_keys_iterator_next(int *iterid);
int codes_f_bufr_keys_iterator_get_name_(int *iterid, char *name, int len);
int codes_f_bufr_keys_iterator_get_name__(int *kiter, char *name, int len);
int codes_f_bufr_keys_iterator_get_name(int *kiter, char *name, int len);
int codes_f_bufr_keys_iterator_rewind_(int *kiter);
int codes_f_bufr_keys_iterator_rewind__(int *kiter);
int codes_f_bufr_keys_iterator_rewind(int *kiter);
int codes_f_bufr_keys_iterator_delete_(int *iterid);
int codes_f_bufr_keys_iterator_delete__(int *iterid);
int codes_f_bufr_keys_iterator_delete(int *iterid);

int grib_f_gribex_mode_on_(void);
int grib_f_gribex_mode_on__(void);
int grib_f_gribex_mode_on(void);
int grib_f_gribex_mode_off_(void);
int grib_f_gribex_mode_off__(void);
int grib_f_gribex_mode_off(void);
int grib_f_skip_computed_(int *iterid);
int grib_f_skip_computed__(int *iterid);
int grib_f_skip_computed(int *iterid);
int grib_f_skip_coded_(int *iterid);
int grib_f_skip_coded__(int *iterid);
int grib_f_skip_coded(int *iterid);
int grib_f_skip_edition_specific_(int *iterid);
int grib_f_skip_edition_specific__(int *iterid);
int grib_f_skip_edition_specific(int *iterid);
int grib_f_skip_duplicates_(int *iterid);
int grib_f_skip_duplicates__(int *iterid);
int grib_f_skip_duplicates(int *iterid);
int grib_f_skip_read_only_(int *iterid);
int grib_f_skip_read_only__(int *iterid);
int grib_f_skip_read_only(int *iterid);
int grib_f_skip_function_(int *iterid);
int grib_f_skip_function__(int *iterid);
int grib_f_skip_function(int *iterid);
int grib_f_keys_iterator_get_name_(int *iterid, char *name, int len);
int grib_f_keys_iterator_get_name__(int *kiter, char *name, int len);
int grib_f_keys_iterator_get_name(int *kiter, char *name, int len);
int grib_f_keys_iterator_rewind_(int *kiter);
int grib_f_keys_iterator_rewind__(int *kiter);
int grib_f_keys_iterator_rewind(int *kiter);
int grib_f_new_from_message_(int *gid, void *buffer, size_t *bufsize);
int grib_f_new_from_message__(int *gid, void *buffer, size_t *bufsize);
int grib_f_new_from_message(int *gid, void *buffer, size_t *bufsize);
int grib_f_new_from_message_copy_(int *gid, void *buffer, size_t *bufsize);
int grib_f_new_from_message_copy__(int *gid, void *buffer, size_t *bufsize);
int grib_f_new_from_message_copy(int *gid, void *buffer, size_t *bufsize);
int grib_f_new_from_samples_(int *gid, char *name, int lname);
int grib_f_new_from_samples__(int *gid, char *name, int lname);
int grib_f_new_from_samples(int *gid, char *name, int lname);
int codes_bufr_f_new_from_samples_(int *gid, char *name, int lname);
int codes_bufr_f_new_from_samples__(int *gid, char *name, int lname);
int codes_bufr_f_new_from_samples(int *gid, char *name, int lname);
int grib_f_clone_(int *gidsrc, int *giddest);
int grib_f_clone__(int *gidsrc, int *giddest);
int grib_f_clone(int *gidsrc, int *giddest);
int grib_f_util_sections_copy_(int *gidfrom, int *gidto, int *what, int *gidout);
int grib_f_util_sections_copy__(int *gidfrom, int *gidto, int *what, int *gidout);
int grib_f_util_sections_copy(int *gidfrom, int *gidto, int *what, int *gidout);
int grib_f_copy_namespace_(int *gidsrc, char *name, int *giddest, int len);
int grib_f_copy_namespace__(int *gidsrc, char *name, int *giddest, int len);
int grib_f_copy_namespace(int *gidsrc, char *name, int *giddest, int len);

int grib_f_copy_key_ (int *gidsrc, char *name, int *giddest, int len);
int grib_f_copy_key__(int *gidsrc, char *name, int *giddest, int len);
int grib_f_copy_key  (int *gidsrc, char *name, int *giddest, int len);

int grib_f_count_in_file(int *fid, int *n);
int grib_f_count_in_file_(int *fid, int *n);
int grib_f_count_in_file__(int *fid, int *n);

int any_f_new_from_file_(int *fid, int *gid);
int any_f_new_from_file__(int *fid, int *gid);
int any_f_new_from_file(int *fid, int *gid);

int any_f_scan_file_(int* fid,int* n);
int any_f_scan_file__(int* fid,int* n);
int any_f_scan_file(int* fid,int* n);

int any_f_new_from_scanned_file_(int* fid,int* msgid,int* gid);
int any_f_new_from_scanned_file__(int* fid,int* msgid,int* gid);
int any_f_new_from_scanned_file(int* fid,int* msgid,int* gid);

int any_f_load_all_from_file_(int* fid,int* n);
int any_f_load_all_from_file__(int* fid,int* n);
int any_f_load_all_from_file(int* fid,int* n);

int any_f_new_from_loaded_(int* msgid,int* gid);
int any_f_new_from_loaded__(int* msgid,int* gid);
int any_f_new_from_loaded(int* msgid,int* gid);

int codes_f_clear_loaded_from_file_(void);
int codes_f_clear_loaded_from_file__(void);
int codes_f_clear_loaded_from_file(void);

int grib_f_new_from_file_(int *fid, int *gid);
int grib_f_new_from_file__(int *fid, int *gid);
int grib_f_new_from_file(int *fid, int *gid);

int bufr_f_new_from_file_(int *fid, int *gid);
int bufr_f_new_from_file__(int *fid, int *gid);
int bufr_f_new_from_file(int *fid, int *gid);

int grib_f_headers_only_new_from_file_(int *fid, int *gid);
int grib_f_headers_only_new_from_file__(int *fid, int *gid);
int grib_f_headers_only_new_from_file(int *fid, int *gid);
int grib_f_new_from_index_(int *iid, int *gid);
int grib_f_new_from_index__(int *iid, int *gid);
int grib_f_new_from_index(int *iid, int *gid);
int grib_f_index_new_from_file_(char *file, char *keys, int *gid, int lfile, int lkeys);
int grib_f_index_new_from_file__(char *file, char *keys, int *gid, int lfile, int lkeys);
int grib_f_index_new_from_file(char *file, char *keys, int *gid, int lfile, int lkeys);
int grib_f_index_add_file_(int* iid, char* file, int lfile);
int grib_f_index_add_file__(int* iid, char* file, int lfile);
int grib_f_index_add_file(int* iid, char* file, int lfile);
int grib_f_index_read_(char *file, int *gid, int lfile);
int grib_f_index_read__(char *file, int *gid, int lfile);
int grib_f_index_read(char *file, int *gid, int lfile);
int grib_f_index_write_(int *gid, char *file, int lfile);
int grib_f_index_write__(int *gid, char *file, int lfile);
int grib_f_index_write(int *gid, char *file, int lfile);
int grib_f_index_release_(int *hid);
int grib_f_index_release__(int *hid);
int grib_f_index_release(int *hid);
int grib_f_multi_handle_release_(int *hid);
int grib_f_multi_handle_release__(int *hid);
int grib_f_multi_handle_release(int *hid);
int grib_f_release_(int *hid);
int grib_f_release__(int *hid);
int grib_f_release(int *hid);
int grib_f_dump_(int *gid);
int grib_f_dump__(int *gid);
int grib_f_dump(int *gid);

int grib_f_get_api_version_(int* apiVersion,int len);
int grib_f_get_api_version__(int* apiVersion,int len);
int grib_f_get_api_version(int* apiVersion,int len);

int grib_f_get_error_string_(int *err, char *buf, int len);
int grib_f_get_error_string__(int *err, char *buf, int len);
int grib_f_get_error_string(int *err, char *buf, int len);
int grib_f_get_size_int_(int *gid, char *key, int *val, int len);
int grib_f_get_size_int__(int *gid, char *key, int *val, int len);
int grib_f_get_size_int(int *gid, char *key, int *val, int len);
int grib_f_get_size_long_(int *gid, char *key, long *val, int len);
int grib_f_get_size_long__(int *gid, char *key, long *val, int len);
int grib_f_get_size_long(int *gid, char *key, long *val, int len);
int grib_f_index_get_size_int_(int *gid, char *key, int *val, int len);
int grib_f_index_get_size_int__(int *gid, char *key, int *val, int len);
int grib_f_index_get_size_int(int *gid, char *key, int *val, int len);
int grib_f_index_get_size_long_(int *gid, char *key, long *val, int len);
int grib_f_index_get_size_long__(int *gid, char *key, long *val, int len);
int grib_f_index_get_size_long(int *gid, char *key, long *val, int len);
int grib_f_get_int_(int *gid, char *key, int *val, int len);
int grib_f_get_int__(int *gid, char *key, int *val, int len);
int grib_f_get_int(int *gid, char *key, int *val, int len);
int grib_f_get_long_(int *gid, char *key, long *val, int len);
int grib_f_get_long__(int *gid, char *key, long *val, int len);
int grib_f_get_long(int *gid, char *key, long *val, int len);

int grib_f_get_native_type_(int* gid, char* key, int* val, int len);
int grib_f_get_native_type__(int* gid, char* key, int* val, int len);
int grib_f_get_native_type(int* gid, char* key, int* val, int len);

int grib_f_get_int_array_(int *gid, char *key, int *val, int *size, int len);
int grib_f_get_int_array__(int *gid, char *key, int *val, int *size, int len);
int grib_f_get_int_array(int *gid, char *key, int *val, int *size, int len);
int grib_f_get_long_array_(int *gid, char *key, long *val, int *size, int len);
int grib_f_get_long_array__(int *gid, char *key, long *val, int *size, int len);
int grib_f_get_long_array(int *gid, char *key, long *val, int *size, int len);
int grib_f_get_byte_array_(int* gid, char* key, unsigned char *val, int* size, int len, int lenv);
int grib_f_get_byte_array__(int* gid, char* key, unsigned char *val, int* size, int len, int lenv);
int grib_f_get_byte_array(int* gid, char* key, unsigned char *val, int* size, int len, int lenv);
int grib_f_index_get_string_(int *gid, char *key, char *val, int *eachsize, int *size, int len);
int grib_f_index_get_string__(int *gid, char *key, char *val, int *eachsize, int *size, int len);
int grib_f_index_get_string(int *gid, char *key, char *val, int *eachsize, int *size, int len);
int grib_f_index_get_long_(int *gid, char *key, long *val, int *size, int len);
int grib_f_index_get_long__(int *gid, char *key, long *val, int *size, int len);
int grib_f_index_get_long(int *gid, char *key, long *val, int *size, int len);
int grib_f_index_get_int_(int *gid, char *key, int *val, int *size, int len);
int grib_f_index_get_int__(int *gid, char *key, int *val, int *size, int len);
int grib_f_index_get_int(int *gid, char *key, int *val, int *size, int len);
int grib_f_index_get_real8_(int *gid, char *key, double *val, int *size, int len);
int grib_f_index_get_real8__(int *gid, char *key, double *val, int *size, int len);
int grib_f_index_get_real8(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_int_array_(int *gid, char *key, int *val, int *size, int len);
int grib_f_set_int_array__(int *gid, char *key, int *val, int *size, int len);
int grib_f_set_int_array(int *gid, char *key, int *val, int *size, int len);
int grib_f_set_long_array_(int *gid, char *key, long *val, int *size, int len);
int grib_f_set_long_array__(int *gid, char *key, long *val, int *size, int len);
int grib_f_set_long_array(int *gid, char *key, long *val, int *size, int len);
int grib_f_set_byte_array_(int* gid, char* key, unsigned char *val, int* size, int len, int lenv);
int grib_f_set_byte_array__(int* gid, char* key, unsigned char *val, int* size, int len, int lenv);
int grib_f_set_byte_array(int* gid, char* key, unsigned char *val, int* size, int len, int lenv);
int grib_f_set_int_(int *gid, char *key, int *val, int len);
int grib_f_set_int__(int *gid, char *key, int *val, int len);
int grib_f_set_int(int *gid, char *key, int *val, int len);
int grib_f_set_long_(int *gid, char *key, long *val, int len);
int grib_f_set_long__(int *gid, char *key, long *val, int len);
int grib_f_set_long(int *gid, char *key, long *val, int len);
int grib_f_set_missing_(int *gid, char *key, int len);
int grib_f_set_missing__(int *gid, char *key, int len);
int grib_f_set_missing(int *gid, char *key, int len);
int grib_f_is_missing_(int *gid, char *key, int *isMissing, int len);
int grib_f_is_missing__(int *gid, char *key, int *isMissing, int len);
int grib_f_is_missing(int *gid, char *key, int *isMissing, int len);
int grib_f_is_defined_(int* gid, char* key,int* isDefined,int len);
int grib_f_is_defined__(int* gid, char* key,int* isDefined,int len);
int grib_f_is_defined(int* gid, char* key,int* isDefined,int len);
int grib_f_set_real4_(int *gid, char *key, float *val, int len);
int grib_f_set_real4__(int *gid, char *key, float *val, int len);
int grib_f_set_real4(int *gid, char *key, float *val, int len);
int grib_f_get_real4_element_(int *gid, char *key, int *index, float *val, int len);
int grib_f_get_real4_element__(int *gid, char *key, int *index, float *val, int len);
int grib_f_get_real4_element(int *gid, char *key, int *index, float *val, int len);
int grib_f_get_real4_elements_(int *gid, char *key, int *index, float *val, int *size, int len);
int grib_f_get_real4_elements__(int *gid, char *key, int *index, float *val, int *len, int size);
int grib_f_get_real4_elements(int *gid, char *key, int *index, float *val, int *len, int size);
int grib_f_get_real4_(int *gid, char *key, float *val, int len);
int grib_f_get_real4__(int *gid, char *key, float *val, int len);
int grib_f_get_real4(int *gid, char *key, float *val, int len);
int grib_f_get_real4_array_(int *gid, char *key, float *val, int *size, int len);
int grib_f_get_real4_array__(int *gid, char *key, float *val, int *size, int len);
int grib_f_get_real4_array(int *gid, char *key, float *val, int *size, int len);
int grib_f_set_real4_array_(int *gid, char *key, float *val, int *size, int len);
int grib_f_set_real4_array__(int *gid, char *key, float *val, int *size, int len);
int grib_f_set_real4_array(int *gid, char *key, float *val, int *size, int len);
int grib_f_set_force_real4_array_(int *gid, char *key, float *val, int *size, int len);
int grib_f_set_force_real4_array__(int *gid, char *key, float *val, int *size, int len);
int grib_f_set_force_real4_array(int *gid, char *key, float *val, int *size, int len);
int grib_f_index_select_real8_(int *gid, char *key, double *val, int len);
int grib_f_index_select_real8__(int *gid, char *key, double *val, int len);
int grib_f_index_select_real8(int *gid, char *key, double *val, int len);
int grib_f_index_select_string_(int *gid, char *key, char *val, int len, int vallen);
int grib_f_index_select_string__(int *gid, char *key, char *val, int len, int vallen);
int grib_f_index_select_string(int *gid, char *key, char *val, int len, int vallen);
int grib_f_index_select_int_(int *gid, char *key, int *val, int len);
int grib_f_index_select_int__(int *gid, char *key, int *val, int len);
int grib_f_index_select_int(int *gid, char *key, int *val, int len);
int grib_f_index_select_long_(int *gid, char *key, long *val, int len);
int grib_f_index_select_long__(int *gid, char *key, long *val, int len);
int grib_f_index_select_long(int *gid, char *key, long *val, int len);
int grib_f_set_real8_(int *gid, char *key, double *val, int len);
int grib_f_set_real8__(int *gid, char *key, double *val, int len);
int grib_f_set_real8(int *gid, char *key, double *val, int len);
int grib_f_get_real8_(int *gid, char *key, double *val, int len);
int grib_f_get_real8__(int *gid, char *key, double *val, int len);
int grib_f_get_real8(int *gid, char *key, double *val, int len);
int grib_f_get_real8_element_(int *gid, char *key, int *index, double *val, int len);
int grib_f_get_real8_element__(int *gid, char *key, int *index, double *val, int len);
int grib_f_get_real8_element(int *gid, char *key, int *index, double *val, int len);
int grib_f_get_real8_elements_(int *gid, char *key, int *index, double *val, int *size, int len);
int grib_f_get_real8_elements__(int *gid, char *key, int *index, double *val, int *len, int size);
int grib_f_get_real8_elements(int *gid, char *key, int *index, double *val, int *len, int size);
int grib_f_find_nearest_four_single_(int *gid, int *is_lsm, double *inlat, double *inlon, double *outlats, double *outlons, double *values, double *distances, int *indexes);
int grib_f_find_nearest_four_single__(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes);
int grib_f_find_nearest_four_single(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes);
int grib_f_find_nearest_single_(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes);
int grib_f_find_nearest_single__(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes);
int grib_f_find_nearest_single(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes);
int grib_f_find_nearest_multiple_(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes, int *npoints);
int grib_f_find_nearest_multiple__(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes, int *npoints);
int grib_f_find_nearest_multiple(int *gid, int *is_lsm, double *inlats, double *inlons, double *outlats, double *outlons, double *values, double *distances, int *indexes, int *npoints);
int grib_f_get_real8_array_(int *gid, char *key, double *val, int *size, int len);
int grib_f_get_real8_array__(int *gid, char *key, double *val, int *size, int len);
int grib_f_get_real8_array(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_real8_array_(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_real8_array__(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_real8_array(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_force_real8_array_(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_force_real8_array__(int *gid, char *key, double *val, int *size, int len);
int grib_f_set_force_real8_array(int *gid, char *key, double *val, int *size, int len);

int grib_f_get_string_array(int* gid, char* key, char* val,int* nvals,int* slen,int len);
int grib_f_get_string_array_(int* gid, char* key, char* val,int* nvals,int* slen,int len);
int grib_f_get_string_array__(int* gid, char* key, char* val,int* nvals,int* slen,int len);
int grib_f_set_string_array(int* gid, char* key, char* val,int* nvals,int* slen, int len);
int grib_f_set_string_array_(int* gid, char* key, char* val,int* nvals,int* slen, int len);
int grib_f_set_string_array__(int* gid, char* key, char* val,int* nvals,int* slen, int len);

int grib_f_get_string_(int *gid, char *key, char *val, int len, int len2);
int grib_f_get_string__(int *gid, char *key, char *val, int len, int len2);
int grib_f_get_string(int *gid, char *key, char *val, int len, int len2);
int grib_f_set_string_(int *gid, char *key, char *val, int len, int len2);
int grib_f_set_string__(int *gid, char *key, char *val, int len, int len2);
int grib_f_set_string(int *gid, char *key, char *val, int len, int len2);
int grib_f_get_data_real4_(int *gid, float *lats, float *lons, float *values, size_t *size);
int grib_f_get_data_real4__(int *gid, float *lats, float *lons, float *values, size_t *size);
int grib_f_get_data_real4(int *gid, float *lats, float *lons, float *values, size_t *size);
int grib_f_get_data_real8_(int *gid, double *lats, double *lons, double *values, size_t *size);
int grib_f_get_data_real8__(int *gid, double *lats, double *lons, double *values, size_t *size);
int grib_f_get_data_real8(int *gid, double *lats, double *lons, double *values, size_t *size);
int grib_f_get_message_size_(int *gid, size_t *len);
int grib_f_get_message_size__(int *gid, size_t *len);
int grib_f_get_message_size(int *gid, size_t *len);
int grib_f_copy_message_(int *gid, void *mess, size_t *len);
int grib_f_copy_message__(int *gid, void *mess, size_t *len);
int grib_f_copy_message(int *gid, void *mess, size_t *len);
void grib_f_check_(int *err, char *call, char *str, int lencall, int lenstr);
void grib_f_check__(int *err, char *call, char *key, int lencall, int lenkey);
void grib_f_check(int *err, char *call, char *key, int lencall, int lenkey);
int grib_f_write_(int *gid, int *fid);
int grib_f_write__(int *gid, int *fid);
int grib_f_write(int *gid, int *fid);
int grib_f_multi_write_(int *gid, int *fid);
int grib_f_multi_write__(int *gid, int *fid);
int grib_f_multi_write(int *gid, int *fid);
int grib_f_multi_append_(int *ingid, int *sec, int *mgid);
int grib_f_multi_append(int *ingid, int *sec, int *mgid);
int grib_f_multi_append__(int *ingid, int *sec, int *mgid);

int codes_f_bufr_copy_data(int* gid1,int* gid2);
int codes_f_bufr_copy_data_(int* gid1,int* gid2);
int codes_f_bufr_copy_data__(int* gid1,int* gid2);

int codes_f_bufr_multi_element_constant_arrays_on_(void);
int codes_f_bufr_multi_element_constant_arrays_on__(void);
int codes_f_bufr_multi_element_constant_arrays_on(void);
int codes_f_bufr_multi_element_constant_arrays_off_(void);
int codes_f_bufr_multi_element_constant_arrays_off__(void);
int codes_f_bufr_multi_element_constant_arrays_off(void);

int grib_f_set_definitions_path_(char *path, int len);
int grib_f_set_definitions_path__(char *path, int len);
int grib_f_set_definitions_path(char *path, int len);
int grib_f_set_samples_path_(char *path, int len);
int grib_f_set_samples_path__(char *path, int len);
int grib_f_set_samples_path(char *path, int len);

int grib_f_julian_to_datetime(double* jd,long* year,long* month,long* day,long *hour,long *minute,long *second);
int grib_f_julian_to_datetime_(double* jd,long* year,long* month,long* day,long *hour,long *minute,long *second);
int grib_f_julian_to_datetime__(double* jd,long* year,long* month,long* day,long *hour,long *minute,long *second);

int grib_f_datetime_to_julian(long* year,long* month,long* day, long* hour,long* minute,long* second,double* jd);
int grib_f_datetime_to_julian_(long* year,long* month,long* day, long* hour,long* minute,long* second,double* jd);
int grib_f_datetime_to_julian__(long* year,long* month,long* day, long* hour,long* minute,long* second,double* jd);

#ifdef __cplusplus
}
#endif
