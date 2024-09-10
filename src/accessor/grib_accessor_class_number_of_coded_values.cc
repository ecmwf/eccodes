/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_number_of_coded_values.h"

AccessorBuilder<grib_accessor_number_of_coded_values_t> _grib_accessor_number_of_coded_values_builder{};

void grib_accessor_number_of_coded_values_t::init(const long l, grib_arguments* c)
{
    grib_accessor_long_t::init(l, c);
    int n             = 0;
    bitsPerValue_     = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    offsetBeforeData_ = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    offsetAfterData_  = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    unusedBits_       = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    numberOfValues_   = grib_arguments_get_name(grib_handle_of_accessor(this), c, n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int grib_accessor_number_of_coded_values_t::unpack_long(long* val, size_t* len)
{
    int ret               = GRIB_SUCCESS;
    long bpv              = 0;
    long offsetBeforeData = 0, offsetAfterData = 0, unusedBits = 0, numberOfValues;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), bitsPerValue_, &bpv)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), offsetBeforeData_, &offsetBeforeData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), offsetAfterData_, &offsetAfterData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), unusedBits_, &unusedBits)) != GRIB_SUCCESS)
        return ret;

    if (bpv != 0) {
        grib_context_log(context_, GRIB_LOG_DEBUG, "grib_accessor_number_of_coded_values_t: offsetAfterData=%ld offsetBeforeData=%ld unusedBits=%ld bpv=%ld\n",
                         offsetAfterData, offsetBeforeData, unusedBits, bpv);
        DEBUG_ASSERT(offsetAfterData > offsetBeforeData);
        *val = ((offsetAfterData - offsetBeforeData) * 8 - unusedBits) / bpv;
    }
    else {
        if ((ret = grib_get_long_internal(grib_handle_of_accessor(this), numberOfValues_, &numberOfValues)) != GRIB_SUCCESS)
            return ret;

        *val = numberOfValues;
    }

    return ret;
}
