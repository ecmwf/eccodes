#ifndef _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_
#define _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_

#include "grib_accessor_impl_gen.h"

namespace eccodes {
    class grib_accessor_impl_proj_string : public grib_accessor_impl_gen {
    public:
        // grib_accessor_proj_string data
        // 
        // In order to support interop with the existing C code, this should be
        // kept in sync with grib_accessor_class_proj_string
        const char* grid_type;
        int endpoint;

    public:
        grib_accessor_impl_proj_string(grib_section* p, grib_action* creator);
        void init(const long len, grib_arguments* params) override;
        grib_section* sub_section() override;
        int pack_missing() override;
        int is_missing() override;
        int pack_string_array(const char** v, size_t* len) override;
        int pack_expression(grib_expression* e) override;
        int unpack_bytes(unsigned char* val, size_t* len) override;
        int unpack_double(double* val, size_t* len) override;
        int unpack_float(float* val, size_t* len) override;
        int unpack_long(long* val, size_t* len) override;
        int unpack_string(char* val, size_t* len) override;
        int unpack_string_array(char** v, size_t* len) override;
        size_t string_length() override;
        long byte_count() override;
        long byte_offset() override;
        long next_offset() override;
        int value_count(long* count) override;
        void dump(grib_dumper* dumper) override;
        void post_init() override;
        int notify_change() override;
        void update_size(size_t s) override;
        size_t preferred_size(int from_handle) override;
        void resize(size_t new_size) override;
        int nearest_smaller_value (double val, double* nearest) override;
        grib_accessor_impl* next_accessor(int mod) override;
        int compare() override;
        int unpack_double_element(size_t i, double* val) override;
        int unpack_float_element(size_t i, float* val) override;
        int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;
        int unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) override;
        int unpack_double_subarray(double* val, size_t start, size_t len) override;
        int clear() override;
        grib_accessor_impl* make_clone(grib_section* s, int* err) override;

    protected:
        virtual void init_proj_string(const long len, grib_arguments* params);
    };
}

#endif // _GRIB_ACCESSOR_IMPL_PROJ_STRING_H_20230519_
