/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Uint32LittleEndian.h"

eccodes::accessor::Uint32LittleEndian _grib_accessor_uint32_little_endian;
eccodes::Accessor* grib_accessor_uint32_little_endian = &_grib_accessor_uint32_little_endian;

namespace eccodes::accessor
{

int Uint32LittleEndian::unpack_long(long* val, size_t* len)
{
    long value = 0;
    /* long pos = offset_ ; */
    /* unsigned char* data = get_enclosing_handle()->buffer->data; */

    if (*len < 1) {
        return GRIB_ARRAY_TOO_SMALL;
    }

    *val = value;
    *len = 1;
    return GRIB_NOT_IMPLEMENTED;
}

long Uint32LittleEndian::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
