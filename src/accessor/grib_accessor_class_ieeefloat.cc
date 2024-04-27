
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_ieeefloat.h"

grib_accessor_class_ieeefloat_t _grib_accessor_class_ieeefloat{"ieeefloat"};
grib_accessor_class* grib_accessor_class_ieeefloat = &_grib_accessor_class_ieeefloat;


void grib_accessor_class_ieeefloat_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_double_t::init(a, len, arg);
    grib_accessor_ieeefloat_t* self = (grib_accessor_ieeefloat_t*)a;
    long count                    = 0;
    self->arg                     = arg;
    a->value_count(&count);    a->length = 4 * count;
    Assert(a->length >= 0);
}

int grib_accessor_class_ieeefloat_t::value_count(grib_accessor* a, long* len){
    grib_accessor_ieeefloat_t* self = (grib_accessor_ieeefloat_t*)a;
    *len = 0;

    if (!self->arg) {
        *len = 1;
        return 0;
    }
    return grib_get_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), len);
}

int grib_accessor_class_ieeefloat_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_ieeefloat_t* self = (grib_accessor_ieeefloat_t*)a;
    int ret                       = 0;
    unsigned long i               = 0;
    unsigned long rlen            = (unsigned long)*len;
    size_t buflen                 = 0;
    unsigned char* buf            = NULL;
    long off                      = 0;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it packs at least 1 value", a->name);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {
        off = a->offset * 8;
        ret = grib_encode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, grib_ieee_to_long(val[0]), &off, 32);
        if (*len > 1)
            grib_context_log(a->context, GRIB_LOG_WARNING, "ieeefloat: Trying to pack %zu values in a scalar %s, packing first value",
                            *len, a->name);
        if (ret == GRIB_SUCCESS)
            len[0] = 1;
        return ret;
    }

    buflen = rlen * 4;

    buf = (unsigned char*)grib_context_malloc(a->context, buflen);

    for (i = 0; i < rlen; i++) {
        grib_encode_unsigned_longb(buf, grib_ieee_to_long(val[i]), &off, 32);
    }
    ret = grib_set_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), rlen);

    if (ret == GRIB_SUCCESS)
        grib_buffer_replace(a, buf, buflen, 1, 1);
    else
        *len = 0;

    grib_context_free(a->context, buf);

    return ret;
}

int grib_accessor_class_ieeefloat_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    return unpack<double>(a, val, len);
}

int grib_accessor_class_ieeefloat_t::unpack_float(grib_accessor* a, float* val, size_t* len){
    return unpack<float>(a, val, len);
}

void grib_accessor_class_ieeefloat_t::update_size(grib_accessor* a, size_t s){
    a->length = (long)s;
    Assert(a->length >= 0);
}

int grib_accessor_class_ieeefloat_t::nearest_smaller_value(grib_accessor* a, double val, double* nearest){
    return grib_nearest_smaller_ieee_float(val, nearest);
}
