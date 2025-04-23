/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Transient.h"

eccodes::accessor::Transient _grib_accessor_transient;
eccodes::Accessor* grib_accessor_transient = &_grib_accessor_transient;

namespace eccodes::accessor
{

void Transient::init(const long l, grib_arguments* args)
{
    Variable::init(l, args);
}

}  // namespace eccodes::accessor
