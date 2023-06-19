#ifndef _GRIB_ACCESSOR_IMPL_H_20230519_
#define _GRIB_ACCESSOR_IMPL_H_20230519_

#include <stddef.h> // size_t

struct grib_arguments;
struct grib_section;
struct grib_dumper;
struct grib_expression;
struct grib_accessor;

namespace eccodes {
    class grib_accessor_impl {
    public:
        virtual ~grib_accessor_impl() = default;
        virtual void init(const long len, grib_arguments* params) = 0;
        //virtual void destroy(grib_context* ct) = 0;
        virtual grib_section* sub_section() = 0;
        virtual int native_type() = 0;
        virtual int pack_missing() = 0;
        virtual int is_missing() = 0;
        virtual int pack_bytes(const unsigned char* val, size_t* len) = 0;
        virtual int pack_double(const double* val, size_t* len) = 0;
        virtual int pack_float(const float* val, size_t* len) = 0;
        virtual int pack_long(const long* val, size_t* len) = 0;
        virtual int pack_string(const char* v, size_t* len) = 0;
        virtual int pack_string_array(const char** v, size_t* len) = 0;
        virtual int pack_expression(grib_expression* e) = 0;
        virtual int unpack_bytes(unsigned char* val, size_t* len) = 0;
        virtual int unpack_double(double* val, size_t* len) = 0;
        virtual int unpack_float(float* val, size_t* len) = 0;
        virtual int unpack_long(long* val, size_t* len) = 0;
        virtual int unpack_string(char* v, size_t* len) = 0;
        virtual int unpack_string_array(char** v, size_t* len) = 0;
        virtual size_t string_length() = 0;
        virtual long byte_count() = 0;
        virtual long byte_offset() = 0;
        virtual long next_offset() = 0;
        virtual int value_count(long* count) = 0;
        virtual void dump(grib_dumper* dumper) = 0;
        virtual void post_init() = 0;
        virtual int notify_change(grib_accessor* observed) = 0;
        virtual void update_size(size_t s) = 0;
        virtual size_t preferred_size(int from_handle) = 0;
        virtual void resize(size_t new_size) = 0;
        virtual int nearest_smaller_value(double val, double* nearest) = 0;
        // Note: Renamed fron next() to avoid clash with member variable of the same name!
        virtual grib_accessor_impl* next_accessor(int mod) = 0;
        virtual int compare() = 0;
        virtual int unpack_double_element(size_t i, double* val) = 0;
        virtual int unpack_float_element(size_t i, float* val) = 0;
        virtual int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) = 0;
        virtual int unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) = 0;
        virtual int unpack_double_subarray(double* val, size_t start, size_t len) = 0;
        virtual int clear() = 0;
        virtual grib_accessor_impl* make_clone(grib_section* s, int* err) = 0;
    };

}

#endif // _GRIB_ACCESSOR_IMPL_H_20230519_
