
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_ibmfloat.h"

grib_accessor_class_ibmfloat_t _grib_accessor_class_ibmfloat{"ibmfloat"};
grib_accessor_class* grib_accessor_class_ibmfloat = &_grib_accessor_class_ibmfloat;


void grib_accessor_class_ibmfloat_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_double_t::init(a, len, arg);
    grib_accessor_ibmfloat_t* self = (grib_accessor_ibmfloat_t*)a;
    long count = 0;

    self->arg = arg;
    a->value_count(&count);    a->length = 4 * count;
    Assert(a->length >= 0);
}

template <typename T>
int unpack(grib_accessor* a, T* val, size_t* len){
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    unsigned long rlen = 0;
    long count         = 0;
    int err            = 0;
    unsigned long i    = 0;
    long bitp          = a->offset * 8;
    grib_handle* hand  = grib_handle_of_accessor(a);

    err = a->value_count(&count);    if (err)
        return err;
    rlen = count;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size (%zu) for %s, it contains %lu values", *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < rlen; i++)
        val[i] = (T)grib_long_to_ibm(grib_decode_unsigned_long(hand->buffer->data, &bitp, 32));

    *len = rlen;
    return GRIB_SUCCESS;
}

int grib_accessor_class_ibmfloat_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    return unpack<double>(a, val, len);
}

int grib_accessor_class_ibmfloat_t::unpack_float(grib_accessor* a, float* val, size_t* len){
    return unpack<float>(a, val, len);
}

int grib_accessor_class_ibmfloat_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_ibmfloat_t* self = (grib_accessor_ibmfloat_t*)a;
    int ret                      = 0;
    unsigned long i              = 0;
    unsigned long rlen           = *len;
    size_t buflen                = 0;
    unsigned char* buf           = NULL;
    long off                     = 0;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it packs at least 1 value", a->name);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {

        //double x = 0;
        //grib_nearest_smaller_ibm_float(val[0],&x);
        //double y = grib_long_to_ibm(grib_ibm_to_long(val[0]));
        //printf("IBMFLOAT val=%.20f nearest_smaller_ibm_float=%.20f long_to_ibm=%.20f\n",val[0],x ,y);

        off = a->byte_offset() * 8;
        ret = grib_encode_unsigned_long(grib_handle_of_accessor(a)->buffer->data, grib_ibm_to_long(val[0]), &off, 32);
        if (*len > 1)
            grib_context_log(a->context, GRIB_LOG_WARNING, "ibmfloat: Trying to pack %zu values in a scalar %s, packing first value",
                             *len, a->name);
        if (ret == GRIB_SUCCESS)
            len[0] = 1;
        return ret;
    }

    buflen = rlen * 4;

    buf = (unsigned char*)grib_context_malloc(a->context, buflen);

    for (i = 0; i < rlen; i++) {
        grib_encode_unsigned_longb(buf, grib_ibm_to_long(val[i]), &off, 32);
    }
    ret = grib_set_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), rlen);

    if (ret == GRIB_SUCCESS)
        grib_buffer_replace(a, buf, buflen, 1, 1);
    else
        *len = 0;

    grib_context_free(a->context, buf);

    a->length = a->byte_count();

    return ret;
}

long grib_accessor_class_ibmfloat_t::byte_count(grib_accessor* a){
    return a->length;
}

int grib_accessor_class_ibmfloat_t::value_count(grib_accessor* a, long* len){
    grib_accessor_ibmfloat_t* self = (grib_accessor_ibmfloat_t*)a;
    *len = 0;
    if (!self->arg) {
        *len = 1;
        return 0;
    }
    return grib_get_long_internal(grib_handle_of_accessor(a), grib_arguments_get_name(a->parent->h, self->arg, 0), len);
}

long grib_accessor_class_ibmfloat_t::byte_offset(grib_accessor* a){
    return a->offset;
}

void grib_accessor_class_ibmfloat_t::update_size(grib_accessor* a, size_t s){
    a->length = (long)s;
    Assert(a->length >= 0);
}

long grib_accessor_class_ibmfloat_t::next_offset(grib_accessor* a){
    return a->byte_offset() + a->byte_count();}

int grib_accessor_class_ibmfloat_t::nearest_smaller_value(grib_accessor* a, double val, double* nearest){
    int ret = 0;
    if (grib_nearest_smaller_ibm_float(val, nearest) == GRIB_INTERNAL_ERROR) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "ibm_float:nearest_smaller_value overflow value=%g", val);
        grib_dump_content(grib_handle_of_accessor(a), stderr, "wmo", GRIB_DUMP_FLAG_HEXADECIMAL, 0);
        ret = GRIB_INTERNAL_ERROR;
    }
    return ret;
}
