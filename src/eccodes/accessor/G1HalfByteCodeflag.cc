/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1HalfByteCodeflag.h"

eccodes::AccessorBuilder<eccodes::accessor::G1HalfByteCodeflag> _grib_accessor_g1_half_byte_codeflag_builder{};

namespace eccodes::accessor
{

void G1HalfByteCodeflag::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

void G1HalfByteCodeflag::dump(eccodes::Dumper* dumper)
{
    dumper->dump_long(this, NULL);
}

int G1HalfByteCodeflag::unpack_long(long* val, size_t* len)
{
    unsigned char dat = 0;
    if (*len < 1) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", name_, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    dat = get_enclosing_handle()->buffer->data[offset_] & 0x0f;

    *val = dat;
    *len = 1;
    return GRIB_SUCCESS;
}

int G1HalfByteCodeflag::pack_long(const long* val, size_t* len)
{
    int ret = 0;
    if (*len < 1) {
        grib_context_log(context_, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", name_, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    /*  printf("HALF BYTE pack long %ld %02x\n",*val,get_enclosing_handle()->buffer->data[offset_ ]);*/
    get_enclosing_handle()->buffer->data[offset_] = (parent_->h->buffer->data[offset_] & 0xf0) | (*val & 0x0f);
    /*  printf("HALF BYTE pack long %ld %02x\n",*val,get_enclosing_handle()->buffer->data[offset_ ]);*/

    *len = 1;
    return ret;
}

long G1HalfByteCodeflag::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
