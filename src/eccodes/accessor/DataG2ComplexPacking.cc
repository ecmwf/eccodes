/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataG2complexPacking.h"

eccodes::accessor::DataG2ComplexPacking _grib_accessor_data_g2complex_packing;
eccodes::Accessor* grib_accessor_data_g2complex_packing = &_grib_accessor_data_g2complex_packing;

namespace eccodes::accessor
{

void DataG2ComplexPacking::init(const long v, grib_arguments* args)
{
    DataComplexPacking::init(v, args);
    numberOfValues_ = args->get_name(grib_handle_of_accessor(this), carg_++);
    edition_        = 2;

    flags_ |= GRIB_ACCESSOR_FLAG_DATA;
}

int DataG2ComplexPacking::value_count(long* numberOfValues)
{
    *numberOfValues = 0;

    return grib_get_long(grib_handle_of_accessor(this), numberOfValues_, numberOfValues);
}

int DataG2ComplexPacking::pack_double(const double* val, size_t* len)
{
    int ret = GRIB_SUCCESS;

    if (*len == 0)
        return GRIB_NO_VALUES;

    ret = DataComplexPacking::pack_double(val, len);

    if (ret == GRIB_SUCCESS)
        ret = grib_set_long_internal(grib_handle_of_accessor(this), numberOfValues_, *len);

    return ret;
}

}  // namespace eccodes::accessor
