/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "BufrExtractSubsets.h"

eccodes::accessor::BufrExtractSubsets _grib_accessor_bufr_extract_subsets;
eccodes::Accessor* grib_accessor_bufr_extract_subsets = &_grib_accessor_bufr_extract_subsets;

namespace eccodes::accessor
{

void BufrExtractSubsets::get_accessors()
{
    const grib_handle* h                       = grib_handle_of_accessor(this);

    if (packAccessor_)
        return;
    numericValuesAccessor_ = grib_find_accessor(h, numericValues_);
    packAccessor_          = grib_find_accessor(h, pack_);
}

void BufrExtractSubsets::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    int n = 0;

    length_        = 0;
    numericValues_ = arg->get_name(grib_handle_of_accessor(this), n++);
    pack_          = arg->get_name(grib_handle_of_accessor(this), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;

    numericValuesAccessor_ = NULL;
    packAccessor_          = NULL;
}

long BufrExtractSubsets::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int BufrExtractSubsets::pack_long(const long* val, size_t* len)
{
    size_t l = 1;
    long v[1];

    get_accessors();

    v[0]    = 1;
    int err = packAccessor_->pack_long(v, &l);
    if (err) {
        if (err == GRIB_ENCODING_ERROR)
            grib_context_log(context_, GRIB_LOG_ERROR, "Could not extract subset(s).\n\tHint: Did you forget to set unpack=1?");
        return err;
    }

    return err;
}

}  // namespace eccodes::accessor
