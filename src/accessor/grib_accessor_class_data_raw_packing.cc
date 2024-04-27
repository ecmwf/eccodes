
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_raw_packing.h"
#include "grib_ieeefloat.h"

grib_accessor_class_data_raw_packing_t _grib_accessor_class_data_raw_packing{"data_raw_packing"};
grib_accessor_class* grib_accessor_class_data_raw_packing = &_grib_accessor_class_data_raw_packing;


void grib_accessor_class_data_raw_packing_t::init(grib_accessor* a, const long v, grib_arguments* args){
    grib_accessor_class_values_t::init(a, v, args);
    grib_accessor_data_raw_packing_t* self = (grib_accessor_data_raw_packing_t*)a;

    self->number_of_values = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    self->precision        = grib_arguments_get_name(grib_handle_of_accessor(a), args, self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_raw_packing_t::value_count(grib_accessor* a, long* n_vals){
    grib_accessor_data_raw_packing_t* self = (grib_accessor_data_raw_packing_t*)a;
    *n_vals                              = 0;
    return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_values, n_vals);
}

int grib_accessor_class_data_raw_packing_t::unpack_double(grib_accessor* a, double* val, size_t* len){
    grib_accessor_data_raw_packing_t* self = (grib_accessor_data_raw_packing_t*)a;
    unsigned char* buf                   = NULL;
    int bytes                            = 0;
    size_t nvals                         = 0;
    long inlen                           = a->byte_count();
    long precision = 0;

    int code = GRIB_SUCCESS;

    if ((code = grib_get_long(grib_handle_of_accessor(a), self->precision, &precision)) != GRIB_SUCCESS)
        return code;

    self->dirty = 0;

    buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    buf += a->byte_offset();
    switch (precision) {
        case 1:
            bytes = 4;
            break;
        case 2:
            bytes = 8;
            break;
        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    nvals = inlen / bytes;

    if (*len < nvals)
        return GRIB_ARRAY_TOO_SMALL;

    code = grib_ieee_decode_array<double>(a->context, buf, nvals, bytes, val);

    *len = nvals;

    return code;
}

int grib_accessor_class_data_raw_packing_t::pack_double(grib_accessor* a, const double* val, size_t* len){
    grib_accessor_data_raw_packing_t* self = (grib_accessor_data_raw_packing_t*)a;

    int bytes             = 0;
    unsigned char* buffer = NULL;

    long precision = 0;

    double* values = (double*)val;
    size_t inlen   = *len;

    /*int free_buffer = 0;
     *int free_values = 0;*/

    int code = GRIB_SUCCESS;

    size_t bufsize = 0;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((code = grib_get_long_internal(grib_handle_of_accessor(a), self->precision, &precision)) != GRIB_SUCCESS)
        return code;

    self->dirty = 1;

    switch (precision) {
        case 1:
            bytes = 4;
            break;

        case 2:
            bytes = 8;
            break;

        default:
            code = GRIB_NOT_IMPLEMENTED;
            goto clean_up;
    }

    bufsize = bytes * inlen;

    buffer = (unsigned char*)grib_context_malloc(a->context, bufsize);

    if (!buffer) {
        code = GRIB_OUT_OF_MEMORY;
        goto clean_up;
    }

    code = grib_ieee_encode_array(a->context, values, inlen, bytes, buffer);

clean_up:
    /*if (free_buffer)
     *  free(buffer);
     * if (free_values)
     *  free(values);
    */

    grib_buffer_replace(a, buffer, bufsize, 1, 1);

    grib_context_buffer_free(a->context, buffer);

    if (code == GRIB_SUCCESS) {
        code = grib_set_long(grib_handle_of_accessor(a), self->number_of_values, inlen);
        if (code == GRIB_READ_ONLY)
            code = 0;
    }

    return code;
}

int grib_accessor_class_data_raw_packing_t::unpack_double_element(grib_accessor* a, size_t idx, double* val){
    int ret                              = 0;
    grib_accessor_data_raw_packing_t* self = (grib_accessor_data_raw_packing_t*)a;
    unsigned char* buf                   = NULL;
    int bytes                            = 0;
    size_t nvals                         = 0;
    long inlen                           = a->byte_count();    long pos                             = 0;
    long precision                       = 0;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->precision, &precision)) != GRIB_SUCCESS)
        return ret;

    self->dirty = 0;

    buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    buf += a->byte_offset();
    switch (precision) {
        case 1:
            bytes = 4;
            break;

        case 2:
            bytes = 8;
            break;

        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    pos = bytes * idx;

    Assert(pos <= inlen);

    nvals = 1;
    buf += pos;

    ret = grib_ieee_decode_array(a->context, buf, nvals, bytes, val);

    return ret;
}

int grib_accessor_class_data_raw_packing_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array){
    int err = 0;
    size_t i = 0;
    for (i=0; i<len; ++i) {
        if ((err = unpack_double_element(a, index_array[i], val_array+i)) != GRIB_SUCCESS)
            return err;
    }
    return GRIB_SUCCESS;
}
