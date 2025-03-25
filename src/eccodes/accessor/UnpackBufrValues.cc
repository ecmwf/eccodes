/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "UnpackBufrValues.h"

eccodes::AccessorBuilder<eccodes::accessor::UnpackBufrValues> _grib_accessor_unpack_bufr_values_builder{};

namespace eccodes::accessor
{

void UnpackBufrValues::init(const long len, grib_arguments* params)
{
    Gen::init(len, params);
    char* key;
    key            = (char*)params->get_name(grib_handle_of_accessor(this), 0);
    data_accessor_ = dynamic_cast<BufrDataArray*>(grib_find_accessor(grib_handle_of_accessor(this), key));

    length_ = 0;
}

void UnpackBufrValues::dump(eccodes::Dumper* dumper)
{
}

int UnpackBufrValues::unpack_string_array(char** buffer, size_t* len)
{
    grib_accessor* data = (grib_accessor*)data_accessor_;
    return data->unpack_double(0, 0);
}

int UnpackBufrValues::unpack_string(char* buffer, size_t* len)
{
    grib_accessor* data = (grib_accessor*)data_accessor_;
    strcpy(buffer, "0");
    *len = 1;

    return data->unpack_double(0, 0);
}

int UnpackBufrValues::unpack_long(long* val, size_t* len)
{
    grib_accessor* data = (grib_accessor*)data_accessor_;
    return data->unpack_double(0, 0);
}

int UnpackBufrValues::unpack_double(double* val, size_t* len)
{
    grib_accessor* data = (grib_accessor*)data_accessor_;

    return data->unpack_double(0, 0);
}

int UnpackBufrValues::value_count(long* count)
{
    *count = 1;
    return 0;
}

void UnpackBufrValues::destroy(grib_context* context)
{
    Gen::destroy(context);
}

long UnpackBufrValues::get_native_type()
{
    return GRIB_TYPE_LONG;
}

// static const char* mode_to_str(int p)
// {
//     if (p==CODES_BUFR_UNPACK_STRUCTURE) return "CODES_BUFR_UNPACK_STRUCTURE";
//     if (p==CODES_BUFR_UNPACK_FLAT)      return "CODES_BUFR_UNPACK_FLAT";
//     if (p==CODES_BUFR_NEW_DATA)         return "CODES_BUFR_NEW_DATA";
//     return "unknown proc flag";
// }

int UnpackBufrValues::pack_long(const long* val, size_t* len)
{
    int unpackMode = CODES_BUFR_UNPACK_STRUCTURE;

    if (*val == 2)
        unpackMode = CODES_BUFR_UNPACK_FLAT;
    if (*val == 3)
        unpackMode = CODES_BUFR_NEW_DATA;

    data_accessor_->accessor_bufr_data_array_set_unpackMode(unpackMode);

    return data_accessor_->unpack_double(0, 0);
}

int UnpackBufrValues::pack_double(const double* val, size_t* len)
{
    return data_accessor_->unpack_double(0, 0);
}

}  // namespace eccodes::accessor
