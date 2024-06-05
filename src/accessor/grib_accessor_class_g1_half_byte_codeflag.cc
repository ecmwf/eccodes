
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_g1_half_byte_codeflag.h"

grib_accessor_class_g1_half_byte_codeflag_t _grib_accessor_class_g1_half_byte_codeflag{"g1_half_byte_codeflag"};
grib_accessor_class* grib_accessor_class_g1_half_byte_codeflag = &_grib_accessor_class_g1_half_byte_codeflag;


void grib_accessor_class_g1_half_byte_codeflag_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_gen_t::init(a, len, arg);
    a->length = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

void grib_accessor_class_g1_half_byte_codeflag_t::dump(grib_accessor* a, grib_dumper* dumper){
    grib_dump_long(dumper, a, NULL);
}

int grib_accessor_class_g1_half_byte_codeflag_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    unsigned char dat = 0;
    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    dat = grib_handle_of_accessor(a)->buffer->data[a->offset] & 0x0f;

    *val = dat;
    *len = 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_g1_half_byte_codeflag_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    int ret = 0;
    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    /*  printf("HALF BYTE pack long %ld %02x\n",*val,grib_handle_of_accessor(a)->buffer->data[a->offset]);*/
    grib_handle_of_accessor(a)->buffer->data[a->offset] = (a->parent->h->buffer->data[a->offset] & 0xf0) | (*val & 0x0f);
    /*  printf("HALF BYTE pack long %ld %02x\n",*val,grib_handle_of_accessor(a)->buffer->data[a->offset]);*/

    *len = 1;
    return ret;
}

int grib_accessor_class_g1_half_byte_codeflag_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}
