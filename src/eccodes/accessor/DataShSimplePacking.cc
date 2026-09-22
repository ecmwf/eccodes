/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "DataShSimplePacking.h"

eccodes::AccessorBuilder<eccodes::accessor::DataShSimplePacking> _grib_accessor_data_shsimple_packing_builder{};

namespace eccodes::accessor
{

void DataShSimplePacking::init(const long v, grib_arguments* args)
{
    Gen::init(v, args);

    coded_values_ = args->get_name(get_enclosing_handle(), 0);
    real_part_    = args->get_name(get_enclosing_handle(), 1);
    flags_ |= GRIB_ACCESSOR_FLAG_DATA;

    length_ = 0;
}

void DataShSimplePacking::dump(eccodes::Dumper* dumper)
{
    dumper->dump_values(this);
}

int DataShSimplePacking::pack_double(const double* val, size_t* len)
{
    int err = GRIB_SUCCESS;

    size_t coded_n_vals = *len - 1;
    size_t n_vals       = *len;

    dirty_ = 1;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if ((err = grib_set_double_internal(get_enclosing_handle(), real_part_, *val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_set_double_array_internal(get_enclosing_handle(), coded_values_, val, coded_n_vals)) != GRIB_SUCCESS)
        return err;

    *len = n_vals;

    return err;
}

long DataShSimplePacking::get_native_type()
{
    return GRIB_TYPE_DOUBLE;
}

}  // namespace eccodes::accessor
