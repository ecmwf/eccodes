/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataG1ShSimplePacking.h"

namespace eccodes::accessor
{

int DataG1ShSimplePacking::value_count(long* count)
{
    size_t len = 0;
    int err = 0;

    err = grib_get_size(get_enclosing_handle(), coded_values_, &len);
    len += 1;

    *count = len;
    return err;
}

}  // namespace eccodes::accessor
