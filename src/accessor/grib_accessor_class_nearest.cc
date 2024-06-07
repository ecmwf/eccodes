/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_nearest.h"

grib_accessor_class_nearest_t _grib_accessor_class_nearest{"nearest"};
grib_accessor_class* grib_accessor_class_nearest = &_grib_accessor_class_nearest;


void grib_accessor_class_nearest_t::init(grib_accessor* a, const long l, grib_arguments* args)
{
    grib_accessor_class_gen_t::init(a, l, args);
    grib_accessor_nearest_t* self = (grib_accessor_nearest_t*)a;
    self->args                  = args;
}

void grib_accessor_class_nearest_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    /* TODO: pass args */
    grib_dump_label(dumper, a, NULL);
}

grib_nearest* grib_nearest_new(const grib_handle* ch, int* error)
{
    grib_handle* h            = (grib_handle*)ch;
    grib_accessor* a          = NULL;
    grib_accessor_nearest_t* na = NULL;
    grib_nearest* n           = NULL;
    *error                    = GRIB_NOT_IMPLEMENTED;
    a                         = grib_find_accessor(h, "NEAREST");
    na                        = (grib_accessor_nearest_t*)a;

    if (!a)
        return NULL;

    n = grib_nearest_factory(h, na->args, error);

    if (n)
        *error = GRIB_SUCCESS;

    return n;
}
