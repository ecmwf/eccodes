/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Pad.h"

eccodes::accessor::Pad _grib_accessor_pad;
eccodes::Accessor* grib_accessor_pad = &_grib_accessor_pad;

namespace eccodes::accessor
{

void Pad::init(const long len, grib_arguments* arg)
{
    Padding::init(len, arg);

    expression_ = arg->get_expression(get_enclosing_handle(), 0);
    length_     = preferred_size(1);
}

size_t Pad::preferred_size(int from_handle)
{
    long length = 0;

    expression_->evaluate_long(get_enclosing_handle(), &length);

    return length > 0 ? length : 0;
}

}  // namespace eccodes::accessor
