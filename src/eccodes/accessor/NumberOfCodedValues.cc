/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "NumberOfCodedValues.h"

eccodes::accessor::NumberOfCodedValues _grib_accessor_number_of_coded_values;
eccodes::Accessor* grib_accessor_number_of_coded_values = &_grib_accessor_number_of_coded_values;

namespace eccodes::accessor
{

void NumberOfCodedValues::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    grib_handle* h  = get_enclosing_handle();

    int n             = 0;
    bitsPerValue_     = c->get_name(h, n++);
    offsetBeforeData_ = c->get_name(h, n++);
    offsetAfterData_  = c->get_name(h, n++);
    unusedBits_       = c->get_name(h, n++);
    numberOfValues_   = c->get_name(h, n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int NumberOfCodedValues::unpack_long(long* val, size_t* len)
{
    int ret  = GRIB_SUCCESS;
    long bpv = 0, offsetBeforeData = 0, offsetAfterData = 0, unusedBits = 0, numberOfValues;
    grib_handle* h  = get_enclosing_handle();

    if ((ret = grib_get_long_internal(h, bitsPerValue_, &bpv)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, offsetBeforeData_, &offsetBeforeData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, offsetAfterData_, &offsetAfterData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, unusedBits_, &unusedBits)) != GRIB_SUCCESS)
        return ret;

    if (bpv != 0) {
        grib_context_log(context_, GRIB_LOG_DEBUG,
                        "NumberOfCodedValues: offsetAfterData=%ld offsetBeforeData=%ld unusedBits=%ld bpv=%ld",
                        offsetAfterData, offsetBeforeData, unusedBits, bpv);
        DEBUG_ASSERT(offsetAfterData > offsetBeforeData);
        *val = ((offsetAfterData - offsetBeforeData) * 8 - unusedBits) / bpv;
    }
    else {
        if ((ret = grib_get_long_internal(h, numberOfValues_, &numberOfValues)) != GRIB_SUCCESS)
            return ret;

        *val = numberOfValues;
    }

    return ret;
}

}  // namespace eccodes::accessor
