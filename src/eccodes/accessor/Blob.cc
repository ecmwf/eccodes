/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Blob.h"

eccodes::AccessorBuilder<eccodes::accessor::Blob> _grib_accessor_blob_builder{};

namespace eccodes::accessor
{

void Blob::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    grib_get_long_internal(grib_handle_of_accessor(this),
                           arg->get_name(parent_->h, 0), &length_);
    ECCODES_ASSERT(length_ >= 0);
}

long Blob::get_native_type()
{
    return GRIB_TYPE_BYTES;
}

int Blob::unpack_bytes(unsigned char* buffer, size_t* len)
{
    if (*len < (size_t)length_) {
        *len = length_;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *len = length_;

    memcpy(buffer, grib_handle_of_accessor(this)->buffer->data + offset_, *len);

    return GRIB_SUCCESS;
}

void Blob::dump(eccodes::Dumper* dumper)
{
    dumper->dump_bytes(this, NULL);
}

}  // namespace eccodes::accessor
