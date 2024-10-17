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

namespace eccodes
{
namespace grib
{
namespace geo
{

class Iterator
{
public:
    virtual ~Iterator() {}
    virtual int init(grib_handle*, grib_arguments*) = 0;
    virtual int next(double*, double*, double*)     = 0;
    virtual int previous(double*, double*, double*) = 0;
    virtual int reset()                             = 0;
    virtual int destroy()                           = 0;
    virtual bool has_next()                         = 0;
    virtual Iterator* create() const                = 0;

    unsigned long flags_;

protected:
    grib_context* context_;
    grib_handle* h_;
    double* data_; /**  data values */
    long e_;       /**  current element */
    size_t nv_;    /**  number of values */
    const char* class_name_;

private:
    grib_arguments* args_; /**  args of iterator */
    size_t size_;
    int inited_;
};

eccodes::grib::geo::Iterator* gribIteratorNew(const grib_handle*, unsigned long, int*);
int gribIteratorDelete(eccodes::grib::geo::Iterator*);

}  // namespace geo
}  // namespace grib
}  // namespace eccodes
