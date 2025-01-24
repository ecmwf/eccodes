/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Evaluate.h"

eccodes::accessor::Evaluate _grib_accessor_evaluate;
eccodes::Accessor* grib_accessor_evaluate = &_grib_accessor_evaluate;

namespace eccodes::accessor
{

void Evaluate::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    arg_ = c; // the expression to be evaluated
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int Evaluate::unpack_long(long* val, size_t* len)
{
    if (!arg_) return GRIB_INVALID_ARGUMENT;

    grib_handle* h = grib_handle_of_accessor(this);
    grib_expression* e = arg_->get_expression(h, 0);

    int ret = e->evaluate_long(h, val);
    *len    = 1;

    return ret;
}

}  // namespace eccodes::accessor
