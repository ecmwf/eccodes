/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "OffsetValues.h"

eccodes::accessor::OffsetValues _grib_accessor_offset_values;
eccodes::Accessor* grib_accessor_offset_values = &_grib_accessor_offset_values;

namespace eccodes::accessor
{

void OffsetValues::init(const long l, grib_arguments* args)
{
    Double::init(l, args);
    int n         = 0;
    values_       = args->get_name(get_enclosing_handle(), n++);
    missingValue_ = args->get_name(get_enclosing_handle(), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int OffsetValues::unpack_double(double* val, size_t* len)
{
    *val    = 0;
    *len    = 1;
    // It does not make sense to decode this key!
    fprintf(stderr, "ECCODES WARNING :  Key %s is applicable only during encoding.\n", name_);
    return GRIB_SUCCESS;
}

int OffsetValues::pack_double(const double* val, size_t* len)
{
    double* values            = NULL;
    size_t size               = 0;
    double missingValue       = 0;
    long missingValuesPresent = 0;
    int ret = 0;
    grib_context* c = context_;
    grib_handle* h  = get_enclosing_handle();

    if (*val == 0)
        return GRIB_SUCCESS;

    if ((ret = grib_get_double_internal(h, missingValue_, &missingValue)) != GRIB_SUCCESS) {
        return ret;
    }
    if ((ret = grib_get_long_internal(h, "missingValuesPresent", &missingValuesPresent)) != GRIB_SUCCESS) {
        return ret;
    }

    if ((ret = grib_get_size(h, values_, &size)) != GRIB_SUCCESS)
        return ret;

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array_internal(h, values_, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }

    for (size_t i = 0; i < size; i++) {
        if (missingValuesPresent) {
            if (values[i] != missingValue)
                values[i] += *val;
        }
        else {
            values[i] += *val;
        }
    }
    // ECC-2083: GRIB: Applying the scaleValuesBy operation can produce constant fields
    // Assigning 0 to both decimalScaleFactor and binaryScaleFactor automatically triggers a
    // recalculation of binaryScaleFactor.

    grib_set_long(h, "decimalScaleFactor", 0);
    grib_set_long(h, "binaryScaleFactor", 0);

    if ((ret = grib_set_double_array_internal(h, values_, values, size)) != GRIB_SUCCESS)
        return ret;

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
