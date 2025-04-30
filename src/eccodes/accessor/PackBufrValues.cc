/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "PackBufrValues.h"

eccodes::AccessorBuilder<eccodes::accessor::PackBufrValues> _grib_accessor_pack_bufr_values_builder{};

namespace eccodes::accessor
{

void PackBufrValues::init(const long len, grib_arguments* params)
{
    Gen::init(len, params);
    char* key;
    key            = (char*)params->get_name(get_enclosing_handle(), 0);
    data_accessor_ = grib_find_accessor(get_enclosing_handle(), key);

    length_ = 0;
}

void PackBufrValues::dump(eccodes::Dumper* dumper)
{
}

int PackBufrValues::unpack_string_array(char** buffer, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int PackBufrValues::unpack_string(char* buffer, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int PackBufrValues::unpack_long(long* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int PackBufrValues::unpack_double(double* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int PackBufrValues::value_count(long* count)
{
    *count = 1;
    return 0;
}

void PackBufrValues::destroy(grib_context* context)
{
    Gen::destroy(context);
}

long PackBufrValues::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int PackBufrValues::pack_long(const long* val, size_t* len)
{
    grib_accessor* data = (grib_accessor*)data_accessor_;
    return data->pack_double(0, 0);
}

int PackBufrValues::pack_double(const double* val, size_t* len)
{
    grib_accessor* data = (grib_accessor*)data_accessor_;
    return data->pack_double(0, 0);
}

}  // namespace eccodes::accessor
