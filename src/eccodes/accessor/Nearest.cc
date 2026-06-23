/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Nearest.h"

eccodes::AccessorBuilder<eccodes::accessor::Nearest> _grib_accessor_nearest_builder{};

namespace eccodes::accessor
{

void Nearest::init(const long l, grib_arguments* args)
{
    Gen::init(l, args);
    args_ = args;
}

void Nearest::dump(eccodes::Dumper* dumper)
{
    /* TODO: pass args */
    dumper->dump_label(this, NULL);
}


}  // namespace eccodes::accessor
