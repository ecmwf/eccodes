#pragma once

#include "../grib_api_internal.h"
#include "grib_accessor.h"

class grib_accessor_gen_t : public grib_accessor {
    void init_accessor(const long, grib_arguments*) override;
    void dump(grib_dumper* f) override;
    int pack_missing() override;
    //int grib_pack_zero(grib_accessor* a) override;
    int is_missing_internal() override;
    int pack_double(const double* v, size_t* len) override;
    int pack_float(const float* v, size_t* len) override;
    int pack_expression(grib_expression* e) override;
    int pack_string(const char* v, size_t* len) override;
    int pack_string_array(const char** v, size_t* len) override;
    int pack_long(const long* v, size_t* len) override;
    int pack_bytes(const unsigned char* v, size_t* len) override;
    int unpack_bytes(unsigned char* v, size_t* len) override;
    int unpack_double_subarray(double* v, size_t start, size_t len) override;
    int unpack_double(double* v, size_t* len) override;
    int unpack_float(float* v, size_t* len) override;
    int unpack_double_element(size_t i, double* v) override;
    int unpack_float_element(size_t i, float* v) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;
    int unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) override;
    int unpack_string(char* v, size_t* len) override;
    int unpack_string_array(char** v, size_t* len) override;
    int unpack_long(long* v, size_t* len) override;
    long get_native_type() override;
    long get_next_position_offset() override;
    long string_length() override;
    long byte_offset() override;
    long byte_count() override;
    int value_count(long* count) override;
    int notify_change(grib_accessor* changed) override;
    grib_accessor* clone(grib_section* s, int* err) override;
    void update_size(size_t len) override;
    int nearest_smaller_value(double val, double* nearest) override;
    size_t preferred_size(int from_handle) override;
    grib_accessor* next_accessor() override;
    void resize(size_t new_size) override;
    void destroy(grib_context* ct) override;
};

class grib_accessor_class_gen_t : public grib_accessor_class
{
public:
    grib_accessor_class_gen_t(const char* name) : grib_accessor_class{name} {}
    ~grib_accessor_class_gen_t();

    grib_accessor* create_empty_accessor() override{ return new grib_accessor_gen_t(); }
    grib_section* sub_section(grib_accessor* a) override;
    int get_native_type(grib_accessor*) override;
    int pack_missing(grib_accessor*) override;
    int is_missing(grib_accessor*) override;
    int pack_bytes(grib_accessor*, const unsigned char*, size_t* len) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int pack_float(grib_accessor*, const float* val, size_t* len) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int pack_string(grib_accessor*, const char*, size_t* len) override;
    int pack_string_array(grib_accessor*, const char**, size_t* len) override;
    int pack_expression(grib_accessor*, grib_expression*) override;
    int unpack_bytes(grib_accessor*, unsigned char*, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    int unpack_string_array(grib_accessor*, char**, size_t* len) override;
    size_t string_length(grib_accessor*) override;
    long byte_count(grib_accessor*) override;
    long byte_offset(grib_accessor*) override;
    long next_offset(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    void post_init(grib_accessor*) override;
    int notify_change(grib_accessor*, grib_accessor*) override;
    void update_size(grib_accessor*, size_t) override;
    size_t preferred_size(grib_accessor*, int) override;
    void resize(grib_accessor*, size_t) override;
    int nearest_smaller_value(grib_accessor*, double, double*) override;
    grib_accessor* next(grib_accessor*, int) override;
    int compare(grib_accessor*, grib_accessor*) override;
    int unpack_double_element(grib_accessor*, size_t i, double* val) override;
    int unpack_float_element(grib_accessor*, size_t i, float* val) override;
    int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) override;
    int unpack_float_element_set(grib_accessor*, const size_t* index_array, size_t len, float* val_array) override;
    int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len) override;
    int clear(grib_accessor*) override;
    grib_accessor* make_clone(grib_accessor*, grib_section*, int*) override;
};
