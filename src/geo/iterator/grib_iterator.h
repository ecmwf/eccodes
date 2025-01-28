/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include <string>

#include "grib_api_internal.h"

// GRIB geoiterator, class supporting geographic iteration of values on a GRIB message

namespace eccodes::geo_iterator
{

class Iterator
{
public:
    virtual ~Iterator();

    virtual int next(double*, double*, double*) const     = 0;
    virtual int previous(double*, double*, double*) const = 0;
    virtual int reset()                                   = 0;
    virtual bool has_next() const                         = 0;

    // labeled deprecated so not to use
    [[deprecated("Iterator::destroy is not functional")]] int destroy() const { return GRIB_SUCCESS; }

    unsigned long flags_ = 0;

protected:
    Iterator(grib_handle*, grib_arguments*, unsigned long flags, int& err);

    grib_handle* h_;
    double* data_;    // data values
    mutable long e_;  // current element
    size_t nv_;       // number of values
};

eccodes::geo_iterator::Iterator* gribIteratorNew(const grib_handle*, unsigned long, int*);
int gribIteratorDelete(eccodes::geo_iterator::Iterator*);

}  // namespace eccodes::geo_iterator
