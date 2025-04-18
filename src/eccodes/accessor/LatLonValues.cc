/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "LatLonValues.h"

eccodes::accessor::LatLonvalues _grib_accessor_latlonvalues;
eccodes::Accessor* grib_accessor_latlonvalues = &_grib_accessor_latlonvalues;

namespace eccodes::accessor
{

void LatLonvalues::init(const long l, grib_arguments* c)
{
    Double::init(l, c);
    int n = 0;

    values_ = c->get_name(get_enclosing_handle(), n++);

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int LatLonvalues::unpack_double(double* val, size_t* len)
{
    int err         = 0;
    double* v       = val;
    double lat, lon, value;
    size_t size         = 0;
    long count          = 0;
    grib_iterator* iter = grib_iterator_new(get_enclosing_handle(), 0, &err);
    if (err) {
        grib_iterator_delete(iter);
        grib_context_log(context_, GRIB_LOG_ERROR, "latlonvalues: Unable to create iterator");
        return err;
    }

    err = value_count(&count);
    if (err) return err;
    size = count;

    if (*len < size) {
        grib_iterator_delete(iter);
        return GRIB_ARRAY_TOO_SMALL;
    }

    while (grib_iterator_next(iter, &lat, &lon, &value)) {
        *(v++) = lat;
        *(v++) = lon;
        *(v++) = value;
    }

    grib_iterator_delete(iter);

    *len = size;

    return GRIB_SUCCESS;
}

int LatLonvalues::value_count(long* count)
{
    grib_handle* h = get_enclosing_handle();
    int ret = GRIB_SUCCESS;
    size_t size = 0;
    if ((ret = grib_get_size(h, values_, &size)) != GRIB_SUCCESS) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "latlonvalues: Unable to get size of %s", values_);
        return ret;
    }

    *count = 3 * size;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
