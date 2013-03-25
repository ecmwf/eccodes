
/* action.c */
void grib_dump(grib_action *a, FILE *f, int l);
void grib_xref(grib_action *a, FILE *f, const char *path);
void grib_free_action(grib_context *context, grib_action *a);
int grib_create_accessor(grib_section *p, grib_action *a, grib_loader *h);
int grib_action_notify_change(grib_action *a, grib_accessor *observer, grib_accessor *observed);
grib_action *grib_action_reparse(grib_action *a, grib_accessor *acc, int *doit);
int grib_action_execute(grib_action *a, grib_handle *h);
void grib_dump_action_branch(FILE *out, grib_action *a, int decay);
void grib_dump_action_tree(grib_context *ctx, FILE *out);
void grib_xref_action_branch(FILE *out, grib_action *a, const char *path);
void grib_compile(grib_action *a, grib_compiler *compiler);

/* action_class_alias.c */
grib_action *grib_action_create_alias(grib_context *context, const char *name, const char *arg1, const char *name_space, int flags);

/* action_class_gen.c */
grib_action *grib_action_create_gen(grib_context *context, const char *name, const char *op, const long len, grib_arguments *params, grib_arguments *default_value, int flags, const char *name_space, const char *set);

/* action_class_if.c */
grib_action *grib_action_create_if(grib_context *context, grib_expression *expression, grib_action *block_true, grib_action *block_false, int transient);

/* action_class_switch.c */
grib_action *grib_action_create_switch(grib_context *context, grib_arguments *args, grib_case *Case, grib_action *Default);
grib_case *grib_case_new(grib_context *c, grib_arguments *values, grib_action *action);

/* grib_accessor_class_g1fcperiod.c */

/* grib_accessor_class_g1end_of_interval_monthly.c */

/* grib_accessor_class_mars_param.c */

/* action_class_section.c */

/* action_class_list.c */
grib_action *grib_action_create_list(grib_context *context, const char *name, grib_expression *expression, grib_action *block);

/* action_class_while.c */
grib_action *grib_action_create_while(grib_context *context, grib_expression *expression, grib_action *block);

/* action_class_put.c */
grib_action *grib_action_create_put(grib_context *context, const char *name, grib_arguments *args);

/* action_class_meta.c */
grib_action *grib_action_create_meta(grib_context *context, const char *name, const char *op, grib_arguments *params, grib_arguments *default_value, unsigned long flags, const char *name_space);

/* action_class_remove.c */
grib_action *grib_action_create_remove(grib_context *context, grib_arguments *args);

/* action_class_assert.c */
grib_action *grib_action_create_assert(grib_context *context, grib_expression *expression);

/* action_class_template.c */
grib_action *grib_action_create_template(grib_context *context, int nofail, const char *name, const char *arg1);
grib_action *get_empty_template(grib_context *c, int *err);

/* action_class_trigger.c */
grib_action *grib_action_create_trigger(grib_context *context, grib_arguments *args, grib_action *block);

/* action_class_when.c */
grib_action *grib_action_create_when(grib_context *context, grib_expression *expression, grib_action *block_true, grib_action *block_false);

/* action_class_concept.c */
grib_action *grib_action_create_concept(grib_context *context, const char *name, grib_concept_value *concept, const char *basename, const char *name_space, const char *defaultkey, const char *masterDir, const char *localDir, const char *ecmfDir, int flags, int nofail);
const char *grib_concept_evaluate(grib_handle *h, grib_action *act);
int grib_concept_apply(grib_handle *h, grib_action *act, const char *name);

/* action_class_set.c */
grib_action *grib_action_create_set(grib_context *context, const char *name, grib_expression *expression, int nofail);

/* action_class_set_darray.c */
grib_action *grib_action_create_set_darray(grib_context *context, const char *name, grib_darray *darray);

/* action_class_set_iarray.c */
grib_action *grib_action_create_set_iarray(grib_context *context, const char *name, grib_iarray *iarray);

/* action_class_noop.c */
grib_action *grib_action_create_noop(grib_context *context, const char *fname);

/* action_class_write.c */
grib_action *grib_action_create_write(grib_context *context, const char *name, int append, int padtomultiple);

/* action_class_print.c */
grib_action *grib_action_create_print(grib_context *context, const char *name, char *outname);

/* action_class_variable.c */
grib_action *grib_action_create_variable(grib_context *context, const char *name, const char *op, const long len, grib_arguments *params, grib_arguments *default_value, int flags, const char *name_space);

/* action_class_modify.c */
grib_action *grib_action_create_modify(grib_context *context, const char *name, long flags);

/* grib_accessor.c */
void grib_print_accessor(grib_accessor *a, grib_dumper *f);
int grib_pack_missing(grib_accessor *a);
int grib_pack_zero(grib_accessor *a);
int grib_is_missing_internal(grib_accessor *a);
int grib_pack_double(grib_accessor *a, const double *v, size_t *len);
int grib_pack_expression(grib_accessor *a, grib_expression *e);
int grib_pack_string(grib_accessor *a, const char *v, size_t *len);
int grib_pack_long(grib_accessor *a, const long *v, size_t *len);
int grib_pack_bytes(grib_accessor *a, const unsigned char *v, size_t *len);
int grib_unpack_bytes(grib_accessor *a, unsigned char *v, size_t *len);
int grib_unpack_double_subarray(grib_accessor *a, double *v, size_t start, size_t len);
int grib_unpack_double(grib_accessor *a, double *v, size_t *len);
int grib_unpack_double_element(grib_accessor *a, size_t i, double *v);
int grib_unpack_string(grib_accessor *a, char *v, size_t *len);
int grib_unpack_long(grib_accessor *a, long *v, size_t *len);
long grib_accessor_get_native_type(grib_accessor *a);
long grib_get_next_position_offset(grib_accessor *a);
long grib_string_length(grib_accessor *a);
long grib_byte_offset(grib_accessor *a);
long grib_byte_count(grib_accessor *a);
long grib_value_count(grib_accessor *a);
int grib_accessor_notify_change(grib_accessor *a, grib_accessor *changed);
void grib_init_accessor(grib_accessor *a, const long len, grib_arguments *args);
void grib_free_accessor(grib_context *ct, grib_accessor *a);
void grib_update_size(grib_accessor *a, size_t len);
int grib_nearest_smaller_value(grib_accessor *a, double val, double *nearest);
size_t grib_preferred_size(grib_accessor *a, int from_handle);
grib_accessor *grib_next_accessor(grib_accessor *a);
void grib_resize(grib_accessor *a, size_t new_size);
int grib_compare_accessors(grib_accessor *a1, grib_accessor *a2, int compare_flags);
const char *grib_get_type_name(int type);

/* grib_concept.c */
grib_concept_value *grib_concept_value_new(grib_context *c, const char *name, grib_concept_condition *conditions);
void grib_concept_value_delete(grib_context *c, grib_concept_value *v);
grib_concept_condition *grib_concept_condition_new(grib_context *c, const char *name, grib_expression *expression);
void grib_concept_condition_delete(grib_context *c, grib_concept_condition *v);

/* grib_darray.c */
grib_darray *grib_darray_new(grib_context *c, size_t size, size_t incsize);
grib_darray *grib_darray_resize(grib_context *c, grib_darray *v);
grib_darray *grib_darray_push(grib_context *c, grib_darray *v, double val);
void grib_darray_delete(grib_context *c, grib_darray *v);

/* grib_iarray.c */
grib_iarray *grib_iarray_new(grib_context *c, size_t size, size_t incsize);
grib_iarray *grib_iarray_resize(grib_context *c, grib_iarray *v);
grib_iarray *grib_iarray_push(grib_context *c, grib_iarray *v, long val);
void grib_iarray_delete(grib_context *c, grib_iarray *v);

/* grib_accessor_class_array.c */

/* grib_accessor_class_assert.c */

/* grib_accessor_class_ascii.c */

/* grib_accessor_class_bit.c */

/* grib_accessor_class_bitmap.c */

/* grib_accessor_class_bits.c */

/* grib_accessor_class_bits_per_value.c */

/* grib_accessor_class_g1bitmap.c */

/* grib_accessor_class_g2bitmap.c */

/* grib_accessor_class_concept.c */

/* grib_accessor_class_decimal_precision.c */

/* grib_accessor_class_divdouble.c */

/* grib_accessor_class_budgdate.c */

/* grib_accessor_class_validity_date.c */

/* grib_accessor_class_validity_time.c */

/* grib_accessor_class_bytes.c */

/* grib_accessor_class.c */
grib_section *grib_create_root_section(const grib_context *context, grib_handle *h);
grib_accessor *grib_accessor_factory(grib_section *p, grib_action *creator, const long len, grib_arguments *params);
void grib_push_accessor(grib_accessor *a, grib_block_of_accessors *l);
void grib_section_post_init(grib_section *s);
void grib_section_adjust_sizes(grib_section *s, int update, int depth);
int grib_get_block_length(grib_section *s, size_t *l);
grib_accessor *find_paddings(grib_section *s);
void grib_update_paddings(grib_section *s);

/* grib_accessor_class_change_scanning_direction.c */

/* grib_accessor_class_codeflag.c */

/* grib_accessor_class_codetable.c */
void grib_codetable_delete(grib_context *c);

/* grib_accessor_class_codetable_units.c */

/* grib_accessor_class_codetable_title.c */

/* grib_accessor_class_count_file.c */

/* grib_accessor_class_count_total.c */

/* grib_accessor_class_double.c */

/* grib_accessor_class_element.c */

/* grib_accessor_class_evaluate.c */

/* grib_accessor_class_g1area.c */

/* grib_accessor_class_g1date.c */

/* grib_accessor_class_g1monthlydate.c */

/* grib_accessor_class_library_version.c */

/* grib_accessor_class_when.c */

/* grib_accessor_class_g1verificationdate.c */

/* grib_accessor_class_g1day_of_the_year_date.c */

/* grib_accessor_class_g1_half_byte_codeflag.c */

/* grib_accessor_class_g1forecastmonth.c */

/* grib_accessor_class_g1step_range.c */
int grib_g1_step_get_steps(grib_accessor *a, long *start, long *end);
int grib_g1_step_apply_units(long *start, long *end, long *step_unit, long *P1, long *P2, long *unit, const int max, const int instant);

/* grib_accessor_class_g2step_range.c */

/* grib_accessor_class_data_g22order_packing.c */

/* grib_accessor_class_mars_step.c */

/* grib_accessor_class_g1param.c */

/* grib_accessor_class_g1p1p2.c */

/* grib_accessor_class_g1_increment.c */

/* grib_accessor_class_latlon_increment.c */

/* grib_accessor_class_g2date.c */

/* grib_accessor_class_g2level.c */

/* grib_accessor_class_g2step.c */

/* grib_accessor_class_g2end_step.c */

/* grib_accessor_class_g2latlon.c */

/* grib_accessor_class_g2lon.c */

/* grib_accessor_class_global_gaussian.c */

/* grib_accessor_class_gen.c */

/* grib_accessor_class_gts_header.c */

/* grib_accessor_class_ifs_param.c */

/* grib_accessor_class_julian_day.c */

/* grib_accessor_class_latlonvalues.c */

/* grib_accessor_class_latitudes.c */

/* grib_accessor_class_longitudes.c */

/* grib_accessor_class_missing.c */

/* grib_accessor_class_offset_file.c */

/* grib_accessor_class_scale.c */

/* grib_accessor_class_from_scale_factor_scaled_value.c */

/* grib_accessor_class_times.c */

/* grib_accessor_class_forward.c */

/* grib_accessor_class_g2bitmap_present.c */

/* grib_accessor_class_ibmfloat.c */

/* grib_accessor_class_ieeefloat.c */

/* grib_accessor_class_constant.c */

/* grib_accessor_class_iterator.c */
grib_iterator *grib_iterator_new(grib_handle *h, unsigned long flags, int *error);

/* grib_accessor_class_message.c */

/* grib_accessor_class_nearest.c */
grib_nearest *grib_nearest_new(grib_handle *h, int *error);

/* grib_accessor_class_box.c */
grib_box *grib_box_new(grib_handle *h, int *error);

/* grib_accessor_class_ksec1expver.c */

/* grib_accessor_class_laplacian.c */

/* grib_accessor_class_label.c */

/* grib_accessor_class_long.c */

/* grib_accessor_class_lookup.c */

/* grib_accessor_class_octect_number.c */

/* grib_accessor_class_headers_only.c */

/* grib_accessor_class_padding.c */

/* grib_accessor_class_pad.c */

/* grib_accessor_class_padto.c */

/* grib_accessor_class_padtoeven.c */

/* grib_accessor_class_padtomultiple.c */

/* grib_accessor_class_section_padding.c */

/* grib_accessor_class_section_pointer.c */

/* grib_accessor_class_position.c */

/* grib_accessor_class_signed.c */

/* grib_accessor_class_signed_bits.c */

/* grib_accessor_class_section.c */

/* grib_accessor_class_step_in_units.c */

/* grib_accessor_class_section_length.c */

/* grib_accessor_class_g1_message_length.c */
int grib_get_g1_message_size(grib_handle *h, grib_accessor *tl, grib_accessor *s4, long *total_length, long *sec4_len);

/* grib_accessor_class_g1_section4_length.c */

/* grib_accessor_class_size.c */

/* grib_accessor_class_scale_values.c */

/* grib_accessor_class_offset_values.c */

/* grib_accessor_class_sprintf.c */

/* grib_accessor_class_round.c */

/* grib_accessor_class_spectral_truncation.c */

/* grib_accessor_class_time.c */

/* grib_accessor_class_transient.c */

/* grib_accessor_class_g1_half_byte_codeflag.c */

/* grib_accessor_class_values.c */

/* grib_accessor_class_simple_packing_error.c */

/* grib_accessor_class_data_simple_packing.c */

/* grib_accessor_class_count_missing.c */

/* grib_accessor_class_data_sh_packed.c */

/* grib_accessor_class_data_sh_unpacked.c */

/* grib_accessor_class_number_of_values_data_raw_packing.c */

/* grib_accessor_class_data_g1simple_packing.c */

/* grib_accessor_class_data_g1shsimple_packing.c */

/* grib_accessor_class_data_shsimple_packing.c */

/* grib_accessor_class_data_constant_field.c */

/* grib_accessor_class_data_dummy_field.c */

/* grib_2order_packer_simple.c */
void grib_free_second_order_groups(grib_context *c, second_order_packed *sp);
second_order_packed *grib_get_second_order_groups(grib_context *c, const unsigned long *vals, size_t len);

/* grib_accessor_class_variable.c */

/* grib_accessor_class_second_order_bits_per_value.c */

/* grib_accessor_class_data_g2simple_packing.c */

/* grib_accessor_class_data_g2simple_packing_with_preprocessing.c */

/* grib_accessor_class_data_g2shsimple_packing.c */

/* grib_accessor_class_data_g2complex_packing.c */

/* grib_accessor_class_data_2order_packing.c */

/* grib_accessor_class_data_2order_packing_count.c */

/* grib_accessor_class_data_g1second_order_row_by_row_packing.c */

/* grib_accessor_class_data_g1second_order_constant_width_packing.c */

/* grib_accessor_class_data_g1second_order_general_packing.c */

/* grib_accessor_class_data_g1second_order_general_extended_packing.c */

/* grib_accessor_class_g2grid.c */

/* grib_accessor_class_data_apply_bitmap.c */

/* grib_accessor_class_data_apply_boustrophedonic.c */

/* grib_accessor_class_data_secondary_bitmap.c */

/* grib_accessor_class_data_g1secondary_bitmap.c */

/* grib_accessor_class_data_g2secondary_bitmap.c */

/* grib_accessor_class_data_jpeg2000_packing.c */

/* grib_accessor_class_data_png_packing.c */

/* grib_accessor_class_data_raw_packing.c */

/* grib_accessor_class_data_complex_packing.c */

/* grib_accessor_class_data_g1complex_packing.c */

/* grib_accessor_class_gds_not_present_bitmap.c */

/* grib_accessor_class_data_apply_gdsnotpresent.c */

/* grib_accessor_class_gds_is_present.c */

/* grib_accessor_class_select_step_template.c */

/* grib_accessor_class_local_definition.c */

/* grib_accessor_class_g2_eps.c */

/* grib_accessor_class_g2_mars_labeling.c */

/* grib_accessor_class_md5.c */

/* grib_jasper_encoding.c */
int grib_jasper_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *values, size_t *no_values);
int grib_jasper_encode(grib_context *c, j2k_encode_helper *helper);
int grib_jasper_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *val, size_t *n_vals);
int grib_jasper_encode(grib_context *c, j2k_encode_helper *helper);

/* grib_openjpeg_encoding.c */
int grib_openjpeg_encode(grib_context *c, j2k_encode_helper *helper);
int grib_openjpeg_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *val, size_t *n_vals);
int grib_openjpeg_decode(grib_context *c, unsigned char *buf, size_t *buflen, double *val, size_t *n_vals);
int grib_openjpeg_encode(grib_context *c, j2k_encode_helper *helper);

/* action_class_set_missing.c */
grib_action *grib_action_create_set_missing(grib_context *context, const char *name);

/* grib_accessor_class_number_of_points.c */

/* grib_accessor_class_suppressed.c */

/* grib_index.c */
int grib_index_compress(grib_index *index);
int grib_read_uchar(FILE *fh, unsigned char *val);
int grib_read_short(FILE *fh, short *val);
int grib_read_long(FILE *fh, long *val);
int grib_read_unsigned_long(FILE *fh, unsigned long *val);
int grib_write_uchar(FILE *fh, unsigned char val);
int grib_write_short(FILE *fh, short val);
int grib_write_long(FILE *fh, long val);
int grib_write_unsigned_long(FILE *fh, unsigned long val);
int grib_write_string(FILE *fh, const char *s);
int grib_write_identifier(FILE *fh);
int grib_write_null_marker(FILE *fh);
int grib_write_not_null_marker(FILE *fh);
char *grib_read_string(grib_context *c, FILE *fh, int *err);
grib_field_tree *grib_read_field_tree(grib_context *c, FILE *fh, grib_file **files, int *err);
grib_index *grib_index_new(grib_context *c, const char *key, int *err);
void grib_index_delete(grib_index *index);
int grib_index_write(grib_index *index, const char *filename);
grib_index *grib_index_read(grib_context *c, const char *filename, int *err);
int grib_index_search_same(grib_index *index, grib_handle *h);
int grib_index_add_file(grib_index *index, const char *filename);
grib_index *grib_index_new_from_file(grib_context *c, char *filename, const char *keys, int *err);
int grib_index_get_size(grib_index *index, const char *key, size_t *size);
int grib_index_get_string(grib_index *index, const char *key, char **values, size_t *size);
int grib_index_get_long(grib_index *index, const char *key, long *values, size_t *size);
int grib_index_get_double(grib_index *index, const char *key, double *values, size_t *size);
int grib_index_select_long(grib_index *index, const char *skey, long value);
int grib_index_select_double(grib_index *index, const char *skey, double value);
int grib_index_select_string(grib_index *index, const char *skey, char *value);
grib_handle *grib_index_get_handle(grib_field *field, int *err);
void grib_index_dump(grib_index *index);
char *grib_get_field_file(grib_index *index, off_t *offset);
grib_handle *grib_handle_new_from_index(grib_index *index, int *err);
void grib_index_rewind(grib_index *index);
int grib_index_search(grib_index *index, grib_index_key *keys);

/* grib_accessor_class_number_of_points_gaussian.c */

/* grib_accessor_class_number_of_values.c */

/* grib_accessor_class_number_of_coded_values.c */

/* grib_accessor_class_g1number_of_coded_values_sh_complex.c */

/* grib_accessor_class_g1number_of_coded_values_sh_simple.c */

/* grib_accessor_class_dirty.c */

/* grib_accessor_class_statistics.c */

/* grib_accessor_class_statistics_spectral.c */

/* grib_accessor_class_unsigned.c */

/* grib_accessor_class_unsigned_bits.c */

/* grib_accessor_class_spd.c */

/* grib_accessor_class_sum.c */

/* grib_accessor_class_vector.c */

/* grib_accessor_class_long_vector.c */

/* grib_gaussian_reduced.c */
void grib_get_reduced_row(long pl, double lon_first, double lon_last, long *npoints, long *ilon_first, long *ilon_last);

/* grib_accessor_class_abstract_vector.c */

/* grib_accessor_class_abstract_long_vector.c */

/* grib_loader_from_handle.c */
int grib_lookup_long_from_handle(grib_context *gc, grib_loader *loader, const char *name, long *value);
int grib_init_accessor_from_handle(grib_loader *loader, grib_accessor *ga, grib_arguments *default_value);

/* grib_bits.c */
unsigned long grib_decode_unsigned_byte_long(const unsigned char *p, long o, int l);
long grib_decode_signed_long(const unsigned char *p, long o, int l);
int grib_encode_signed_long(unsigned char *p, long val, long o, int l);
int grib_get_bit(const unsigned char *p, long bitp);
void grib_set_bit(unsigned char *p, long bitp, int val);
long grib_decode_signed_longb(const unsigned char *p, long *bitp, long nbits);
int grib_encode_signed_longb(unsigned char *p, long val, long *bitp, long nb);

/* grib_timer.c */
double proc_cpu(void);
char *timename(double t);
grib_timer *grib_get_timer(grib_context *c, const char *name, const char *statname, int elapsed);
int grib_timer_start(grib_timer *t);
int grib_timer_stop(grib_timer *t, long total);
double grib_timer_value(grib_timer *t);
const char *bytename(double bytes);
void grib_timer_print(grib_timer *t);
void grib_timer_partial_rate(grib_timer *t, double start, long total);
void grib_print_all_timers(void);
void grib_reset_all_timers(void);
grib_timer *grib_get_timer(grib_context *c, const char *name, const char *statname, int elapsed);
int grib_timer_start(grib_timer *t);
int grib_timer_stop(grib_timer *t, long total);
double grib_timer_value(grib_timer *t);
void grib_timer_print(grib_timer *t);
void grib_timer_partial_rate(grib_timer *t, double start, long total);
void grib_print_all_timers(void);
void grib_reset_all_timers(void);

/* grib_ibmfloat.c */
unsigned long grib_ibm_to_long(double x);
double grib_ibmfloat_error(double x);
double grib_long_to_ibm(unsigned long x);
double grib_ibm_table_e(unsigned long e);
double grib_ibm_table_v(unsigned long e);
unsigned long grib_ibm_nearest_smaller_to_long(double x);
int grib_nearest_smaller_ibm_float(double a, double *ret);

/* grib_ieeefloat.c */
double grib_ieee_table_e(unsigned long e);
double grib_ieee_table_v(unsigned long e);
unsigned long grib_ieee_to_long(double x);
double grib_ieeefloat_error(double x);
double grib_long_to_ieee(unsigned long x);
unsigned long grib_ieee_nearest_smaller_to_long(double x);
int grib_nearest_smaller_ieee_float(double a, double *ret);
double grib_ieeefloat_error(double x);
double grib_long_to_ieee(unsigned long x);
int grib_nearest_smaller_ieee_float(double a, double *x);
unsigned long grib_ieee_to_long(double x);
unsigned long grib_ieee64_to_long(double x);
double grib_long_to_ieee64(unsigned long x);
int grib_ieee_decode_array(grib_context *c, unsigned char *buf, size_t nvals, int bytes, double *val);
int grib_ieee_decode_array(grib_context *c, unsigned char *buf, size_t nvals, int bytes, double *val);
int grib_ieee_encode_array(grib_context *c, double *val, size_t nvals, int bytes, unsigned char *buf);
int grib_ieee_encode_array(grib_context *c, double *val, size_t nvals, int bytes, unsigned char *buf);

/* grib_accessor_class_reference_value_error.c */

/* grib_memory.c */
void *grib_transient_malloc(const grib_context *c, size_t s);
void *grib_transient_realloc(const grib_context *c, void *p, size_t s);
void grib_transient_free(const grib_context *c, void *p);
void *grib_permanent_malloc(const grib_context *c, size_t s);
void *grib_permanent_realloc(const grib_context *c, void *p, size_t s);
void grib_permanent_free(const grib_context *c, void *p);
void *grib_buffer_malloc(const grib_context *c, size_t s);
void grib_buffer_free(const grib_context *c, void *p);
void *grib_buffer_realloc(const grib_context *c, void *p, size_t s);

/* grib_buffer.c */
void grib_get_buffer_ownership(const grib_context *c, grib_buffer *b);
grib_buffer *grib_create_growable_buffer(const grib_context *c);
grib_buffer *grib_new_buffer(const grib_context *c, unsigned char *data, size_t buflen);
void grib_buffer_delete(const grib_context *c, grib_buffer *b);
void grib_grow_buffer(const grib_context *c, grib_buffer *b, size_t new_size);
void grib_buffer_set_ulength(const grib_context *c, grib_buffer *b, size_t length);
void grib_recompute_sections_lengths(grib_section *s);
void grib_buffer_replace(grib_accessor *a, const unsigned char *data, size_t newsize, int update_lengths, int update_paddings);
void grib_update_sections_lengths(grib_handle *h);

/* grib_dumper.c */
void grib_init_dumper(grib_dumper *d);
void grib_dumper_delete(grib_dumper *d);
void grib_dump_long(grib_dumper *d, grib_accessor *a, const char *comment);
void grib_dump_double(grib_dumper *d, grib_accessor *a, const char *comment);
void grib_dump_string(grib_dumper *d, grib_accessor *a, const char *comment);
void grib_dump_label(grib_dumper *d, grib_accessor *a, const char *comment);
void grib_dump_bytes(grib_dumper *d, grib_accessor *a, const char *comment);
void grib_dump_bits(grib_dumper *d, grib_accessor *a, const char *comment);
void grib_dump_section(grib_dumper *d, grib_accessor *a, grib_block_of_accessors *block);
void grib_dump_values(grib_dumper *d, grib_accessor *a);
void grib_dump_header(grib_dumper *d, grib_handle *h);
void grib_dump_footer(grib_dumper *d, grib_handle *h);

/* grib_dumper_class_serialize.c */

/* grib_dumper_class_debug.c */

/* grib_dumper_class_default.c */

/* grib_dumper_class_keys.c */

/* grib_dumper_class_c_code.c */

/* grib_dumper_class_wmo.c */

/* grib_dumper_class.c */
grib_dumper *grib_dumper_factory(const char *op, grib_handle *h, FILE *out, unsigned long option_flags, void *arg);
void grib_dump_accessors_block(grib_dumper *dumper, grib_block_of_accessors *block);
int grib_print(grib_handle *h, const char *name, grib_dumper *d);
void grib_dump_content(grib_handle *h, FILE *f, const char *mode, unsigned long option_flags, void *data);

/* grib_context.c */
size_t grib_context_read(const grib_context *c, void *ptr, size_t size, void *stream);
off_t grib_context_tell(const grib_context *c, void *stream);
int grib_context_seek(const grib_context *c, off_t offset, int whence, void *stream);
int grib_context_eof(const grib_context *c, void *stream);
size_t grib_context_write(const grib_context *c, const void *ptr, size_t size, void *stream);
void grib_context_set_print_proc(grib_context *c, grib_print_proc p);
void grib_context_set_debug(grib_context *c, int mode);
void grib_context_set_logging_proc(grib_context *c, grib_log_proc p);
long grib_get_api_version(void);
void grib_print_api_version(FILE *out);
grib_context *grib_context_get_default(void);
grib_context *grib_context_new(grib_context *parent);
char *grib_context_full_path(grib_context *c, const char *basename);
void grib_context_free(const grib_context *c, void *p);
void grib_context_free_persistent(const grib_context *c, void *p);
void grib_context_reset(grib_context *c);
void grib_context_delete(grib_context *c);
void *grib_context_malloc_persistent(const grib_context *c, size_t size);
char *grib_context_strdup_persistent(const grib_context *c, const char *s);
void *grib_context_malloc_clear_persistent(const grib_context *c, size_t size);
void *grib_context_malloc(const grib_context *c, size_t size);
void *grib_context_realloc(const grib_context *c, void *p, size_t size);
char *grib_context_strdup(const grib_context *c, const char *s);
void *grib_context_malloc_clear(const grib_context *c, size_t size);
void *grib_context_buffer_malloc(const grib_context *c, size_t size);
void grib_context_buffer_free(const grib_context *c, void *p);
void *grib_context_buffer_realloc(const grib_context *c, void *p, size_t size);
void *grib_context_buffer_malloc_clear(const grib_context *c, size_t size);
void grib_context_set_memory_proc(grib_context *c, grib_malloc_proc m, grib_free_proc f, grib_realloc_proc r);
void grib_context_set_persistent_memory_proc(grib_context *c, grib_malloc_proc m, grib_free_proc f);
void grib_context_set_buffer_memory_proc(grib_context *c, grib_malloc_proc m, grib_free_proc f, grib_realloc_proc r);
void grib_context_set_data_accessing_proc(grib_context *c, grib_data_read_proc read, grib_data_write_proc write, grib_data_tell_proc tell);
void grib_context_log(const grib_context *c, int level, const char *fmt, ...);
void grib_context_print(const grib_context *c, void *descriptor, const char *fmt, ...);

/* grib_date.c */
int grib_julian_to_datetime(double jd, long *year, long *month, long *day, long *hour, long *minute, long *second);
int grib_datetime_to_julian(long year, long month, long day, long hour, long minute, long second, double *jd);
long grib_julian_to_date(long jdate);
long grib_date_to_julian(long ddate);

/* grib_fieldset.c */
int grib_fieldset_new_column(grib_fieldset *set, int id, char *key, int type);
int grib_fieldset_column_copy_from_handle(grib_handle *h, grib_fieldset *set, int i);
grib_fieldset *grib_fieldset_new_from_files(grib_context *c, char *filenames[], int nfiles, char **keys, int nkeys, char *where_string, char *order_by_string, int *err);
int grib_fieldset_apply_where(grib_fieldset *set, const char *where_string);
int grib_fieldset_apply_order_by(grib_fieldset *set, const char *order_by_string);
void grib_fieldset_delete_order_by(grib_context *c, grib_order_by *order_by);
void grib_fieldset_delete(grib_fieldset *set);
int grib_fieldset_add(grib_fieldset *set, char *filename);
void grib_fieldset_rewind(grib_fieldset *set);
grib_handle *grib_fieldset_next_handle(grib_fieldset *set, int *err);
int grib_fieldset_count(grib_fieldset *set);
grib_handle *grib_fieldset_retrieve(grib_fieldset *set, int i, int *err);

/* grib_filepool.c */
void grib_file_pool_clean(void);
grib_file *grib_file_pool_get_files(void);
int grib_file_pool_read(grib_context *c, FILE *fh);
int grib_file_pool_write(FILE *fh);
grib_file *grib_file_open(const char *filename, const char *mode, int *err);
void grib_file_close(const char *filename, int *err);
void grib_file_close_all(int *err);
grib_file *grib_get_file(const char *filename, int *err);
grib_file *grib_find_file(short id);
grib_file *grib_file_new(grib_context *c, const char *name, int *err);
void grib_file_delete(grib_file *file);

/* grib_geography.c */
int grib_get_gaussian_latitudes(long trunc, double *lats);

/* grib_handle.c */
grib_section *grib_section_create(grib_handle *h, grib_accessor *owner);
void grib_swap_sections(grib_section *old, grib_section *new);
void grib_empty_section(grib_context *c, grib_section *b);
void grib_section_delete(grib_context *c, grib_section *b);
int grib_handle_delete(grib_handle *h);
grib_handle *grib_new_handle(grib_context *c);
grib_handle *grib_handle_new_from_template(grib_context *c, const char *name);
grib_handle *grib_handle_new_from_samples(grib_context *c, const char *name);
int grib_write_message(grib_handle *h, const char *file, const char *mode);
grib_handle *grib_handle_clone(grib_handle *h);
grib_handle *grib_handle_new_from_message_copy(grib_context *c, const void *data, size_t size);
grib_handle *grib_handle_new_from_partial_message_copy(grib_context *c, const void *data, size_t size);
grib_handle *grib_handle_new_from_partial_message(grib_context *c, void *data, size_t buflen);
grib_handle *grib_handle_new_from_message(grib_context *c, void *data, size_t buflen);
grib_handle *grib_handle_new_from_multi_message(grib_context *c, void **data, size_t *buflen, int *error);
grib_handle *grib_handle_new_from_file(grib_context *c, FILE *f, int *error);
grib_handle *grib_handle_new_from_nc_file(grib_context *c, const char *file, int *error);
grib_handle *grib_handle_headers_only_new_from_file(grib_context *c, FILE *f, int *error);
grib_handle *grib_gts_handle_new_from_file(grib_context *c, FILE *f, int *error);
grib_handle *grib_bufr_handle_new_from_file(grib_context *c, FILE *f, int *error);
grib_multi_handle *grib_multi_handle_new(grib_context *c);
int grib_multi_handle_delete(grib_multi_handle *h);
int grib_multi_handle_append(grib_handle *h, int start_section, grib_multi_handle *mh);
int grib_multi_handle_write(grib_multi_handle *h, FILE *f);
int grib_get_partial_message(grib_handle *h, const void **msg, size_t *len, int start_section);
int grib_get_partial_message_copy(grib_handle *h, void *message, size_t *len, int start_section);
int grib_get_message_copy(grib_handle *h, void *message, size_t *len);
int grib_get_message(grib_handle *h, const void **msg, size_t *size);
int grib_get_message_headers(grib_handle *h, const void **msg, size_t *size);
grib_handle *grib_handle_new(grib_context *c);
grib_action *grib_action_from_filter(const char *filter);
int grib_handle_apply_action(grib_handle *h, grib_action *a);
int grib_handle_prepare_action(grib_handle *h, grib_action *a);
void grib_multi_support_on(grib_context *c);
void grib_multi_support_off(grib_context *c);
void grib_gts_header_on(grib_context *c);
void grib_gts_header_off(grib_context *c);
int grib_get_gribex_mode(grib_context *c);
void grib_gribex_mode_on(grib_context *c);
void grib_gribex_mode_off(grib_context *c);
void grib_multi_support_reset(grib_context *c);

/* grib_header_compute.c */
void print_math(grib_math *m);
grib_math *grib_math_clone(grib_context *c, grib_math *m);
void grib_math_delete(grib_context *c, grib_math *m);
grib_math *grib_math_new(grib_context *c, const char *formula, int *err);

/* grib_hash_keys.c */
struct grib_keys_hash *grib_keys_hash_get(const char *str, unsigned int len);
grib_itrie *grib_hash_keys_new(grib_context *c, int *count);
void grib_hash_keys_delete(grib_itrie *t);
int grib_hash_keys_get_id(grib_itrie *t, const char *key);
int grib_hash_keys_insert(grib_itrie *t, const char *key);
int grib_hash_keys_get_size(grib_itrie *t);

/* grib_io.c */
off_t stdio_tell(void *data);
int stdio_seek(void *data, off_t len);
size_t stdio_read(void *data, void *buf, size_t len, int *err);
int wmo_read_any_from_file(FILE *f, void *buffer, size_t *len);
int wmo_read_grib_from_file(FILE *f, void *buffer, size_t *len);
int wmo_read_bufr_from_file(FILE *f, void *buffer, size_t *len);
int wmo_read_gts_from_file(FILE *f, void *buffer, size_t *len);
int wmo_read_any_from_stream(void *stream_data, long (*stream_proc )(void *, void *buffer, long len ), void *buffer, size_t *len);
void *wmo_read_gts_from_file_malloc(FILE *f, size_t *size, int *err);
void *wmo_read_any_from_file_malloc(FILE *f, size_t *size, int *err);
void *wmo_read_grib_from_file_malloc(FILE *f, size_t *size, int *err);
void *wmo_read_bufr_from_file_malloc(FILE *f, size_t *size, int *err);
int grib_read_any_headers_only_from_file_alloc(grib_context *ctx, FILE *f, void **buffer, size_t *length, off_t *offset);
int grib_read_any_from_file_alloc(grib_context *ctx, FILE *f, void **buffer, size_t *length);
int grib_read_any_headers_only_from_file(grib_context *ctx, FILE *f, void *buffer, size_t *len);
int grib_read_any_from_file(grib_context *ctx, FILE *f, void *buffer, size_t *len);
int grib_read_any_from_memory_alloc(grib_context *ctx, unsigned char **data, size_t *data_length, void **buffer, size_t *length);
int grib_read_any_from_memory(grib_context *ctx, unsigned char **data, size_t *data_length, void *buffer, size_t *len);
int grib_count_in_file(grib_context *c, FILE *f, int *n);

/* grib_trie.c */
grib_trie *grib_trie_new(grib_context *c);
void grib_trie_delete(grib_trie *t);
void grib_trie_clear(grib_trie *t);
void *grib_trie_insert(grib_trie *t, const char *key, void *data);
void *grib_trie_insert_no_replace(grib_trie *t, const char *key, void *data);
void *grib_trie_get(grib_trie *t, const char *key);

/* grib_itrie.c */
grib_itrie *grib_itrie_new(grib_context *c, int *count);
void grib_itrie_delete(grib_itrie *t);
int grib_itrie_get_id(grib_itrie *t, const char *key);
int grib_itrie_insert(grib_itrie *t, const char *key);
int grib_itrie_get_size(grib_itrie *t);

/* grib_rules.c */
grib_rule_entry *grib_new_rule_entry(grib_context *c, const char *name, grib_expression *expression);
grib_rule *grib_new_rule(grib_context *c, grib_expression *condition, grib_rule_entry *entries);

/* grib_keys_iterator.c */
grib_keys_iterator *grib_keys_iterator_new(grib_handle *h, unsigned long filter_flags, char *name_space);
int grib_keys_iterator_set_flags(grib_keys_iterator *ki, unsigned long flags);
int grib_keys_iterator_rewind(grib_keys_iterator *ki);
int grib_keys_iterator_next(grib_keys_iterator *kiter);
const char *grib_keys_iterator_get_name(grib_keys_iterator *kiter);
grib_accessor *grib_keys_iterator_get_accessor(grib_keys_iterator *kiter);
int grib_keys_iterator_delete(grib_keys_iterator *kiter);
int grib_keys_iterator_get_long(grib_keys_iterator *kiter, long *v, size_t *len);
int grib_keys_iterator_get_double(grib_keys_iterator *kiter, double *v, size_t *len);
int grib_keys_iterator_get_string(grib_keys_iterator *kiter, char *v, size_t *len);
int grib_keys_iterator_get_bytes(grib_keys_iterator *kiter, unsigned char *v, size_t *len);
int grib_keys_iterator_get_native_type(grib_keys_iterator *kiter);

/* grib_parse_utils.c */
int grib_recompose_name(grib_handle *h, grib_accessor *observer, const char *uname, char *fname, int fail);
int grib_recompose_print(grib_handle *h, grib_accessor *observer, const char *uname, char *fname, int fail, FILE *out);
grib_action_file *grib_find_action_file(const char *fname, grib_action_file_list *afl);
void grib_push_action_file(grib_action_file *af, grib_action_file_list *afl);
int grib_yywrap(void);
int grib_yyerror(const char *msg);
void grib_parser_include(const char *fname);
grib_concept_value *grib_parse_concept_file(grib_context *gc, const char *filename);
grib_rule *grib_parse_rules_file(grib_context *gc, const char *filename);
grib_action *grib_parse_file(grib_context *gc, const char *filename);
int grib_type_to_int(char id);

/* grib_query.c */
grib_accessor *grib_find_accessor_fast(grib_handle *h, const char *name);
grib_accessor *grib_find_accessor(grib_handle *h, const char *name);
int grib_find_all_accessors(grib_handle *h, const char *name, search_all_callback_proc callback, void *data);

/* grib_scaling.c */
double grib_power(long s, long n);
long grib_get_binary_scale_fact(double max, double min, long bpval, int *ret);
long grib_get_bits_per_value(double max, double min, long binary_scale_factor);
long grib_get_decimal_scale_fact(double max, double min, long bpval, long binary_scale);

/* grib_templates.c */
grib_handle *grib_internal_template(grib_context *c, const char *name);
grib_handle *grib_internal_template(grib_context *c, const char *name);
grib_handle *grib_external_template(grib_context *c, const char *name);
char *grib_external_template_path(grib_context *c, const char *name);

/* grib_dependency.c */
void grib_dependency_add(grib_accessor *observer, grib_accessor *observed);
void grib_dependency_remove_observed(grib_accessor *observed);
int grib_dependency_notify_change(grib_accessor *observed);
void grib_dependency_remove_observer(grib_accessor *observer);
void grib_dependency_observe_expression(grib_accessor *observer, grib_expression *e);
void grib_dependency_observe_arguments(grib_accessor *observer, grib_arguments *a);

/* grib_value.c */
int grib_set_expression(grib_handle *h, const char *name, grib_expression *e);
int grib_set_expression_internal(grib_handle *h, const char *name, grib_expression *e);
int grib_set_long_internal(grib_handle *h, const char *name, long val);
int grib_set_long(grib_handle *h, const char *name, long val);
int grib_set_double_internal(grib_handle *h, const char *name, double val);
int grib_copy_namespace(grib_handle *dest, const char *name, grib_handle *src);
int grib_set_double(grib_handle *h, const char *name, double val);
int grib_set_string_internal(grib_handle *h, const char *name, const char *val, size_t *length);
int grib_set_string(grib_handle *h, const char *name, const char *val, size_t *length);
int grib_set_bytes_internal(grib_handle *h, const char *name, const unsigned char *val, size_t *length);
int grib_set_bytes(grib_handle *h, const char *name, const unsigned char *val, size_t *length);
int grib_clear(grib_handle *h, const char *name);
int grib_set_missing_internal(grib_handle *h, const char *name);
int grib_set_missing(grib_handle *h, const char *name);
int grib_is_missing(grib_handle *h, const char *name, int *err);
int grib_set_flag(grib_handle *h, const char *name, unsigned long flag);
int grib_set_double_array_internal(grib_handle *h, const char *name, const double *val, size_t length);
int grib_set_double_array(grib_handle *h, const char *name, const double *val, size_t length);
int grib_set_long_array_internal(grib_handle *h, const char *name, const long *val, size_t length);
int grib_set_long_array(grib_handle *h, const char *name, const long *val, size_t length);
int grib_get_long_internal(grib_handle *h, const char *name, long *val);
int grib_get_long(grib_handle *h, const char *name, long *val);
int grib_get_double_internal(grib_handle *h, const char *name, double *val);
int grib_get_double(grib_handle *h, const char *name, double *val);
int grib_get_double_element_internal(grib_handle *h, const char *name, int i, double *val);
int grib_get_double_element(grib_handle *h, const char *name, int i, double *val);
int grib_points_get_values(grib_handle *h, grib_points *points, double *val);
int grib_get_double_elements(grib_handle *h, const char *name, int *i, long len, double *val);
int grib_get_string_internal(grib_handle *h, const char *name, char *val, size_t *length);
int grib_get_string(grib_handle *h, const char *name, char *val, size_t *length);
int grib_get_bytes_internal(grib_handle *h, const char *name, unsigned char *val, size_t *length);
int grib_get_bytes(grib_handle *h, const char *name, unsigned char *val, size_t *length);
int grib_get_native_type(grib_handle *h, const char *name, int *type);
const char *grib_get_accessor_class_name(grib_handle *h, const char *name);
int _grib_get_double_array_internal(grib_handle *h, grib_accessor *a, double *val, size_t buffer_len, size_t *decoded_length);
int grib_get_double_array_internal(grib_handle *h, const char *name, double *val, size_t *length);
int grib_get_double_array(grib_handle *h, const char *name, double *val, size_t *length);
int grib_get_string_length(grib_handle *h, const char *name, size_t *size);
int grib_get_size(grib_handle *h, const char *name, size_t *size);
int grib_get_count(grib_handle *h, const char *name, size_t *size);
int _grib_get_size(grib_handle *h, grib_accessor *a, size_t *size);
int grib_get_offset(grib_handle *h, const char *key, size_t *val);
int _grib_get_long_array_internal(grib_handle *h, grib_accessor *a, long *val, size_t buffer_len, size_t *decoded_length);
int grib_get_long_array_internal(grib_handle *h, const char *name, long *val, size_t *length);
int grib_get_long_array(grib_handle *h, const char *name, long *val, size_t *length);
grib_key_value_list *grib_key_value_list_clone(grib_context *c, grib_key_value_list *list);
void grib_key_value_list_delete(grib_context *c, grib_key_value_list *kvl);
int grib_get_key_value_list(grib_handle *h, grib_key_value_list *list);
int grib_get_values(grib_handle *h, grib_values *args, size_t count);
int grib_set_values(grib_handle *h, grib_values *args, size_t count);
int grib_get_nearest_smaller_value(grib_handle *h, const char *name, double val, double *nearest);
void grib_print_values(grib_values *values, int count);
int grib_values_check(grib_handle *h, grib_values *values, int count);

/* grib_errors.c */
const char *grib_get_error_message(int code);
void grib_check(const char *call, const char *file, int line, int e, const char *msg);
void grib_fail(const char *expr, const char *file, int line);

/* grib_expression_class_binop.c */
grib_expression *new_binop_expression(grib_context *c, grib_binop_long_proc long_func, grib_binop_double_proc double_func, grib_expression *left, grib_expression *right);

/* grib_expression_class_true.c */
grib_expression *new_true_expression(grib_context *c);

/* grib_expression_class_string_compare.c */
grib_expression *new_string_compare_expression(grib_context *c, grib_expression *left, grib_expression *right);

/* grib_expression_class_unop.c */
grib_expression *new_unop_expression(grib_context *c, grib_unop_long_proc long_func, grib_unop_double_proc double_func, grib_expression *exp);

/* grib_expression_class_functor.c */
grib_expression *new_func_expression(grib_context *c, const char *name, grib_arguments *args);

/* grib_expression_class_accessor.c */
grib_expression *new_accessor_expression(grib_context *c, const char *name);

/* grib_expression_class_long.c */
grib_expression *new_long_expression(grib_context *c, long value);

/* grib_expression_class_double.c */
grib_expression *new_double_expression(grib_context *c, double value);

/* grib_expression_class_string.c */
grib_expression *new_string_expression(grib_context *c, const char *value);

/* grib_box.c */
grib_points *grib_box_get_points(grib_box *box, double north, double west, double south, double east, int *err);
int grib_box_init(grib_box *box, grib_handle *h, grib_arguments *args);
int grib_box_delete(grib_box *box);
grib_points *grib_points_new(grib_context *c, size_t size);
void grib_points_delete(grib_points *points);

/* grib_box_class.c */
grib_box *grib_box_factory(grib_handle *h, grib_arguments *args);

/* grib_box_class_gen.c */

/* grib_box_class_regular_gaussian.c */

/* grib_box_class_reduced_gaussian.c */

/* grib_nearest.c */
int grib_nearest_find(grib_nearest *nearest, grib_handle *h, double inlat, double inlon, unsigned long flags, double *outlats, double *outlons, double *values, double *distances, int *indexes, size_t *len);
int grib_nearest_init(grib_nearest *i, grib_handle *h, grib_arguments *args);
int grib_nearest_delete(grib_nearest *i);
void grib_binary_search(double xx[], const unsigned long n, double x, int *ju, int *jl);
double grib_nearest_distance(double radius, double lon1, double lat1, double lon2, double lat2);
int grib_nearest_find_multiple(grib_handle *h, int is_lsm, double *inlats, double *inlons, long npoints, double *outlats, double *outlons, double *values, double *distances, int *indexes);

/* grib_nearest_class.c */
grib_nearest *grib_nearest_factory(grib_handle *h, grib_arguments *args);

/* grib_nearest_class_gen.c */

/* grib_nearest_class_regular.c */

/* grib_nearest_class_reduced.c */

/* grib_nearest_class_latlon_reduced.c */

/* grib_nearest_class_sh.c */

/* grib_iterator_class_polar_stereographic.c */

/* grib_iterator_class_lambert_azimuthal_equal_area.c */
/* grib_iterator_class_lambert_conformal.c */

/* grib_iterator.c */
int grib_get_data(grib_handle *h, double *lats, double *lons, double *values, size_t *size);
int grib_iterator_next(grib_iterator *i, double *lat, double *lon, double *value);
int grib_iterator_has_next(grib_iterator *i);
int grib_iterator_previous(grib_iterator *i, double *lat, double *lon, double *value);
int grib_iterator_reset(grib_iterator *i);
int grib_iterator_init(grib_iterator *i, grib_handle *h, grib_arguments *args);
int grib_iterator_delete(grib_iterator *i);

/* grib_iterator_class.c */
grib_iterator *grib_iterator_factory(grib_handle *h, grib_arguments *args, unsigned long flags, int *ret);

/* grib_iterator_class_gaussian.c */

/* grib_iterator_class_gaussian_reduced.c */

/* grib_iterator_class_latlon_reduced.c */

/* grib_iterator_class_gen.c */

/* grib_iterator_class_latlon.c */

/* grib_iterator_class_regular.c */

/* grib_expression.c */
int grib_expression_native_type(grib_handle *h, grib_expression *g);
int grib_expression_evaluate_long(grib_handle *h, grib_expression *g, long *result);
int grib_expression_evaluate_double(grib_handle *h, grib_expression *g, double *result);
const char *grib_expression_evaluate_string(grib_handle *h, grib_expression *g, char *buf, size_t *size, int *err);
const char *grib_expression_get_name(grib_expression *g);
void grib_expression_print(grib_context *ctx, grib_expression *g, grib_handle *f);
void grib_expression_compile(grib_expression *g, grib_compiler *f);
void grib_expression_free(grib_context *ctx, grib_expression *g);
void grib_expression_add_dependency(grib_expression *e, grib_accessor *observer);
int grib_expression_set_value(grib_handle *h, grib_expression *g, grib_values *v);
grib_arguments *grib_arguments_new(grib_context *c, grib_expression *g, grib_arguments *n);
void grib_arguments_free(grib_context *c, grib_arguments *g);
void grib_arguments_print(grib_context *c, grib_arguments *g, grib_handle *f);
const char *grib_arguments_get_name(grib_handle *h, grib_arguments *args, int n);
const char *grib_arguments_get_string(grib_handle *h, grib_arguments *args, int n);
long grib_arguments_get_long(grib_handle *h, grib_arguments *args, int n);
double grib_arguments_get_double(grib_handle *h, grib_arguments *args, int n);
grib_expression *grib_arguments_get_expression(grib_handle *h, grib_arguments *args, int n);

/* grib_util.c */
grib_handle *grib_util_sections_copy(grib_handle *hfrom, grib_handle *hto, int what, int *err);
grib_string_list *grib_util_get_param_id(const char *mars_param);
grib_string_list *grib_util_get_mars_param(const char *param_id);
grib_handle *grib_util_set_spec(grib_handle *h, const grib_util_grid_spec *spec, const grib_util_packing_spec *packing_spec, int flags, const double *data_values, size_t data_values_count, int *err);
int grib_moments(grib_handle *h, double east, double north, double west, double south, int order, double *moments, long *count);
int parse_keyval_string(char* grib_tool, char* arg, int values_required, int default_type, grib_values values[], int* count);

/* compile.c */
void grib_compile_flags(grib_compiler *c, long flags);
void grib_compile_action_branch(grib_action *a, grib_compiler *c, char *name);
void grib_compile_expression(grib_expression *e, grib_compiler *compiler);
void grib_compile_arguments(grib_arguments *a, grib_compiler *compiler);

/* functions.c */
long grib_op_eq(long a, long b);
long grib_op_ne(long a, long b);
long grib_op_lt(long a, long b);
long grib_op_gt(long a, long b);
long grib_op_and(long a, long b);
long grib_op_or(long a, long b);
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
const char *grib_binop_long_proc_name(grib_binop_long_proc proc);
const char *grib_binop_double_proc_name(grib_binop_double_proc proc);
const char *grib_unop_long_proc_name(grib_unop_long_proc proc);
const char *grib_unop_double_proc_name(grib_unop_double_proc proc);

/* grib_bits_any_endian.c */
unsigned long grib_decode_unsigned_long(const unsigned char *p, long *bitp, long nbits);
int grib_encode_unsigned_long(unsigned char *p, unsigned long val, long *bitp, long nbits);
int grib_encode_unsigned_longb(unsigned char *p, unsigned long val, long *bitp, long nb);

/* grib_bits_any_endian_simple.c */
int grib_decode_long_array(const unsigned char *p, long *bitp, long bitsPerValue, size_t n_vals, long *val);
int grib_decode_double_array(const unsigned char *p, long *bitp, long bitsPerValue, double reference_value, double s, double d, size_t n_vals, double *val);
int grib_decode_double_array_complex(const unsigned char *p, long *bitp, long nbits, double reference_value, double s, double *d, size_t size, double *val);
int grib_encode_long_array(size_t n_vals, const long *val, long bits_per_value, unsigned char *p, long *off);
int grib_encode_double_array(size_t n_vals, const double *val, long bits_per_value, double reference_value, double d, double divisor, unsigned char *p, long *off);
int grib_encode_double_array_complex(size_t n_vals, double *val, long nbits, double reference_value, double *scal, double d, double divisor, unsigned char *p, long *bitp);
