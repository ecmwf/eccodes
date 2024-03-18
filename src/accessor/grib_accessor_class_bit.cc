
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
#include "grib_accessor_class_bit.h"

grib_accessor_class_bit_t _grib_accessor_class_bit{"bit"};
grib_accessor_class* grib_accessor_class_bit = &_grib_accessor_class_bit;


void grib_accessor_class_bit_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_bit_t* self = (grib_accessor_bit_t*)a;
    a->length       = 0;
    self->owner     = grib_arguments_get_name(grib_handle_of_accessor(a), arg, 0);
    self->bit_index = grib_arguments_get_long(grib_handle_of_accessor(a), arg, 1);
}

int grib_accessor_class_bit_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_bit_t* self = (grib_accessor_bit_t*)a;
    int ret = 0;
    long data = 0;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_bit_t: unpack_long: Wrong size for %s, it contains %d values ", a->name, 1);
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->owner, &data)) != GRIB_SUCCESS) {
        *len = 0;
        return ret;
    }

    if (data & (1 << self->bit_index))
        *val = 1;
    else
        *val = 0;

    *len = 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_bit_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_bit_t* self = (grib_accessor_bit_t*)a;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_bit_t: pack_long: At least one value to pack for %s", a->name);
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    grib_accessor* owner = grib_find_accessor(grib_handle_of_accessor(a), self->owner);
    if (!owner) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "grib_accessor_bit_t: Cannot get the owner %s for computing the bit value of %s",
                        self->owner, a->name);
        *len = 0;
        return GRIB_NOT_FOUND;
    }

    unsigned char* mdata = grib_handle_of_accessor(a)->buffer->data;
    mdata += owner->byte_offset();
    /* Note: In the definitions, flagbit numbers go from 7 to 0 (the bit_index), while WMO convention is from 1 to 8 */
    if (a->context->debug) {
        /* Print bit positions from 1 (MSB) */
        fprintf(stderr, "ECCODES DEBUG Setting bit %d in %s to %d\n", 8 - self->bit_index, owner->name, (*val > 0) );
    }
    grib_set_bit(mdata, 7 - self->bit_index, *val > 0);

    *len = 1;
    return GRIB_SUCCESS;
}
