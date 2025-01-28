/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_iterator_class_unstructured.h"
#include "grib_iterator_factory.h"

namespace eccodes::geo_iterator
{

FactoryBuilderGeneric<Unstructured> __builder("unstructured");

#define ITER "Unstructured grid Geoiterator"

int Unstructured::next(double* lat, double* lon, double* val) const
{
    if (e_ >= static_cast<long>(nv_ - 1)) {
        return 0;
    }
    e_++;

    *lat = lats_[e_];
    *lon = lons_[e_];
    if (val && data_) {
        *val = data_[e_];
    }
    return 1;
}

Unstructured::Unstructured(grib_handle* h, grib_arguments* args, unsigned long flags, int& err) :
    Gen(h, args, flags, err)
{
    if (err != GRIB_SUCCESS) {
        return;
    }

    const char* s_uuidOfHGrid = args->get_name(h, carg_++);
    char uuidOfHGrid[32]      = {
        0,
    };
    auto slen = sizeof(uuidOfHGrid);
    if ((err = grib_get_string_internal(h, s_uuidOfHGrid, uuidOfHGrid, &slen)) != GRIB_SUCCESS) {
        return;
    }

    lats_ = (double*)grib_context_malloc(h->context, nv_ * sizeof(double));
    if (!lats_) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nv_ * sizeof(double));
        err = GRIB_OUT_OF_MEMORY;
        return;
    }
    lons_ = (double*)grib_context_malloc(h->context, nv_ * sizeof(double));
    if (!lons_) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nv_ * sizeof(double));
        err = GRIB_OUT_OF_MEMORY;
        return;
    }

    e_ = -1;
}

int Unstructured::destroy()
{
    DEBUG_ASSERT(h_);
    const grib_context* c = h_->context;
    grib_context_free(c, lats_);
    grib_context_free(c, lons_);

    return Gen::destroy();
}

}  // namespace eccodes::geo_iterator
