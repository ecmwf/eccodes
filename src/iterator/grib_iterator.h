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

#include "grib_api_internal.h"

/*! Grib geoiterator, structure supporting a geographic iteration of values on a GRIB message.
    \ingroup grib_iterator
*/

namespace eccodes {
namespace grib {
namespace geo {

class Iterator
{
public:
    virtual int init(grib_handle*, grib_arguments*) = 0;
    virtual int next(double*, double*, double*) = 0;
    virtual int previous(double*, double*, double*) = 0;
    virtual int reset() = 0;
    virtual int destroy() = 0;
    virtual long has_next() = 0;
    virtual Iterator* create() const = 0;

public:
    grib_context* context_;
    grib_arguments* args_; /**  args of iterator */
    grib_handle* h_;
    long e_;       /**  current element */
    size_t nv_;    /**  number of values */
    double* data_; /**  data values */
    unsigned long flags_;
    const char* class_name_;
    size_t size_;
    int inited_;
};

eccodes::grib::geo::Iterator* gribIteratorNew(const grib_handle*, unsigned long, int*);
int gribIteratorDelete(eccodes::grib::geo::Iterator*);

} // namespace geo
} // namespace grib
} // namespace eccodes
