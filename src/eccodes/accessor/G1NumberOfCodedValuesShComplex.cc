/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G1NumberOfCodedValuesShComplex.h"

eccodes::accessor::G1NumberOfCodedValuesShComplex _grib_accessor_g1number_of_coded_values_sh_complex;
eccodes::Accessor* grib_accessor_g1number_of_coded_values_sh_complex = &_grib_accessor_g1number_of_coded_values_sh_complex;

namespace eccodes::accessor
{

void G1NumberOfCodedValuesShComplex::init(const long l, grib_arguments* c)
{
    Long::init(l, c);
    int n             = 0;
    bitsPerValue_     = c->get_name(get_enclosing_handle(), n++);
    offsetBeforeData_ = c->get_name(get_enclosing_handle(), n++);
    offsetAfterData_  = c->get_name(get_enclosing_handle(), n++);
    unusedBits_       = c->get_name(get_enclosing_handle(), n++);
    numberOfValues_   = c->get_name(get_enclosing_handle(), n++);
    JS_               = c->get_name(get_enclosing_handle(), n++);
    KS_               = c->get_name(get_enclosing_handle(), n++);
    MS_               = c->get_name(get_enclosing_handle(), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int G1NumberOfCodedValuesShComplex::unpack_long(long* val, size_t* len)
{
    int ret               = GRIB_SUCCESS;
    long bpv              = 0;
    long offsetBeforeData = 0, offsetAfterData = 0, unusedBits = 0, numberOfValues;
    long JS = 0, KS = 0, MS = 0, NS = 0;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), bitsPerValue_, &bpv)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), offsetBeforeData_, &offsetBeforeData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), offsetAfterData_, &offsetAfterData)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), unusedBits_, &unusedBits)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), JS_, &JS)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), KS_, &KS)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(get_enclosing_handle(), MS_, &MS)) != GRIB_SUCCESS)
        return ret;

    if (JS != KS || KS != MS)
        return GRIB_NOT_IMPLEMENTED;

    NS = (MS + 1) * (MS + 2);

    if (bpv != 0) {
        DEBUG_ASSERT(offsetAfterData > offsetBeforeData);
        *val = ((offsetAfterData - offsetBeforeData) * 8 - unusedBits + NS * (bpv - 32)) / bpv;
    }
    else {
        if ((ret = grib_get_long_internal(get_enclosing_handle(), numberOfValues_, &numberOfValues)) != GRIB_SUCCESS)
            return ret;

        *val = numberOfValues;
    }

    return ret;
}

}  // namespace eccodes::accessor
