/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Constant.h"

eccodes::accessor::Constant _grib_accessor_constant;
eccodes::Accessor* grib_accessor_constant = &_grib_accessor_constant;

namespace eccodes::accessor
{

void Constant::init(const long len, grib_arguments* arg)
{
    Variable::init(len, arg);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

}  // namespace eccodes::accessor
