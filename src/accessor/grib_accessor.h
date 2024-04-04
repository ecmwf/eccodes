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

#include "../grib_api_internal.h"
#include "../grib_value.h"

class grib_accessor
{
public:
    grib_accessor() : name(nullptr), name_space(nullptr), context(nullptr), h(nullptr), creator(nullptr), length(0), offset(0), parent(nullptr), next_(nullptr), previous_(nullptr), cclass(nullptr), flags(0), sub_section(nullptr), dirty(0), same(nullptr), loop(0), vvalue(nullptr), set(nullptr), parent_as_attribute(nullptr) {}
    virtual ~grib_accessor() {}

    virtual void init_accessor(const long, grib_arguments*) = 0;
    virtual void dump(grib_dumper* f) = 0;
    virtual int pack_missing() = 0;
    //virtual int grib_pack_zero(grib_accessor* a) = 0;
    virtual int is_missing_internal() = 0;
    virtual int pack_double(const double* v, size_t* len) = 0;
    virtual int pack_float(const float* v, size_t* len) = 0;
    virtual int pack_expression(grib_expression* e) = 0;
    virtual int pack_string(const char* v, size_t* len) = 0;
    virtual int pack_string_array(const char** v, size_t* len) = 0;
    virtual int pack_long(const long* v, size_t* len) = 0;
    virtual int pack_bytes(const unsigned char* v, size_t* len) = 0;
    virtual int unpack_bytes(unsigned char* v, size_t* len) = 0;
    virtual int unpack_double_subarray(double* v, size_t start, size_t len) = 0;
    virtual int unpack_double(double* v, size_t* len) = 0;
    virtual int unpack_float(float* v, size_t* len) = 0;
    virtual int unpack_double_element(size_t i, double* v) = 0;
    virtual int unpack_float_element(size_t i, float* v) = 0;
    virtual int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) = 0;
    virtual int unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) = 0;
    virtual int unpack_string(char* v, size_t* len) = 0;
    virtual int unpack_string_array(char** v, size_t* len) = 0;
    virtual int unpack_long(long* v, size_t* len) = 0;
    virtual long get_native_type() = 0;
    virtual long get_next_position_offset() = 0;
    virtual long string_length() = 0;
    virtual long byte_offset() = 0;
    virtual long byte_count() = 0;
    virtual int value_count(long* count) = 0;
    virtual int notify_change(grib_accessor* changed) = 0;
    virtual grib_accessor* clone(grib_section* s, int* err) = 0;
    virtual void update_size(size_t len) = 0;
    virtual int nearest_smaller_value(double val, double* nearest) = 0;
    virtual size_t preferred_size(int from_handle) = 0;
    virtual grib_accessor* next_accessor() = 0;
    virtual void resize(size_t new_size) = 0;
    virtual void destroy(grib_context* ct) = 0;

    virtual int compare_accessors(grib_accessor* a2, int compare_flags);
    virtual int add_attribute(grib_accessor* attr, int nest_if_clash);
    virtual grib_accessor* get_attribute_index(const char* name, int* index);
    virtual int has_attributes();
    virtual grib_accessor* get_attribute(const char* name);


    const char* name;       /** < name of the accessor */
    const char* name_space; /** < namespace to which the accessor belongs */
    grib_context* context;
    grib_handle* h;
    grib_action* creator;        /** < action that created the accessor */
    long length;                 /** < byte length of the accessor */
    long offset;                 /** < offset of the data in the buffer */
    grib_section* parent;        /** < section to which the accessor is attached */
    grib_accessor* next_;         /** < next accessor in list */
    grib_accessor* previous_;     /** < next accessor in list */
    grib_accessor_class* cclass; /** < behaviour of the accessor */
    unsigned long flags;         /** < Various flags */
    grib_section* sub_section;

    const char* all_names[MAX_ACCESSOR_NAMES];       /** < name of the accessor */
    const char* all_name_spaces[MAX_ACCESSOR_NAMES]; /** < namespace to which the accessor belongs */
    int dirty;

    grib_accessor* same;        /** < accessors with the same name */
    long loop;                  /** < used in lists */
    grib_virtual_value* vvalue; /** < virtual value used when transient flag on **/
    const char* set;
    grib_accessor* attributes[MAX_ACCESSOR_ATTRIBUTES]; /** < attributes are accessors */
    grib_accessor* parent_as_attribute;
};


class grib_accessor_class
{
public:
    const char* name;

    grib_accessor_class(const char* name) : name(name){}
    virtual ~grib_accessor_class(){}

    virtual grib_accessor* create_empty_accessor() = 0;
    virtual grib_section* sub_section(grib_accessor* a) = 0;
    virtual int get_native_type(grib_accessor*) = 0;
    virtual int pack_missing(grib_accessor*) = 0;
    virtual int is_missing(grib_accessor*) = 0;
    virtual int pack_bytes(grib_accessor*, const unsigned char*, size_t* len) = 0;
    virtual int pack_double(grib_accessor*, const double* val, size_t* len) = 0;
    virtual int pack_float(grib_accessor*, const float* val, size_t* len) = 0;
    virtual int pack_long(grib_accessor*, const long* val, size_t* len) = 0;
    virtual int pack_string(grib_accessor*, const char*, size_t* len) = 0;
    virtual int pack_string_array(grib_accessor*, const char**, size_t* len) = 0;
    virtual int pack_expression(grib_accessor*, grib_expression*) = 0;
    virtual int unpack_bytes(grib_accessor*, unsigned char*, size_t* len) = 0;
    virtual int unpack_double(grib_accessor*, double* val, size_t* len) = 0;
    virtual int unpack_float(grib_accessor*, float* val, size_t* len) = 0;
    virtual int unpack_long(grib_accessor*, long* val, size_t* len) = 0;
    virtual int unpack_string(grib_accessor*, char*, size_t* len) = 0;
    virtual int unpack_string_array(grib_accessor*, char**, size_t* len) = 0;
    virtual size_t string_length(grib_accessor*) = 0;
    virtual long byte_count(grib_accessor*) = 0;
    virtual long byte_offset(grib_accessor*) = 0;
    virtual long next_offset(grib_accessor*) = 0;
    virtual int value_count(grib_accessor*, long*) = 0;
    virtual void destroy(grib_context*, grib_accessor*) = 0;
    virtual void dump(grib_accessor*, grib_dumper*) = 0;
    virtual void init(grib_accessor*, const long, grib_arguments*) = 0;
    virtual void post_init(grib_accessor*) = 0;
    virtual int notify_change(grib_accessor*, grib_accessor*) = 0;
    virtual void update_size(grib_accessor*, size_t) = 0;
    virtual size_t preferred_size(grib_accessor*, int) = 0;
    virtual void resize(grib_accessor*,size_t) = 0;
    virtual int nearest_smaller_value (grib_accessor*, double, double*) = 0;
    virtual grib_accessor* next(grib_accessor*, int) = 0;
    virtual int compare(grib_accessor*, grib_accessor*) = 0;
    virtual int unpack_double_element(grib_accessor*, size_t i, double* val) = 0;
    virtual int unpack_float_element(grib_accessor*, size_t i, float* val) = 0;
    virtual int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array) = 0;
    virtual int unpack_float_element_set(grib_accessor*, const size_t* index_array, size_t len, float* val_array) = 0;
    virtual int unpack_double_subarray(grib_accessor*, double* val, size_t start, size_t len) = 0;
    virtual int clear(grib_accessor*) = 0;
    virtual grib_accessor* make_clone(grib_accessor*, grib_section*, int*) = 0;
};


