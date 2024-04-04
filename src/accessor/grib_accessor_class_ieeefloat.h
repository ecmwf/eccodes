
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_ieeefloat_h
#define eccodes_accessor_ieeefloat_h

#include "grib_accessor_class_double.h"

class grib_accessor_ieeefloat_t : public grib_accessor_double_t
{
public:
    grib_arguments* arg;
};

class grib_accessor_class_ieeefloat_t : public grib_accessor_class_double_t
{
public:
    grib_accessor_class_ieeefloat_t(const char* name) : grib_accessor_class_double_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_ieeefloat_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_float(grib_accessor*, float* val, size_t* len) override;
    int value_count(grib_accessor*, long*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
    void update_size(grib_accessor*, size_t) override;
    int nearest_smaller_value(grib_accessor* a, double val, double* nearest) override;

private:
    template <typename T> int unpack(grib_accessor*, T*, size_t*);
};


template <typename T>
int grib_accessor_class_ieeefloat_t::unpack(grib_accessor* a, T* val, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    long rlen = 0;
    int err   = 0;
    long i    = 0;
    long bitp = a->offset * 8;
    grib_handle* hand  = grib_handle_of_accessor(a);

    err = a->value_count(&rlen);    if (err)
        return err;

    if (*len < (size_t)rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size (%zu) for %s, it contains %ld values", *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < rlen; i++)
        val[i] = (T)grib_long_to_ieee(grib_decode_unsigned_long(hand->buffer->data, &bitp, 32));

    *len = rlen;
    return GRIB_SUCCESS;
}

#endif /* eccodes_accessor_ieeefloat_h */
