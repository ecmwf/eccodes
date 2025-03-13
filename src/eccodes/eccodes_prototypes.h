/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#ifdef ECCODES_ON_WINDOWS
    #include <stdint.h>
#endif

/* action.cc */
void grib_action_delete(grib_context* context, grib_action* a);
int grib_create_accessor(grib_section* p, grib_action* a, grib_loader* h);
int grib_action_notify_change(grib_action* a, grib_accessor* observer, grib_accessor* observed);
grib_action* grib_action_reparse(grib_action* a, grib_accessor* acc, int* doit);
int grib_action_execute(grib_action* a, grib_handle* h);
void grib_dump_action_branch(FILE* out, grib_action* a, int decay);
void grib_dump_action_tree(grib_context* ctx, FILE* out);

/* action_class_alias.cc */
grib_action* grib_action_create_alias(grib_context* context, const char* name, const char* arg1, const char* name_space, int flags);

/* action_class_gen.cc */
grib_action* grib_action_create_gen(grib_context* context, const char* name, const char* op, const long len, grib_arguments* params, grib_arguments* default_value, int flags, const char* name_space, const char* set);

/* action_class_if.cc */
grib_action* grib_action_create_if(grib_context* context, grib_expression* expression, grib_action* block_true, grib_action* block_false, int transient, int lineno, const char* file_being_parsed);

/* action_class_switch.cc */
grib_action* grib_action_create_switch(grib_context* context, grib_arguments* args, grib_case* Case, grib_action* Default);
grib_case* grib_case_new(grib_context* c, grib_arguments* values, grib_action* action);

/* action_class_list.cc */
grib_action* grib_action_create_list(grib_context* context, const char* name, grib_expression* expression, grib_action* block);

/* action_class_while.cc */
grib_action* grib_action_create_while(grib_context* context, grib_expression* expression, grib_action* block);

/* action_class_put.cc */
grib_action* grib_action_create_put(grib_context* context, const char* name, grib_arguments* args);

/* action_class_meta.cc */
grib_action* grib_action_create_meta(grib_context* context, const char* name, const char* op, grib_arguments* params, grib_arguments* default_value, unsigned long flags, const char* name_space);

/* action_class_remove.cc */
grib_action* grib_action_create_remove(grib_context* context, grib_arguments* args);

/* action_class_rename.cc */
grib_action* grib_action_create_rename(grib_context* context, char* the_old, char* the_new);

/* action_class_assert.cc */
grib_action* grib_action_create_assert(grib_context* context, grib_expression* expression);

/* action_class_template.cc */
grib_action* grib_action_create_template(grib_context* context, int nofail, const char* name, const char* arg1);

/* action_class_trigger.cc */
grib_action* grib_action_create_trigger(grib_context* context, grib_arguments* args, grib_action* block);

/* action_class_when.cc */
grib_action* grib_action_create_when(grib_context* context, grib_expression* expression, grib_action* block_true, grib_action* block_false);

/* action_class_concept.cc */
grib_concept_value* action_concept_get_concept(grib_accessor* a);
int action_concept_get_nofail(grib_accessor* a);
grib_action* grib_action_create_concept(grib_context* context, const char* name, grib_concept_value* concept_value, const char* basename, const char* name_space, const char* defaultkey, const char* masterDir, const char* localDir, const char* ecmfDir, int flags, int nofail);
int get_concept_condition_string(grib_handle* h, const char* key, const char* value, char* result);

/* action_class_hash_array.cc */
grib_action* grib_action_create_hash_array(grib_context* context, const char* name, grib_hash_array_value* hash_array, const char* basename, const char* name_space, const char* defaultkey, const char* masterDir, const char* localDir, const char* ecmfDir, int flags, int nofail);
grib_hash_array_value* get_hash_array(grib_handle* h, grib_action* a);
const char* get_hash_array_full_path(grib_action* a);

/* action_class_set.cc */
grib_action* grib_action_create_set(grib_context* context, const char* name, grib_expression* expression, int nofail);

/* action_class_set_darray.cc */
grib_action* grib_action_create_set_darray(grib_context* context, const char* name, grib_darray* darray);

/* action_class_set_sarray.cc */
grib_action* grib_action_create_set_sarray(grib_context* context, const char* name, grib_sarray* sarray);

/* action_class_noop.cc */
grib_action* grib_action_create_noop(grib_context* context, const char* fname);

/* action_class_write.cc */
grib_action* grib_action_create_write(grib_context* context, const char* name, int append, int padtomultiple);

/* action_class_print.cc */
grib_action* grib_action_create_print(grib_context* context, const char* name, char* outname);

/* action_class_close.cc */
grib_action* grib_action_create_close(grib_context* context, char* filename);

/* action_class_variable.cc */
grib_action* grib_action_create_variable(grib_context* context, const char* name, const char* op, const long len, grib_arguments* params, grib_arguments* default_value, int flags, const char* name_space);

/* action_class_modify.cc */
grib_action* grib_action_create_modify(grib_context* context, const char* name, long flags);

/* action_class_transient_darray.cc */
grib_action* grib_action_create_transient_darray(grib_context* context, const char* name, grib_darray* darray, int flags);

/* grib_accessor.cc */
void grib_accessor_dump(grib_accessor* a, grib_dumper* f);
int grib_pack_missing(grib_accessor* a);
int grib_is_missing_internal(grib_accessor* a);
int grib_pack_double(grib_accessor* a, const double* v, size_t* len);
int grib_pack_float(grib_accessor* a, const float* v, size_t* len);
int grib_pack_expression(grib_accessor* a, grib_expression* e);
int grib_pack_string(grib_accessor* a, const char* v, size_t* len);
int grib_pack_string_array(grib_accessor* a, const char** v, size_t* len);
int grib_pack_long(grib_accessor* a, const long* v, size_t* len);
int grib_pack_bytes(grib_accessor* a, const unsigned char* v, size_t* len);
int grib_unpack_bytes(grib_accessor* a, unsigned char* v, size_t* len);
int grib_unpack_double_subarray(grib_accessor* a, double* v, size_t start, size_t len);
int grib_unpack_double(grib_accessor* a, double* v, size_t* len);
int grib_unpack_float(grib_accessor* a, float* v, size_t* len);
int grib_unpack_double_element(grib_accessor* a, size_t i, double* v);
int grib_unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array);
int grib_unpack_float_element(grib_accessor* a, size_t i, float* v);
int grib_unpack_float_element_set(grib_accessor* a, const size_t* index_array, size_t len, float* val_array);
int grib_unpack_string(grib_accessor* a, char* v, size_t* len);
int grib_unpack_string_array(grib_accessor* a, char** v, size_t* len);
int grib_accessors_list_unpack_long(grib_accessors_list* al, long* val, size_t* buffer_len);
int grib_accessors_list_unpack_double(grib_accessors_list* al, double* val, size_t* buffer_len);
int grib_accessors_list_unpack_float(grib_accessors_list* al, float* val, size_t* buffer_len);
int grib_accessors_list_unpack_string(grib_accessors_list* al, char** val, size_t* buffer_len);
int grib_unpack_long(grib_accessor* a, long* v, size_t* len);
long grib_accessor_get_native_type(grib_accessor* a);
long grib_get_next_position_offset(grib_accessor* a);
long grib_string_length(grib_accessor* a);
long grib_byte_offset(grib_accessor* a);
long grib_byte_count(grib_accessor* a);
int grib_value_count(grib_accessor* a, long* count);
int grib_accessors_list_value_count(grib_accessors_list* al, size_t* count);
int grib_accessor_notify_change(grib_accessor* a, grib_accessor* changed);
void grib_init_accessor(grib_accessor* a, const long len, grib_arguments* args);
void grib_accessor_delete(grib_context* ct, grib_accessor* a);
grib_accessor* grib_accessor_clone(grib_accessor* a, grib_section* s, int* err);
void grib_update_size(grib_accessor* a, size_t len);
int grib_nearest_smaller_value(grib_accessor* a, double val, double* nearest);
size_t grib_preferred_size(grib_accessor* a, int from_handle);
grib_accessor* grib_next_accessor(grib_accessor* a);
void grib_resize(grib_accessor* a, size_t new_size);
int grib_compare_accessors(grib_accessor* a1, grib_accessor* a2, int compare_flags);
int grib_accessor_add_attribute(grib_accessor* a, grib_accessor* attr, int nest_if_clash);
grib_accessor* grib_accessor_get_attribute_index(grib_accessor* a, const char* name, int* index);
int grib_accessor_has_attributes(grib_accessor* a);
grib_accessor* grib_accessor_get_attribute(grib_accessor* a, const char* name);
grib_accessors_list* grib_accessors_list_create(grib_context* c);
void grib_accessors_list_push(grib_accessors_list* al, grib_accessor* a, int rank);
grib_accessors_list* grib_accessors_list_last(grib_accessors_list* al);
void grib_accessors_list_delete(grib_context* c, grib_accessors_list* al);

/* grib_concept.cc */
grib_concept_value* grib_concept_value_new(grib_context* c, const char* name, grib_concept_condition* conditions);
void grib_concept_value_delete(grib_context* c, grib_concept_value* v);
grib_concept_condition* grib_concept_condition_new(grib_context* c, const char* name, grib_expression* expression, grib_iarray* iarray);
void grib_concept_condition_delete(grib_context* c, grib_concept_condition* v);

/* grib_hash_array.cc */
grib_hash_array_value* grib_integer_hash_array_value_new(grib_context* c, const char* name, grib_iarray* array);

/* grib_bufr_descriptor.cc */
bufr_descriptor* grib_bufr_descriptor_new(grib_accessor* tables_accessor, int code, int silent, int* err);
bufr_descriptor* grib_bufr_descriptor_clone(bufr_descriptor* d);
int grib_bufr_descriptor_set_code(bufr_descriptor* v, int code);
void grib_bufr_descriptor_set_scale(bufr_descriptor* v, long scale);
int grib_bufr_descriptor_can_be_missing(const bufr_descriptor* v);
void grib_bufr_descriptor_delete(bufr_descriptor* v);

/* grib_bufr_descriptors_array.cc */
bufr_descriptors_array* grib_bufr_descriptors_array_new(grib_context* c, size_t size, size_t incsize);
bufr_descriptor* grib_bufr_descriptors_array_pop_front(bufr_descriptors_array* a);
bufr_descriptors_array* grib_bufr_descriptors_array_push(bufr_descriptors_array* v, bufr_descriptor* val);
bufr_descriptors_array* grib_bufr_descriptors_array_append(bufr_descriptors_array* v, bufr_descriptors_array* ar);
bufr_descriptor* grib_bufr_descriptors_array_get(bufr_descriptors_array* a, size_t i);
void grib_bufr_descriptors_array_delete(bufr_descriptors_array* v);
void grib_bufr_descriptors_array_delete_array(bufr_descriptors_array* v);
size_t grib_bufr_descriptors_array_used_size(bufr_descriptors_array* v);

/* grib_darray.cc */
void grib_darray_print(const char* title, const grib_darray* darray);
grib_darray* grib_darray_new(grib_context* c, size_t size, size_t incsize);
grib_darray* grib_darray_push(grib_context* c, grib_darray* v, double val);
void grib_darray_delete(grib_context* c, grib_darray* v);
int grib_darray_is_constant(grib_darray* v, double epsilon);
size_t grib_darray_used_size(grib_darray* v);

/* grib_sarray.cc */
void grib_sarray_print(const char* title, const grib_sarray* sarray);
grib_sarray* grib_sarray_new(grib_context* c, size_t size, size_t incsize);
grib_sarray* grib_sarray_push(grib_context* c, grib_sarray* v, char* val);
void grib_sarray_delete(grib_context* c, grib_sarray* v);
void grib_sarray_delete_content(grib_context* c, grib_sarray* v);
char** grib_sarray_get_array(grib_context* c, grib_sarray* v);
size_t grib_sarray_used_size(grib_sarray* v);

/* grib_oarray.cc */
grib_oarray* grib_oarray_new(grib_context* c, size_t size, size_t incsize);
grib_oarray* grib_oarray_push(grib_context* c, grib_oarray* v, void* val);
void grib_oarray_delete(grib_context* c, grib_oarray* v);
void* grib_oarray_get(grib_oarray* v, int i);

/* grib_iarray.cc */
void grib_iarray_print(const char* title, const grib_iarray* iarray);
grib_iarray* grib_iarray_new(grib_context* c, size_t size, size_t incsize);
long grib_iarray_pop(grib_iarray* a);
long grib_iarray_pop_front(grib_iarray* a);
grib_iarray* grib_iarray_push(grib_iarray* v, long val);
void grib_iarray_delete(grib_iarray* v);
void grib_iarray_delete_array(grib_iarray* v);
long* grib_iarray_get_array(grib_iarray* v);
size_t grib_iarray_used_size(grib_iarray* v);

/* grib_vdarray.cc */
void grib_vdarray_print(const char* title, const grib_vdarray* vdarray);
grib_vdarray* grib_vdarray_new(grib_context* c, size_t size, size_t incsize);
grib_vdarray* grib_vdarray_push(grib_context* c, grib_vdarray* v, grib_darray* val);
void grib_vdarray_delete(grib_context* c, grib_vdarray* v);
void grib_vdarray_delete_content(grib_context* c, grib_vdarray* v);
size_t grib_vdarray_used_size(grib_vdarray* v);

/* grib_vsarray.cc */
void grib_vsarray_print(const char* title, const grib_vsarray* vdarray);
grib_vsarray* grib_vsarray_new(grib_context* c, size_t size, size_t incsize);
grib_vsarray* grib_vsarray_push(grib_context* c, grib_vsarray* v, grib_sarray* val);
void grib_vsarray_delete(grib_context* c, grib_vsarray* v);
void grib_vsarray_delete_content(grib_context* c, grib_vsarray* v);
size_t grib_vsarray_used_size(grib_vsarray* v);

/* grib_viarray.cc */
void grib_viarray_print(const char* title, const grib_viarray* viarray);
grib_viarray* grib_viarray_new(grib_context* c, size_t size, size_t incsize);
grib_viarray* grib_viarray_push(grib_context* c, grib_viarray* v, grib_iarray* val);
void grib_viarray_delete(grib_context* c, grib_viarray* v);
void grib_viarray_delete_content(grib_context* c, grib_viarray* v);

/* grib_accessor_class_bufr_data_array.cc */
grib_vsarray* accessor_bufr_data_array_get_stringValues(grib_accessor* a);
grib_accessors_list* accessor_bufr_data_array_get_dataAccessors(grib_accessor* a);
grib_trie_with_rank* accessor_bufr_data_array_get_dataAccessorsTrie(grib_accessor* a);
void accessor_bufr_data_array_set_unpackMode(grib_accessor* a, int unpackMode);

/* grib_accessor_class_bufr_data_element.cc */
void accessor_bufr_data_element_set_index(grib_accessor* a, long index);
void accessor_bufr_data_element_set_type(grib_accessor* a, int type);
void accessor_bufr_data_element_set_numberOfSubsets(grib_accessor* a, long numberOfSubsets);
void accessor_bufr_data_element_set_subsetNumber(grib_accessor* a, long subsetNumber);
void accessor_bufr_data_element_set_compressedData(grib_accessor* a, int compressedData);
void accessor_bufr_data_element_set_descriptors(grib_accessor* a, bufr_descriptors_array* descriptors);
void accessor_bufr_data_element_set_numericValues(grib_accessor* a, grib_vdarray* numericValues);
void accessor_bufr_data_element_set_stringValues(grib_accessor* a, grib_vsarray* stringValues);
void accessor_bufr_data_element_set_elementsDescriptorsIndex(grib_accessor* a, grib_viarray* elementsDescriptorsIndex);

/* grib_accessor_class_bufr_elements_table.cc */
int bufr_descriptor_is_marker(bufr_descriptor* d);
bufr_descriptor* accessor_bufr_elements_table_get_descriptor(grib_accessor* a, int code, int* err);

/* grib_accessor_class.cc */
grib_section* grib_create_root_section(const grib_context* context, grib_handle* h);
grib_accessor* grib_accessor_factory(grib_section* p, grib_action* creator, const long len, grib_arguments* params);
void grib_push_accessor(grib_accessor* a, grib_block_of_accessors* l);
void grib_section_post_init(grib_section* s);
int grib_section_adjust_sizes(grib_section* s, int update, int depth);
int grib_get_block_length(grib_section* s, size_t* l);
grib_accessor* find_paddings(grib_section* s);
void grib_update_paddings(grib_section* s);

/* grib_accessor_class_smart_table.cc */
void grib_smart_table_delete(grib_context* c);

/* grib_accessor_class_codetable.cc */
void grib_codetable_delete(grib_context* c);
int codes_codetable_get_contents_malloc(const grib_handle* h, const char* key, code_table_entry** entries, size_t* num_entries);
int codes_codetable_check_code_figure(const grib_handle* h, const char* key, long code_figure);
int codes_codetable_check_abbreviation(const grib_handle* h, const char* key, const char* abbreviation);

/* grib_accessor_class_g1step_range.cc */
int grib_g1_step_get_steps(grib_accessor* a, long* start, long* theEnd);

/* grib_accessor_class_iterator.cc */
grib_iterator* grib_iterator_new(const grib_handle* h, unsigned long flags, int* error);

/* grib_accessor_class_nearest.cc */
grib_nearest* grib_nearest_new(const grib_handle* h, int* error);

/* grib_accessor_class_g1_message_length.cc */
int grib_get_g1_message_size(grib_handle* h, grib_accessor* tl, grib_accessor* s4, long* total_length, long* sec4_len);

/* grib_accessor_class_variable.cc */
void accessor_variable_set_type(grib_accessor* a, int type);

/* grib_accessor_class_expanded_descriptors.cc */
int grib_accessor_class_expanded_descriptors_set_do_expand(grib_accessor* a, long do_expand);
bufr_descriptors_array* grib_accessor_class_expanded_descriptors_get_expanded(grib_accessor* a, int* err);

/* grib_jasper_encoding.cc */
int grib_jasper_decode(grib_context* c, unsigned char* buf, const size_t* buflen, double* values, const size_t* n_vals);
int grib_jasper_encode(grib_context* c, j2k_encode_helper* helper);

/* grib_openjpeg_encoding.cc */
int grib_openjpeg_decode(grib_context* c, unsigned char* buf, const size_t* buflen, double* values, const size_t* n_vals);
int grib_openjpeg_encode(grib_context* c, j2k_encode_helper* helper);

/* action_class_set_missing.cc */
grib_action* grib_action_create_set_missing(grib_context* context, const char* name);

/* grib_index.cc */
int grib_index_compress(grib_index* index);
grib_field_tree* grib_read_field_tree(grib_context* c, FILE* fh, grib_file** files, int* err);
grib_index* grib_index_new(grib_context* c, const char* key, int* err);
void grib_index_delete(grib_index* index);
int grib_index_write(grib_index* index, const char* filename);
grib_index* grib_index_read(grib_context* c, const char* filename, int* err);
int grib_index_search_same(grib_index* index, grib_handle* h);
int grib_index_add_file(grib_index* index, const char* filename);
grib_index* grib_index_new_from_file(grib_context* c, const char* filename, const char* keys, int* err);
int grib_index_get_size(const grib_index* index, const char* key, size_t* size);
int grib_index_get_string(const grib_index* index, const char* key, char** values, size_t* size);
int grib_index_get_long(const grib_index* index, const char* key, long* values, size_t* size);
int grib_index_get_double(const grib_index* index, const char* key, double* values, size_t* size);
int grib_index_select_long(grib_index* index, const char* skey, long value);
int grib_index_select_double(grib_index* index, const char* skey, double value);
int grib_index_select_string(grib_index* index, const char* skey, const char* value);
grib_handle* codes_index_get_handle(grib_field* field, int message_type, int* err);
int grib_index_dump_file(FILE* fout, const char* filename, unsigned long flags);
void grib_index_dump(FILE* fout, grib_index* index, unsigned long flags);
char* grib_get_field_file(grib_index* index, off_t* offset);
grib_handle* grib_handle_new_from_index(grib_index* index, int* err);
grib_handle* codes_new_from_index(grib_index* index, int message_type, int* err);
int codes_index_set_product_kind(grib_index* index, ProductKind product_kind);
int codes_index_set_unpack_bufr(grib_index* index, int unpack);
int is_index_file(const char* filename);

/* grib_accessor_class_unsigned.cc */
int pack_long_unsigned_helper(grib_accessor* a, const long* val, size_t* len, int check);

/* grib_accessor_class_raw.cc */
void accessor_raw_set_length(grib_accessor* a, size_t len);
long accessor_raw_get_offset(grib_accessor* a);

/* grib_gaussian_reduced.cc */
void grib_get_reduced_row_wrapper(grib_handle* h, long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last);
void grib_get_reduced_row_legacy(long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last);
void grib_get_reduced_row(long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last);
void grib_get_reduced_row_p(long pl, double lon_first, double lon_last, long* npoints, double* olon_first, double* olon_last);

/* grib_loader_from_handle.cc */
int grib_lookup_long_from_handle(grib_context* gc, grib_loader* loader, const char* name, long* value);
int grib_init_accessor_from_handle(grib_loader* loader, grib_accessor* ga, grib_arguments* default_value);

/* grib_bits.cc */
unsigned long grib_decode_unsigned_byte_long(const unsigned char* p, long o, int l);
long grib_decode_signed_long(const unsigned char* p, long o, int l);
int grib_encode_signed_long(unsigned char* p, long val, long o, int l);
void grib_set_bits_on(unsigned char* p, long* bitp, long nbits);
int grib_get_bit(const unsigned char* p, long bitp);
void grib_set_bit(unsigned char* p, long bitp, int val);
long grib_decode_signed_longb(const unsigned char* p, long* bitp, long nbits);
int grib_encode_signed_longb(unsigned char* p, long val, long* bitp, long nb);

/* grib_timer.cc */
double proc_cpu(void);
char* timename(double t);
grib_timer* grib_get_timer(grib_context* c, const char* name, const char* statname, int elapsed);
int grib_timer_start(grib_timer* t);
int grib_timer_stop(grib_timer* t, long total);
double grib_timer_value(grib_timer* t);
const char* bytename(double bytes);
void grib_timer_print(grib_timer* t);
void grib_timer_partial_rate(grib_timer* t, double start, long total);
void grib_print_all_timers(void);
void grib_reset_all_timers(void);

/* grib_ibmfloat.cc */
unsigned long grib_ibm_to_long(double x);
double grib_ibmfloat_error(double x);
double grib_long_to_ibm(unsigned long x);
double grib_ibm_table_e(unsigned long e);
double grib_ibm_table_v(unsigned long e);
unsigned long grib_ibm_nearest_smaller_to_long(double x);
int grib_nearest_smaller_ibm_float(double a, double* ret);

/* grib_ieeefloat.cc */
unsigned long grib_ieee_to_long(double x);
double grib_ieeefloat_error(double x);
double grib_long_to_ieee(unsigned long x);
unsigned long grib_ieee_nearest_smaller_to_long(double x);
int grib_nearest_smaller_ieee_float(double a, double* ret);

unsigned long grib_ieee64_to_long(double x);
double grib_long_to_ieee64(unsigned long x);
// ECC-1467
//int grib_ieee_decode_array(grib_context* c, unsigned char* buf, size_t nvals, int bytes, double* val);
//int grib_ieee_decode_array_float(grib_context* c, unsigned char* buf, size_t nvals, int bytes, float* val);
int grib_ieee_encode_array(grib_context* c, double* val, size_t nvals, int bytes, unsigned char* buf);

/* grib_memory.cc */
void* grib_transient_malloc(const grib_context* c, size_t s);
void* grib_transient_realloc(const grib_context* c, void* p, size_t s);
void grib_transient_free(const grib_context* c, void* p);
void* grib_permanent_malloc(const grib_context* c, size_t s);
void* grib_permanent_realloc(const grib_context* c, void* p, size_t s);
void grib_permanent_free(const grib_context* c, void* p);
void* grib_buffer_malloc(const grib_context* c, size_t s);
void grib_buffer_free(const grib_context* c, void* p);
void* grib_buffer_realloc(const grib_context* c, void* p, size_t s);

/* grib_buffer.cc */
grib_buffer* grib_create_growable_buffer(const grib_context* c);
grib_buffer* grib_new_buffer(const grib_context* c, const unsigned char* data, size_t buflen);
void grib_buffer_delete(const grib_context* c, grib_buffer* b);
void grib_grow_buffer(const grib_context* c, grib_buffer* b, size_t new_size);
void grib_buffer_set_ulength_bits(const grib_context* c, grib_buffer* b, size_t length_bits);
void grib_buffer_set_ulength(const grib_context* c, grib_buffer* b, size_t length);
void grib_recompute_sections_lengths(grib_section* s);
int grib_buffer_replace(grib_accessor* a, const unsigned char* data, size_t newsize, int update_lengths, int update_paddings);
void grib_update_sections_lengths(grib_handle* h);

/* grib_dumper.cc */
void grib_init_dumper(grib_dumper* d);
void grib_dumper_delete(grib_dumper* d);
void grib_dump_long(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_double(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_string(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_string_array(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_label(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_bytes(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_bits(grib_dumper* d, grib_accessor* a, const char* comment);
void grib_dump_section(grib_dumper* d, grib_accessor* a, grib_block_of_accessors* block);
void grib_dump_values(grib_dumper* d, grib_accessor* a);
void grib_dump_header(grib_dumper* d, const grib_handle* h);
void grib_dump_footer(grib_dumper* d, const grib_handle* h);

/* grib_dumper_class.cc */
grib_dumper* grib_dumper_factory(const char* op, const grib_handle* h, FILE* out, unsigned long option_flags, void* arg);
void grib_dump_accessors_block(grib_dumper* dumper, grib_block_of_accessors* block);
void grib_dump_accessors_list(grib_dumper* dumper, grib_accessors_list* al);
int grib_print(grib_handle* h, const char* name, grib_dumper* d);
void grib_dump_content(const grib_handle* h, FILE* f, const char* mode, unsigned long flags, void* data);
void grib_dump_keys(grib_handle* h, FILE* f, const char* mode, unsigned long flags, void* data, const char** keys, size_t num_keys);
grib_dumper* grib_dump_content_with_dumper(grib_handle* h, grib_dumper* dumper, FILE* f, const char* mode, unsigned long flags, void* data);
void codes_dump_bufr_flat(grib_accessors_list* al, grib_handle* h, FILE* f, const char* mode, unsigned long flags, void* data);

/* grib_context.cc */
size_t grib_context_read(const grib_context* c, void* ptr, size_t size, void* stream);
off_t grib_context_tell(const grib_context* c, void* stream);
int grib_context_seek(const grib_context* c, off_t offset, int whence, void* stream);
int grib_context_eof(const grib_context* c, void* stream);
size_t grib_context_write(const grib_context* c, const void* ptr, size_t size, void* stream);
void grib_context_set_print_proc(grib_context* c, grib_print_proc p);
void grib_context_set_debug(grib_context* c, int mode);
void grib_context_set_logging_proc(grib_context* c, grib_log_proc p);
long grib_get_api_version(void);
void grib_print_api_version(FILE* out);
const char* grib_get_package_name(void);
grib_context* grib_context_get_default(void);
char* codes_resolve_path(grib_context* c, const char* path);
char* grib_context_full_defs_path(grib_context* c, const char* basename);
char* grib_samples_path(const grib_context* c);
char* grib_definition_path(const grib_context* c);
void grib_context_free(const grib_context* c, void* p);
void grib_context_free_persistent(const grib_context* c, void* p);
void grib_context_reset(grib_context* c);
void grib_context_delete(grib_context* c);
void codes_bufr_multi_element_constant_arrays_on(grib_context* c);
void codes_bufr_multi_element_constant_arrays_off(grib_context* c);
void grib_context_set_definitions_path(grib_context* c, const char* path);
void grib_context_set_samples_path(grib_context* c, const char* path);
void* grib_context_malloc_persistent(const grib_context* c, size_t size);
char* grib_context_strdup_persistent(const grib_context* c, const char* s);
void* grib_context_malloc_clear_persistent(const grib_context* c, size_t size);
void* grib_context_malloc(const grib_context* c, size_t size);
void* grib_context_realloc(const grib_context* c, void* p, size_t size);
char* grib_context_strdup(const grib_context* c, const char* s);
void* grib_context_malloc_clear(const grib_context* c, size_t size);
void* grib_context_buffer_malloc(const grib_context* c, size_t size);
void grib_context_buffer_free(const grib_context* c, void* p);
void* grib_context_buffer_realloc(const grib_context* c, void* p, size_t size);
void* grib_context_buffer_malloc_clear(const grib_context* c, size_t size);
void grib_context_set_memory_proc(grib_context* c, grib_malloc_proc m, grib_free_proc f, grib_realloc_proc r);
void grib_context_set_persistent_memory_proc(grib_context* c, grib_malloc_proc m, grib_free_proc f);
void grib_context_set_buffer_memory_proc(grib_context* c, grib_malloc_proc m, grib_free_proc f, grib_realloc_proc r);
void grib_context_set_data_accessing_proc(grib_context* c, grib_data_read_proc read, grib_data_write_proc write, grib_data_tell_proc tell);
void grib_context_log(const grib_context* c, int level, const char* fmt, ...);
void grib_context_print(const grib_context* c, void* descriptor, const char* fmt, ...);
int grib_context_get_handle_file_count(grib_context* c);
int grib_context_get_handle_total_count(grib_context* c);
void grib_context_set_handle_file_count(grib_context* c, int new_count);
void grib_context_set_handle_total_count(grib_context* c, int new_count);
void grib_context_increment_handle_file_count(grib_context* c);
void grib_context_increment_handle_total_count(grib_context* c);
bufr_descriptors_array* grib_context_expanded_descriptors_list_get(grib_context* c, const char* key, long* u, size_t size);
void grib_context_expanded_descriptors_list_push(grib_context* c, const char* key, bufr_descriptors_array* expanded, bufr_descriptors_array* unexpanded);
void codes_set_codes_assertion_failed_proc(codes_assertion_failed_proc proc);
void codes_assertion_failed(const char* message, const char* file, int line);
int grib_get_gribex_mode(grib_context* c);
void grib_gribex_mode_on(grib_context* c);
void grib_gribex_mode_off(grib_context* c);
void grib_gts_header_on(grib_context* c);
void grib_gts_header_off(grib_context* c);
void grib_multi_support_on(grib_context* c);
void grib_multi_support_off(grib_context* c);

/* grib_date.cc */
int grib_julian_to_datetime(double jd, long* year, long* month, long* day, long* hour, long* minute, long* second);
int grib_datetime_to_julian(long year, long month, long day, long hour, long minute, long second, double* jd);
int grib_datetime_to_julian_d(long year, long month, long day, long hour, long minute, double second, double* jd);
long grib_julian_to_date(long jdate);
long grib_date_to_julian(long ddate);

/* grib_fieldset.cc */
int grib_fieldset_new_column(grib_fieldset* set, int id, char* key, int type);
grib_fieldset* grib_fieldset_new_from_files(grib_context* c, const char* filenames[], int nfiles, const char** keys, int nkeys, const char* where_string, const char* order_by_string, int* err);
int grib_fieldset_apply_where(grib_fieldset* set, const char* where_string);
int grib_fieldset_apply_order_by(grib_fieldset* set, const char* order_by_string);
void grib_fieldset_delete_order_by(grib_context* c, grib_order_by* order_by);
void grib_fieldset_delete(grib_fieldset* set);
int grib_fieldset_add(grib_fieldset* set, const char* filename);
void grib_fieldset_rewind(grib_fieldset* set);
grib_handle* grib_fieldset_next_handle(grib_fieldset* set, int* err);
int grib_fieldset_count(const grib_fieldset* set);
grib_handle* grib_fieldset_retrieve(grib_fieldset* set, int i, int* err);

/* grib_filepool.cc */
void grib_file_pool_clean(void);
grib_file* grib_file_open(const char* filename, const char* mode, int* err);
void grib_file_pool_delete_file(grib_file* file);
void grib_file_close(const char* filename, int force, int* err);
void grib_file_close_all(int* err);
grib_file* grib_get_file(const char* filename, int* err);
void grib_file_delete(grib_file* file);

/* grib_geography.cc */
int grib_get_gaussian_latitudes(long trunc, double* lats);
int is_gaussian_global(double lat1, double lat2, double lon1, double lon2, long num_points_equator, const double* latitudes, double angular_precision);
void rotate(const double inlat, const double inlon, const double angleOfRot, const double southPoleLat, const double southPoleLon, double* outlat, double* outlon);
void unrotate(const double inlat, const double inlon, const double angleOfRot, const double southPoleLat, const double southPoleLon, double* outlat, double* outlon);
double geographic_distance_spherical(double radius, double lon1, double lat1, double lon2, double lat2);

/* grib_handle.cc */
grib_section* grib_section_create(grib_handle* h, grib_accessor* owner);
void grib_swap_sections(grib_section* the_old, grib_section* the_new);
void grib_empty_section(grib_context* c, grib_section* b);
void grib_section_delete(grib_context* c, grib_section* b);
int grib_handle_delete(grib_handle* h);
grib_handle* grib_new_handle(grib_context* c);
grib_handle* codes_handle_new_from_samples(grib_context* c, const char* name);
grib_handle* grib_handle_new_from_samples(grib_context* c, const char* name);
grib_handle* codes_bufr_handle_new_from_samples(grib_context* c, const char* name);
int grib_write_message(const grib_handle* h, const char* file, const char* mode);
grib_handle* grib_handle_clone(const grib_handle* h);
grib_handle* codes_handle_new_from_file(grib_context* c, FILE* f, ProductKind product, int* error);
grib_handle* codes_grib_handle_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* codes_bufr_handle_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* codes_metar_handle_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* codes_gts_handle_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* grib_handle_new_from_message_copy(grib_context* c, const void* data, size_t size);
grib_handle* grib_handle_new_from_partial_message_copy(grib_context* c, const void* data, size_t size);
grib_handle* grib_handle_new_from_partial_message(grib_context* c, const void* data, size_t buflen);
grib_handle* grib_handle_new_from_message(grib_context* c, const void* data, size_t buflen);
grib_handle* grib_handle_new_from_multi_message(grib_context* c, void** data, size_t* buflen, int* error);
grib_handle* grib_handle_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* grib_new_from_file(grib_context* c, FILE* f, int headers_only, int* error);
grib_handle* gts_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* taf_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* metar_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* bufr_new_from_file(grib_context* c, FILE* f, int* error);
grib_handle* any_new_from_file(grib_context* c, FILE* f, int* error);
grib_multi_handle* grib_multi_handle_new(grib_context* c);
int grib_multi_handle_delete(grib_multi_handle* h);
int grib_multi_handle_append(grib_handle* h, int start_section, grib_multi_handle* mh);
int grib_multi_handle_write(grib_multi_handle* h, FILE* f);
int grib_get_partial_message(grib_handle* h, const void** msg, size_t* len, int start_section);
int grib_get_partial_message_copy(grib_handle* h, void* message, size_t* len, int start_section);
int grib_get_message_copy(const grib_handle* h, void* message, size_t* len);
int grib_get_message_offset(const grib_handle* h, off_t* offset);
int codes_get_product_kind(const grib_handle* h, ProductKind* product_kind);
int codes_check_message_header(const void* bytes, size_t length, ProductKind product);
int codes_check_message_footer(const void* bytes, size_t length, ProductKind product);
int grib_get_message_size(const grib_handle* h, size_t* size);
int grib_get_message(const grib_handle* h, const void** msg, size_t* size);
int grib_get_message_headers(const grib_handle* h, const void** msg, size_t* size);
grib_action* grib_action_from_filter(const char* filter);
int grib_handle_apply_action(grib_handle* h, grib_action* a);
void grib_multi_support_reset_file(grib_context* c, FILE* f);
void grib_multi_support_reset(grib_context* c);

/* grib_hash_keys.cc */
const struct grib_keys_hash* grib_keys_hash_get(const char* str, size_t len);
grib_itrie* grib_hash_keys_new(grib_context* c, int* count);
void grib_hash_keys_delete(grib_itrie* t);
int grib_hash_keys_get_id(grib_itrie* t, const char* key);
int grib_hash_keys_get_size(grib_itrie* t);

/* grib_io.cc */
off_t stdio_tell(void* data);
int stdio_seek(void* data, off_t len);
int stdio_seek_from_start(void* data, off_t len);
size_t stdio_read(void* data, void* buf, size_t len, int* err);
int wmo_read_any_from_file(FILE* f, void* buffer, size_t* len);
int wmo_read_grib_from_file(FILE* f, void* buffer, size_t* len);
int wmo_read_bufr_from_file(FILE* f, void* buffer, size_t* len);
int wmo_read_gts_from_file(FILE* f, void* buffer, size_t* len);
int wmo_read_any_from_file_fast(FILE* f, size_t* msg_len, off_t* msg_offset);
int wmo_read_grib_from_file_fast(FILE* f, size_t* msg_len, off_t* msg_offset);
int wmo_read_bufr_from_file_fast(FILE* f, size_t* msg_len, off_t* msg_offset);
int wmo_read_gts_from_file_fast(FILE* f, size_t* msg_len, off_t* msg_offset);
int wmo_read_any_from_stream(void* stream_data, long (*stream_proc)(void*, void* buffer, long len), void* buffer, size_t* len);
void* wmo_read_any_from_stream_malloc(void* stream_data, long (*stream_proc)(void*, void* buffer, long len), size_t* size, int* err);
void* wmo_read_gts_from_file_malloc(FILE* f, int headers_only, size_t* size, off_t* offset, int* err);
void* wmo_read_taf_from_file_malloc(FILE* f, int headers_only, size_t* size, off_t* offset, int* err);
void* wmo_read_metar_from_file_malloc(FILE* f, int headers_only, size_t* size, off_t* offset, int* err);
void* wmo_read_any_from_file_malloc(FILE* f, int headers_only, size_t* size, off_t* offset, int* err);
void* wmo_read_grib_from_file_malloc(FILE* f, int headers_only, size_t* size, off_t* offset, int* err);
void* wmo_read_bufr_from_file_malloc(FILE* f, int headers_only, size_t* size, off_t* offset, int* err);
int grib_read_any_headers_only_from_file(grib_context* ctx, FILE* f, void* buffer, size_t* len);
int grib_read_any_from_file(grib_context* ctx, FILE* f, void* buffer, size_t* len);
int grib_read_any_from_memory_alloc(grib_context* ctx, unsigned char** data, size_t* data_length, void** buffer, size_t* length);
int grib_read_any_from_memory(grib_context* ctx, unsigned char** data, size_t* data_length, void* buffer, size_t* len);
int grib_count_in_file(grib_context* c, FILE* f, int* n);
int grib_count_in_filename(grib_context* c, const char* filename, int* n);
int codes_extract_offsets_malloc(grib_context* c, const char* filename, ProductKind product, off_t** offsets, int* num_messages, int strict_mode);
int codes_extract_offsets_sizes_malloc(grib_context* c, const char* filename, ProductKind product,
                                       off_t** offsets, size_t** sizes, int* num_messages, int strict_mode);


/* grib_trie.cc */
grib_trie* grib_trie_new(grib_context* c);
void grib_trie_delete_container(grib_trie* t);
void grib_trie_delete(grib_trie* t);
void grib_trie_clear(grib_trie* t);
void* grib_trie_insert(grib_trie* t, const char* key, void* data);
void* grib_trie_insert_no_replace(grib_trie* t, const char* key, void* data);
void* grib_trie_get(grib_trie* t, const char* key);

/* grib_trie_with_rank.cc */
grib_trie_with_rank* grib_trie_with_rank_new(grib_context* c);
void grib_trie_with_rank_delete_container(grib_trie_with_rank* t);
int grib_trie_with_rank_insert(grib_trie_with_rank* t, const char* key, void* data);
void* grib_trie_with_rank_get(grib_trie_with_rank* t, const char* key, int rank);

/* grib_itrie.cc */
grib_itrie* grib_itrie_new(grib_context* c, int* count);
void grib_itrie_delete(grib_itrie* t);
int grib_itrie_get_id(grib_itrie* t, const char* key);
int grib_itrie_insert(grib_itrie* t, const char* key);
int grib_itrie_get_size(grib_itrie* t);

/* grib_rules.cc */
grib_rule_entry* grib_new_rule_entry(grib_context* c, const char* name, grib_expression* expression);
grib_rule* grib_new_rule(grib_context* c, grib_expression* condition, grib_rule_entry* entries);

/* grib_keys_iterator.cc */
grib_keys_iterator* grib_keys_iterator_new(grib_handle* h, unsigned long filter_flags, const char* name_space);
int grib_keys_iterator_set_flags(grib_keys_iterator* ki, unsigned long flags);
int grib_keys_iterator_rewind(grib_keys_iterator* ki);
int grib_keys_iterator_next(grib_keys_iterator* kiter);
const char* grib_keys_iterator_get_name(const grib_keys_iterator* kiter);
grib_accessor* grib_keys_iterator_get_accessor(grib_keys_iterator* kiter);
int grib_keys_iterator_delete(grib_keys_iterator* kiter);
int grib_keys_iterator_get_long(const grib_keys_iterator* kiter, long* v, size_t* len);
int grib_keys_iterator_get_double(const grib_keys_iterator* kiter, double* v, size_t* len);
int grib_keys_iterator_get_float(const grib_keys_iterator* kiter, float* v, size_t* len);
int grib_keys_iterator_get_string(const grib_keys_iterator* kiter, char* v, size_t* len);
int grib_keys_iterator_get_bytes(const grib_keys_iterator* kiter, unsigned char* v, size_t* len);
int grib_keys_iterator_get_native_type(const grib_keys_iterator* kiter);

/* bufr_keys_iterator.cc */
bufr_keys_iterator* codes_bufr_keys_iterator_new(grib_handle* h, unsigned long filter_flags);
bufr_keys_iterator* codes_bufr_data_section_keys_iterator_new(grib_handle* h);
int codes_bufr_keys_iterator_rewind(bufr_keys_iterator* ki);
int codes_bufr_keys_iterator_next(bufr_keys_iterator* kiter);
char* codes_bufr_keys_iterator_get_name(const bufr_keys_iterator* kiter);
grib_accessor* codes_bufr_keys_iterator_get_accessor(bufr_keys_iterator* kiter);
int codes_bufr_keys_iterator_delete(bufr_keys_iterator* kiter);

/* grib_parse_utils.cc */
int grib_recompose_name(grib_handle* h, grib_accessor* observer, const char* uname, char* fname, int fail);
int grib_accessors_list_print(grib_handle* h, grib_accessors_list* al, const char* name, int type, const char* format, const char* separator, int maxcols, int* newline, FILE* out);
int grib_recompose_print(grib_handle* h, grib_accessor* observer, const char* uname, int fail, FILE* out);
grib_action_file* grib_find_action_file(const char* fname, grib_action_file_list* afl);
int grib_yywrap(void);
char* file_being_parsed(void);
int grib_yyerror(const char* msg);
void grib_parser_include(const char* included_fname);
grib_concept_value* grib_parse_concept_file(grib_context* gc, const char* filename);
grib_hash_array_value* grib_parse_hash_array_file(grib_context* gc, const char* filename);
grib_action* grib_parse_file(grib_context* gc, const char* filename);
int grib_type_to_int(char id);

/* grib_query.cc */
grib_accessors_list* grib_find_accessors_list(const grib_handle* h, const char* name);
char* grib_split_name_attribute(grib_context* c, const char* name, char* attribute_name);
grib_accessor* grib_find_accessor(const grib_handle* h, const char* name);
grib_accessor* grib_find_accessor_fast(grib_handle* h, const char* name);

/* grib_scaling.cc */
double grib_power(long s, long n);
long grib_get_binary_scale_fact(double max, double min, long bpval, int* error);

/* grib_templates.cc */
grib_handle* codes_external_sample(grib_context* c, ProductKind product_kind, const char* name);
char* get_external_sample_path(grib_context* c, const char* name);

/* grib_dependency.cc */
grib_handle* grib_handle_of_accessor(const grib_accessor* a);
void grib_dependency_add(grib_accessor* observer, grib_accessor* observed);
void grib_dependency_remove_observed(grib_accessor* observed);
int grib_dependency_notify_change_h(grib_handle* h, grib_accessor* observed);
int grib_dependency_notify_change(grib_accessor* observed);
void grib_dependency_remove_observer(grib_accessor* observer);
void grib_dependency_observe_expression(grib_accessor* observer, grib_expression* e);
void grib_dependency_observe_arguments(grib_accessor* observer, grib_arguments* a);

/* grib_value.cc */
int grib_set_expression(grib_handle* h, const char* name, grib_expression* e);
int grib_set_long_internal(grib_handle* h, const char* name, long val);
int grib_set_long(grib_handle* h, const char* name, long val);
int grib_set_double_internal(grib_handle* h, const char* name, double val);
int grib_copy_namespace(grib_handle* dest, const char* name, grib_handle* src);
int grib_set_double(grib_handle* h, const char* name, double val);
int grib_set_string_internal(grib_handle* h, const char* name, const char* val, size_t* length);
int grib_set_string(grib_handle* h, const char* name, const char* val, size_t* length);
int grib_set_string_array(grib_handle* h, const char* name, const char** val, size_t length);
int grib_set_bytes(grib_handle* h, const char* name, const unsigned char* val, size_t* length);
int grib_set_missing(grib_handle* h, const char* name);
int grib_is_missing_long(grib_accessor* a, long x);
int grib_is_missing_double(grib_accessor* a, double x);
int grib_is_missing_string(grib_accessor* a, const unsigned char* x, size_t len);
int grib_accessor_can_be_missing(grib_accessor* a, int* err);
int grib_accessor_is_missing(grib_accessor* a, int* err);
int grib_is_missing(const grib_handle* h, const char* name, int* err);
int grib_is_defined(const grib_handle* h, const char* name);
int grib_set_flag(grib_handle* h, const char* name, unsigned long flag);
int grib_set_double_array_internal(grib_handle* h, const char* name, const double* val, size_t length);
int grib_set_force_double_array(grib_handle* h, const char* name, const double* val, size_t length);
int grib_set_double_array(grib_handle* h, const char* name, const double* val, size_t length);
int grib_set_float_array_internal(grib_handle* h, const char* name, const float* val, size_t length);
int grib_set_force_float_array(grib_handle* h, const char* name, const float* val, size_t length);
int grib_set_float_array(grib_handle* h, const char* name, const float* val, size_t length);
int grib_set_long_array_internal(grib_handle* h, const char* name, const long* val, size_t length);
int grib_set_long_array(grib_handle* h, const char* name, const long* val, size_t length);
int grib_get_long_internal(grib_handle* h, const char* name, long* val);
int grib_get_long(const grib_handle* h, const char* name, long* val);
int grib_get_double_internal(grib_handle* h, const char* name, double* val);
int grib_get_double(const grib_handle* h, const char* name, double* val);
int grib_get_double_element_internal(grib_handle* h, const char* name, int i, double* val);
int grib_get_double_element(const grib_handle* h, const char* name, int i, double* val);
int grib_get_float_element(const grib_handle* h, const char* name, int i, float* val);

int grib_get_double_element_set_internal(grib_handle* h, const char* name, const size_t* index_array, size_t len, double* val_array);
int grib_get_double_element_set(const grib_handle* h, const char* name, const size_t* index_array, size_t len, double* val_array);
int grib_get_float_element_set_internal(grib_handle* h, const char* name, const size_t* index_array, size_t len, float* val_array);
int grib_get_float_element_set(const grib_handle* h, const char* name, const size_t* index_array, size_t len, float* val_array);

int grib_get_double_elements(const grib_handle* h, const char* name, const int* index_array, long len, double* val_array);
int grib_get_float_elements(const grib_handle* h, const char* name, const int* index_array, long len, float* val_array);
int grib_get_string_internal(grib_handle* h, const char* name, char* val, size_t* length);
int grib_get_string(const grib_handle* h, const char* name, char* val, size_t* length);
int grib_get_bytes(const grib_handle* h, const char* name, unsigned char* val, size_t* length);
int grib_get_native_type(const grib_handle* h, const char* name, int* type);

int grib_get_double_array_internal(const grib_handle* h, const char* name, double* val, size_t* length);
int grib_get_double_array(const grib_handle* h, const char* name, double* val, size_t* length);
int grib_get_float_array_internal(const grib_handle* h, const char* name, float* val, size_t* length);
int grib_get_float_array(const grib_handle* h, const char* name, float* val, size_t* length);

int grib_get_string_length_acc(grib_accessor* a, size_t* size);
int grib_get_string_length(const grib_handle* h, const char* name, size_t* size);
int grib_get_size_acc(const grib_handle* h, grib_accessor* a, size_t* size);
int grib_get_size(const grib_handle* h, const char* name, size_t* size);
int grib_get_length(const grib_handle* h, const char* name, size_t* length);
int grib_get_offset(const grib_handle* h, const char* key, size_t* val);
int grib_get_string_array(const grib_handle* h, const char* name, char** val, size_t* length);
int grib_get_long_array_internal(grib_handle* h, const char* name, long* val, size_t* length);
int grib_get_long_array(const grib_handle* h, const char* name, long* val, size_t* length);
int grib_set_values(grib_handle* h, grib_values* args, size_t count);
int grib_get_nearest_smaller_value(grib_handle* h, const char* name, double val, double* nearest);
void grib_print_values(const char* title, grib_values* values, FILE* out);
int grib_values_check(grib_handle* h, grib_values* values, int count);
int codes_copy_key(grib_handle* h1, grib_handle* h2, const char* key, int type);
int codes_compare_key(grib_handle* h1, grib_handle* h2, const char* key, int compare_flags);

/* grib_errors.cc */
const char* grib_get_error_message(int code);
void grib_check(const char* call, const char* file, int line, int e, const char* msg);

/* grib_expression_class_binop.cc */
grib_expression* new_binop_expression(grib_context* c, grib_binop_long_proc long_func, grib_binop_double_proc double_func, grib_expression* left, grib_expression* right);

/* grib_expression_class_logical_and.cc */
grib_expression* new_logical_and_expression(grib_context* c, grib_expression* left, grib_expression* right);

/* grib_expression_class_logical_or.cc */
grib_expression* new_logical_or_expression(grib_context* c, grib_expression* left, grib_expression* right);

/* grib_expression_class_is_in_dict.cc */
grib_expression* new_is_in_dict_expression(grib_context* c, const char* name, const char* list);

/* grib_expression_class_true.cc */
grib_expression* new_true_expression(grib_context* c);

/* grib_expression_class_string_compare.cc */
grib_expression* new_string_compare_expression(grib_context* c, grib_expression* left, grib_expression* right);

/* grib_expression_class_unop.cc */
grib_expression* new_unop_expression(grib_context* c, grib_unop_long_proc long_func, grib_unop_double_proc double_func, grib_expression* exp);

/* grib_expression_class_functor.cc */
grib_expression* new_func_expression(grib_context* c, const char* name, grib_arguments* args);

/* grib_expression_class_accessor.cc */
grib_expression* new_accessor_expression(grib_context* c, const char* name, long start, size_t length);

/* grib_expression_class_is_in_list.cc */
grib_expression* new_is_in_list_expression(grib_context* c, const char* name, const char* list);

/* grib_expression_class_is_integer.cc */
grib_expression* new_is_integer_expression(grib_context* c, const char* name, int start, int length);

/* grib_expression_class_length.cc */
grib_expression* new_length_expression(grib_context* c, const char* name);

/* grib_expression_class_long.cc */
grib_expression* new_long_expression(grib_context* c, long value);

/* grib_expression_class_double.cc */
grib_expression* new_double_expression(grib_context* c, double value);

/* grib_expression_class_string.cc */
grib_expression* new_string_expression(grib_context* c, const char* value);

/* grib_expression_class_sub_string.cc */
grib_expression* new_sub_string_expression(grib_context* c, const char* value, size_t start, size_t length);

/* grib_nearest.cc */
int grib_nearest_find(grib_nearest* nearest, const grib_handle* h, double inlat, double inlon, unsigned long flags, double* outlats, double* outlons, double* values, double* distances, int* indexes, size_t* len);
int grib_nearest_init(grib_nearest* i, grib_handle* h, grib_arguments* args);
int grib_nearest_delete(grib_nearest* i);
int grib_nearest_get_radius(grib_handle* h, double* radiusInKm);
void grib_binary_search(const double xx[], const size_t n, double x, size_t* ju, size_t* jl);
int grib_nearest_find_multiple(const grib_handle* h, int is_lsm, const double* inlats, const double* inlons, long npoints, double* outlats, double* outlons, double* values, double* distances, int* indexes);
int grib_nearest_find_generic(grib_nearest* nearest, grib_handle* h, double inlat, double inlon, unsigned long flags,
                              const char* values_keyname,
                              double** out_lats,
                              int* out_lats_count,
                              double** out_lons,
                              int* out_lons_count,
                              double** out_distances,
                              double* outlats, double* outlons, double* values, double* distances, int* indexes, size_t* len);

/* grib_nearest_class.cc */
grib_nearest* grib_nearest_factory(grib_handle* h, grib_arguments* args, int* error);

/* grib_iterator.cc */
int grib_get_data(const grib_handle* h, double* lats, double* lons, double* values);
int grib_iterator_next(grib_iterator* i, double* lat, double* lon, double* value);
int grib_iterator_has_next(grib_iterator* i);
int grib_iterator_previous(grib_iterator* i, double* lat, double* lon, double* value);
int grib_iterator_reset(grib_iterator* i);
int grib_iterator_init(grib_iterator* i, grib_handle* h, grib_arguments* args);
int grib_iterator_delete(grib_iterator* i);

/* grib_iterator_class.cc */
grib_iterator* grib_iterator_factory(grib_handle* h, grib_arguments* args, unsigned long flags, int* error);

/* grib_iterator_class_gen.cc */
int transform_iterator_data(grib_context* c, double* data, long iScansNegatively, long jScansPositively, long jPointsAreConsecutive, long alternativeRowScanning, size_t numPoints, long nx, long ny);

/* grib_expression.cc */
int grib_expression_native_type(grib_handle* h, grib_expression* g);
int grib_expression_evaluate_long(grib_handle* h, grib_expression* g, long* result);
int grib_expression_evaluate_double(grib_handle* h, grib_expression* g, double* result);
const char* grib_expression_evaluate_string(grib_handle* h, grib_expression* g, char* buf, size_t* size, int* err);
const char* grib_expression_get_name(grib_expression* g);
void grib_expression_print(grib_context* ctx, grib_expression* g, grib_handle* f);
void grib_expression_free(grib_context* ctx, grib_expression* g);
void grib_expression_add_dependency(grib_expression* e, grib_accessor* observer);
grib_arguments* grib_arguments_new(grib_context* c, grib_expression* g, grib_arguments* n);
void grib_arguments_free(grib_context* c, grib_arguments* g);
void grib_arguments_print(grib_context* c, grib_arguments* g, grib_handle* f);
const char* grib_arguments_get_name(grib_handle* h, grib_arguments* args, int n);
const char* grib_arguments_get_string(grib_handle* h, grib_arguments* args, int n);
long grib_arguments_get_long(grib_handle* h, grib_arguments* args, int n);
double grib_arguments_get_double(grib_handle* h, grib_arguments* args, int n);
grib_expression* grib_arguments_get_expression(grib_handle* h, grib_arguments* args, int n);
int grib_arguments_get_count(grib_arguments* args);

/* codes_util.cc */
double normalise_longitude_in_degrees(double lon);
char get_dir_separator_char(void);
int path_is_regular_file(const char* path);
int path_is_directory(const char* filename);
char* codes_getenv(const char* name);
int codes_check_grib_ieee_packing_value(int value);
int codes_flush_sync_close_file(FILE* f);
int is_date_valid(long year, long month, long day, long hour, long minute, double second);
int is_time_valid(long number); // number is HHMM
int compute_scaled_value_and_scale_factor(double input, int64_t scaled_value_max, int64_t scale_factor_max, int64_t* ret_value, int64_t* ret_factor);

/* grib_util.cc */
grib_handle* grib_util_sections_copy(grib_handle* hfrom, grib_handle* hto, int what, int* err);
grib_string_list* grib_util_get_param_id(const char* mars_param);
grib_string_list* grib_util_get_mars_param(const char* param_id);
grib_handle* grib_util_set_spec(grib_handle* h, const grib_util_grid_spec* spec, const grib_util_packing_spec* packing_spec, int flags, const double* data_values, size_t data_values_count, int* err);
int parse_keyval_string(const char* grib_tool, char* arg, int values_required, int default_type, grib_values values[], int* count);
int grib2_is_PDTN_EPS(long productDefinitionTemplateNumber);
int grib2_is_PDTN_Chemical(long productDefinitionTemplateNumber);
int grib2_is_PDTN_ChemicalSourceSink(long productDefinitionTemplateNumber);
int grib2_is_PDTN_ChemicalDistFunc(long productDefinitionTemplateNumber);
int grib2_is_PDTN_Aerosol(long productDefinitionTemplateNumber);
int grib2_is_PDTN_AerosolOptical(long productDefinitionTemplateNumber);
int grib2_select_PDTN(int is_eps, int is_instant, int is_chemical, int is_chemical_srcsink, int is_chemical_distfn, int is_aerosol, int is_aerosol_optical);
size_t sum_of_pl_array(const long* pl, size_t plsize);
int grib_is_earth_oblate(const grib_handle* h);
int grib_check_data_values_minmax(grib_handle* h, const double min_val, const double max_val);
int grib_producing_large_constant_fields(const grib_handle* h, int edition);
int grib_util_grib_data_quality_check(grib_handle* h, double min_val, double max_val);

/* bufr_util.cc */
int compute_bufr_key_rank(grib_handle* h, grib_string_list* keys, const char* key);
char** codes_bufr_copy_data_return_copied_keys(grib_handle* hin, grib_handle* hout, size_t* nkeys, int* err);
int codes_bufr_copy_data(grib_handle* hin, grib_handle* hout);
int codes_bufr_extract_headers_malloc(grib_context* c, const char* filename, codes_bufr_header** result, int* num_messages, int strict_mode);
int codes_bufr_header_get_string(codes_bufr_header* bh, const char* key, char* val, size_t* len);
int codes_bufr_key_is_header(const grib_handle* h, const char* key, int* err);
int codes_bufr_key_is_coordinate(const grib_handle* h, const char* key, int* err);
int codes_bufr_key_exclude_from_dump(const char* key);

/* string_util.cc */
int strcmp_nocase(const char* s1, const char* s2);
void string_rtrim(char* s);
void string_lrtrim(char** x, int do_left, int do_right);
const char* extract_filename(const char* filepath);
char** string_split(char* inputString, const char* delimiter);
int string_to_long(const char* input, long* output, int strict);
int string_ends_with(const char* str1, const char* str2);
int string_count_char(const char* str, char c);
const char* codes_get_product_name(ProductKind product);
const char* grib_get_type_name(int type);
char* string_replace_char(char* str, char oldc, char newc);
void string_remove_char(char* str, char c);

/* functions.cc */
long grib_op_eq(long a, long b);
long grib_op_ne(long a, long b);
long grib_op_lt(long a, long b);
long grib_op_gt(long a, long b);
long grib_op_ge(long a, long b);
long grib_op_le(long a, long b);
long grib_op_bit(long a, long b);
long grib_op_bitoff(long a, long b);
long grib_op_not(long a);
long grib_op_neg(long a);
double grib_op_neg_d(double a);
long grib_op_pow(long a, long b);
long grib_op_add(long a, long b);
long grib_op_sub(long a, long b);
long grib_op_div(long a, long b);
long grib_op_mul(long a, long b);
long grib_op_modulo(long a, long b);
double grib_op_mul_d(double a, double b);
double grib_op_div_d(double a, double b);
double grib_op_add_d(double a, double b);
double grib_op_sub_d(double a, double b);
double grib_op_eq_d(double a, double b);
double grib_op_ne_d(double a, double b);
double grib_op_lt_d(double a, double b);
double grib_op_gt_d(double a, double b);
double grib_op_ge_d(double a, double b);
double grib_op_le_d(double a, double b);

/* codes_memfs.cc */
FILE* codes_fopen(const char* name, const char* mode);
int codes_access(const char* name, int mode);

/* grib_optimize_decimal_factor.cc */
int grib_optimize_decimal_factor(grib_accessor* a, const char* reference_value, const double pmax, const double pmin, const int knbit, const int compat_gribex, const int compat_32bit, long* kdec, long* kbin, double* ref);

/* grib_api_version.cc */
const char* grib_get_git_sha1(void);
const char* codes_get_build_date(void);

/* grib_bits_any_endian.cc */
int grib_is_all_bits_one(int64_t val, long nbits);
int grib_encode_string(unsigned char* bitStream, long* bitOffset, size_t numberOfCharacters, const char* string);
char* grib_decode_string(const unsigned char* bitStream, long* bitOffset, size_t numberOfCharacters, char* string);
unsigned long grib_decode_unsigned_long(const unsigned char* p, long* bitp, long nbits);
int grib_encode_unsigned_long(unsigned char* p, unsigned long val, long* bitp, long nbits);
size_t grib_decode_size_t(const unsigned char* p, long* bitp, long nbits);
int grib_encode_unsigned_longb(unsigned char* p, unsigned long val, long* bitp, long nb);
int grib_encode_size_tb(unsigned char* p, size_t val, long* bitp, long nb);


/* grib_bits_any_endian_simple.cc */
int grib_decode_long_array(const unsigned char* p, long* bitp, long bitsPerValue, size_t n_vals, long* val);
//int grib_decode_double_array(const unsigned char* p, long* bitp, long bitsPerValue, double reference_value, double s, double d, size_t n_vals, double* val);
//int grib_decode_float_array(const unsigned char* p, long* bitp, long bitsPerValue, double reference_value, double s, double d, size_t n_vals, float* val);
int grib_decode_double_array_complex(const unsigned char* p, long* bitp, long nbits, double reference_value, double s, double* d, size_t size, double* val);
int grib_encode_long_array(size_t n_vals, const long* val, long bits_per_value, unsigned char* p, long* off);
int grib_encode_double_array(size_t n_vals, const double* val, long bits_per_value, double reference_value, double d, double divisor, unsigned char* p, long* off);
int grib_encode_double_array_complex(size_t n_vals, double* val, long nbits, double reference_value, double* scal, double d, double divisor, unsigned char* p, long* bitp);
