/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "BufrStringValues.h"
#include "BufrDataArray.h"

eccodes::AccessorBuilder<eccodes::accessor::BufrStringValues> _grib_accessor_bufr_string_values_builder{};

namespace eccodes::accessor
{

void BufrStringValues::init(const long len, grib_arguments* args)
{
    Ascii::init(len, args);
    int n             = 0;
    dataAccessorName_ = args->get_name(get_enclosing_handle(), n++);
    dataAccessor_     = NULL;
    length_           = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

void BufrStringValues::dump(Dumper* dumper)
{
    dumper->dump_string_array(this, NULL);
}

grib_accessor* BufrStringValues::get_accessor()
{
    if (!dataAccessor_) {
        dataAccessor_ = grib_find_accessor(get_enclosing_handle(), dataAccessorName_);
    }
    return dataAccessor_;
}

int BufrStringValues::unpack_string_array(char** buffer, size_t* len)
{
    grib_context* c = context_;
    grib_vsarray* stringValues = NULL;
    size_t l = 0, tl;
    size_t i, j, n = 0;
    char** b = buffer;

  accessor::BufrDataArray* data = dynamic_cast<accessor::BufrDataArray*>(get_accessor());
    if (!data)
        return GRIB_NOT_FOUND;

    stringValues = data->accessor_bufr_data_array_get_stringValues();

    n = grib_vsarray_used_size(stringValues);

    tl = 0;
    for (j = 0; j < n; j++) {
        l = grib_sarray_used_size(stringValues->v[j]);
        tl += l;

        if (tl > *len)
            return GRIB_ARRAY_TOO_SMALL;

        for (i = 0; i < l; i++) {
            *(b++) = grib_context_strdup(c, stringValues->v[j]->v[i]);
        }
    }
    *len = tl;

    return GRIB_SUCCESS;
}

int BufrStringValues::unpack_string(char* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int BufrStringValues::value_count(long* rlen)
{
    grib_accessor* descriptors = get_accessor();
    return descriptors->value_count(rlen);
}

void BufrStringValues::destroy(grib_context* c)
{
    Ascii::destroy(c);
}

}  // namespace eccodes::accessor
