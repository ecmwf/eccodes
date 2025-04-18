/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "PadToMultiple.h"

eccodes::accessor::PadToMultiple _grib_accessor_padtomultiple;
eccodes::Accessor* grib_accessor_padtomultiple = &_grib_accessor_padtomultiple;

namespace eccodes::accessor
{

size_t PadToMultiple::preferred_size(int from_handle)
{
    long padding  = 0;
    long begin    = 0;
    long multiple = 0;

    begin_->evaluate_long(get_enclosing_handle(), &begin);
    multiple_->evaluate_long(get_enclosing_handle(), &multiple);

    padding = offset_ - begin;
    padding = ((padding + multiple - 1) / multiple) * multiple - padding;

    return padding == 0 ? multiple : padding;
}

void PadToMultiple::init(const long len, grib_arguments* arg)
{
    Padding::init(len, arg);

    begin_    = arg->get_expression(get_enclosing_handle(), 0);
    multiple_ = arg->get_expression(get_enclosing_handle(), 1);
    length_   = preferred_size(1);
}

}  // namespace eccodes::accessor
