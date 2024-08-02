/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_section_padding.h"

grib_accessor_class_section_padding_t _grib_accessor_class_section_padding{ "section_padding" };
grib_accessor_class* grib_accessor_class_section_padding = &_grib_accessor_class_section_padding;


size_t grib_accessor_class_section_padding_t::preferred_size(grib_accessor* a, int from_handle)
{
    grib_accessor_section_padding_t* self = (grib_accessor_section_padding_t*)a;

    grib_accessor* b              = a;
    grib_accessor* section_length = 0;
    long length                   = 0;
    size_t size                   = 1;
    long alength                  = 0;

    if (!from_handle) {
        if (self->preserve)
            return a->length;
        else
            return 0;
    }

    /* The section length should be a parameter */
    while (section_length == NULL && b != NULL) {
        section_length = b->parent->aclength;
        b = b->parent->owner;
    }

    if (!section_length) {
        /* printf("PADDING is no !section_length\n"); */
        return 0;
    }

    if (section_length->unpack_long(&length, &size) == GRIB_SUCCESS) {
        if (length)
            alength = length - a->offset + section_length->parent->owner->offset;
        else
            alength = 0;

        /*Assert(a->length>=0);*/

        if (alength < 0)
            alength = 0;

        /* printf("PADDING is %ld\n",a->length); */
    }
    else {
        /* printf("PADDING unpack fails\n"); */
    }

    return alength;
}

void grib_accessor_class_section_padding_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_padding_t::init(a, len, arg);
    grib_accessor_section_padding_t* self = (grib_accessor_section_padding_t*)a;
    self->preserve = 1; /* This should be a parameter */
    a->length      = preferred_size(a, 1);
}
