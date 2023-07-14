#ifndef _ACCESSOR_H_20230630_
#define _ACCESSOR_H_20230630_

#include "AccessorImpl.h"

namespace eccodes {

template<typename ACCESSOR_TYPE>
class Accessor : public AccessorImpl {

    ACCESSOR_TYPE accessor_;

public:
    operator grib_accessor*();
    operator ACCESSOR_TYPE*();

private:
    void dump(grib_dumper* dumper) override;
    long next_offset() override;
    size_t string_length() override;
    int value_count(long* count) override;
    long byte_count() override;
    long byte_offset() override;
    int get_native_type() override;
    grib_section* sub_section() override;
    int pack_missing() override;
    int is_missing() override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int pack_double(const double* val, size_t* len) override;
    int pack_float(const float* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_float(float* val, size_t* len) override;
    int pack_string(const char* v, size_t* len) override;
    int unpack_string(char* v, size_t* len) override;
    int pack_string_array(const char** v, size_t* len) override;
    int unpack_string_array(char** v, size_t* len) override;
    int pack_bytes(const unsigned char* val, size_t* len) override;
    int unpack_bytes(unsigned char* val, size_t* len) override;
    int pack_expression(grib_expression* e) override;
    int notify_change(grib_accessor* observed) override;
    void update_size(size_t s) override;
    size_t preferred_size(int from_handle) override;
    void resize(size_t new_size) override;
    int nearest_smaller_value(double val, double* nearest) override;
    grib_accessor* next(int mod) override;
    int compare(grib_accessor* b) override;
    int unpack_double_element(size_t i, double* val) override;
    int unpack_float_element(size_t i, float* val) override;
    int unpack_double_element_set(const size_t* index_array, size_t len, double* val_array) override;
    int unpack_float_element_set(const size_t* index_array, size_t len, float* val_array) override;
    int unpack_double_subarray(double* val, size_t start, size_t len) override;
    int clear() override;
    grib_accessor* make_clone(grib_section* s, int* err) override;

public:
    virtual ~AccessorImpl();
};

}

#endif // _ACCESSOR_H_20230630_
