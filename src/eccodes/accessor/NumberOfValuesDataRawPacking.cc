/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "NumberOfValuesDataRawPacking.h"

eccodes::AccessorBuilder<eccodes::accessor::NumberOfValuesDataRawPacking> _grib_accessor_number_of_values_data_raw_packing_builder{};

namespace eccodes::accessor
{

void NumberOfValuesDataRawPacking::init(const long v, grib_arguments* args)
{
    Gen::init(v, args);
    int n = 0;

    values_    = args->get_name(get_enclosing_handle(), n++);
    precision_ = args->get_name(get_enclosing_handle(), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

int NumberOfValuesDataRawPacking::unpack_long(long* val, size_t* len)
{
    int err              = 0;
    grib_accessor* adata = NULL;
    long precision       = 0;
    int bytes            = 0;
    long byte_count      = 0;

    adata = grib_find_accessor(get_enclosing_handle(), values_);
    ECCODES_ASSERT(adata != NULL);
    byte_count = adata->byte_count();
    if ((err = grib_get_long_internal(get_enclosing_handle(), precision_, &precision)) != GRIB_SUCCESS)
        return err;

    switch (precision) {
        case 1:
            bytes = 4;
            break;

        case 2:
            bytes = 8;
            break;

        default:
            return GRIB_NOT_IMPLEMENTED;
    }

    *val = byte_count / bytes;

    return err;
}

long NumberOfValuesDataRawPacking::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
