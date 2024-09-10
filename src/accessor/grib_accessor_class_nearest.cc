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

AccessorBuilder<grib_accessor_nearest_t> _grib_accessor_nearest_builder{};

void grib_accessor_nearest_t::init(const long l, grib_arguments* args)
{
    grib_accessor_gen_t::init(l, args);
    args_ = args;
}

void grib_accessor_nearest_t::dump(grib_dumper* dumper)
{
    /* TODO: pass args */
    grib_dump_label(dumper, this, NULL);
}

#if defined(HAVE_GEOGRAPHY)
grib_nearest* grib_nearest_new(const grib_handle* ch, int* error)
{
    grib_handle* h              = (grib_handle*)ch;
    grib_accessor* a            = NULL;
    grib_accessor_nearest_t* na = NULL;
    grib_nearest* n             = NULL;
    *error                      = GRIB_NOT_IMPLEMENTED;
    a                           = grib_find_accessor(h, "NEAREST");
    na                          = (grib_accessor_nearest_t*)a;

    if (!a)
        return NULL;

    n = grib_nearest_factory(h, na->args_, error);

    if (n)
        *error = GRIB_SUCCESS;

    return n;
}
#else
grib_nearest* grib_nearest_new(const grib_handle* ch, int* error)
{
    *error = GRIB_FUNCTIONALITY_NOT_ENABLED;
    grib_context_log(ch->context, GRIB_LOG_ERROR,
                     "Nearest neighbour functionality not enabled. Please rebuild with -DENABLE_GEOGRAPHY=ON");

    return NULL;
}
#endif
