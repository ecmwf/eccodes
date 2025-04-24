/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "PadTo.h"

eccodes::accessor::PadTo _grib_accessor_padto;
eccodes::Accessor* grib_accessor_padto = &_grib_accessor_padto;

namespace eccodes::accessor
{

size_t PadTo::preferred_size(int from_handle)
{
    long length = 0;
    long theEnd;

    expression_->evaluate_long(get_enclosing_handle(), &theEnd);

    length = theEnd - offset_;

    /* printf("preferred_size: prefered: %ld current:%ld %s %s %ld\n", (long)length,(long)a->length,a->cclass->name,a->name,(long)offset_ ); */

    return length > 0 ? length : 0;
}

void PadTo::init(const long len, grib_arguments* arg)
{
    Padding::init(len, arg);

    expression_ = arg->get_expression(get_enclosing_handle(), 0);
    length_     = preferred_size(1);
}

void PadTo::dump(eccodes::Dumper* dumper)
{
    /*dumper->dump_string(a,NULL);*/
}

}  // namespace eccodes::accessor
