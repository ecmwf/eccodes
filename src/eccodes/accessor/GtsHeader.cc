/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "GtsHeader.h"

eccodes::accessor::GtsHeader _grib_accessor_gts_header;
eccodes::Accessor* grib_accessor_gts_header = &_grib_accessor_gts_header;

namespace eccodes::accessor
{

void GtsHeader::init(const long l, grib_arguments* c)
{
    Ascii::init(l, c);
    gts_offset_ = c ? c->get_long(get_enclosing_handle(), 0) : 0;
    gts_length_ = c ? c->get_long(get_enclosing_handle(), 1) : 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

int GtsHeader::unpack_string(char* val, size_t* len)
{
    grib_handle* h = get_enclosing_handle();
    int offset     = 0;
    size_t length  = 0;

    if (h->gts_header == NULL || h->gts_header_len < 8) {
        if (*len < 8)
            return GRIB_BUFFER_TOO_SMALL;
        snprintf(val, 1024, "missing");
        return GRIB_SUCCESS;
    }
    if (*len < h->gts_header_len)
        return GRIB_BUFFER_TOO_SMALL;

    offset = gts_offset_ > 0 ? gts_offset_ : 0;
    length = gts_length_ > 0 ? gts_length_ : h->gts_header_len;

    memcpy(val, h->gts_header + offset, length);

    *len = length;

    return GRIB_SUCCESS;
}

int GtsHeader::value_count(long* count)
{
    *count = 1;
    return 0;
}

size_t GtsHeader::string_length()
{
    const grib_handle* h = get_enclosing_handle();
    return h->gts_header_len;
}

}  // namespace eccodes::accessor
