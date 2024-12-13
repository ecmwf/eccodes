/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_message_is_valid.h"

grib_accessor_message_is_valid_t _grib_accessor_message_is_valid{};
grib_accessor* grib_accessor_message_is_valid = &_grib_accessor_message_is_valid;

void grib_accessor_message_is_valid_t::init(const long l, grib_arguments* c)
{
    grib_accessor_long_t::init(l, c);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

static int check_grid_pl_array(grib_handle* h)
{
    int ret = GRIB_SUCCESS;
    long Ni = 0,plpresent  = 0;
    long* pl = NULL; // pl array
    size_t plsize = 0;
    grib_context* c = h->context;

    // is there a PL array?
    ret = grib_get_long(h, "PLPresent", &plpresent);
    if (ret != GRIB_SUCCESS || plpresent == 0)
        return GRIB_SUCCESS;

    if ((ret = grib_get_size(h, "pl", &plsize)) != GRIB_SUCCESS)
        return ret;
    if (plsize == 0) { // pl array must have at least one element
        return GRIB_WRONG_GRID;
    }

    // If we have a PL array and PLPresent=true, then Ni must be missing
    ret = grib_get_long(h, "Ni", &Ni);
    if (ret == GRIB_SUCCESS && Ni != GRIB_MISSING_LONG) {
        grib_context_log(c, GRIB_LOG_ERROR, "Invalid Ni: If there is a PL array, Ni must be set to MISSING");
        return GRIB_WRONG_GRID;
    }

    pl = (long*)grib_context_malloc_clear(c, sizeof(long) * plsize);
    if (!pl) {
        return GRIB_OUT_OF_MEMORY;
    }
    if ((ret = grib_get_long_array(h, "pl", pl, &plsize)) != GRIB_SUCCESS)
        return ret;

    for (size_t j = 0; j < plsize; j++) {
        if (pl[j] == 0) {
            grib_context_log(c, GRIB_LOG_ERROR, "Invalid PL array: entry at index=%d is zero", j);
            return GRIB_WRONG_GRID;
        }
    }
    grib_context_free(c, pl);

    return GRIB_SUCCESS;
}

int grib_accessor_message_is_valid_t::unpack_long(long* val, size_t* len)
{
    grib_handle* h = grib_handle_of_accessor(this);
    *len = 1;
    *val = 0;

    // TODO: Move all checks to dedicated classes, e.g.,
    //  GribDataIntegrityChecker
    //  BufrDataIntegrityChecker
    // etc.
    int ret = check_grid_pl_array(h); // Just prototyping
    if (ret == GRIB_SUCCESS) {
        *val = 1;
    }

    return GRIB_SUCCESS;
}
