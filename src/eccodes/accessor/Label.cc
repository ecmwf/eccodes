/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Label.h"

eccodes::AccessorBuilder<eccodes::accessor::Label> _grib_accessor_label_builder{};

namespace eccodes::accessor
{

void Label::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
    length_ = 0;
}

void Label::dump(eccodes::Dumper* dumper)
{
    dumper->dump_label(this, NULL);
}

long Label::get_native_type()
{
    return GRIB_TYPE_LABEL;
}

int Label::unpack_string(char* val, size_t* len)
{
    size_t vlen = strlen(name_);
    if (vlen > *len)
        return GRIB_BUFFER_TOO_SMALL;
    *len = vlen;
    strcpy(val, name_);
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
