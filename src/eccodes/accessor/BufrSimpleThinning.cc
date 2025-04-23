/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "BufrSimpleThinning.h"

eccodes::accessor::BufrSimpleThinning _grib_accessor_bufr_simple_thinning;
eccodes::Accessor* grib_accessor_bufr_simple_thinning = &_grib_accessor_bufr_simple_thinning;

namespace eccodes::accessor
{

void BufrSimpleThinning::init(const long len, grib_arguments* arg)
{
    Gen::init(len, arg);
    grib_handle* h = get_enclosing_handle();
    int n          = 0;

    length_                      = 0;
    doExtractSubsets_            = arg->get_name(h, n++);
    numberOfSubsets_             = arg->get_name(h, n++);
    extractSubsetList_           = arg->get_name(h, n++);
    simpleThinningStart_         = arg->get_name(h, n++);
    simpleThinningMissingRadius_ = arg->get_name(h, n++);
    simpleThinningSkip_          = arg->get_name(h, n++);

    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

long BufrSimpleThinning::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int BufrSimpleThinning::apply_thinning()
{
    long skip;
    grib_handle* h  = get_enclosing_handle();
    grib_context* c = h->context;
    long compressed = 0, nsubsets;
    grib_iarray* subsets;
    long* subsets_ar = 0;
    long start = 0, radius = 0;

    int ret = grib_get_long(h, "compressedData", &compressed);
    if (ret)
        return ret;
    if (compressed) {
        long numberOfSubsets = 0;
        ret                  = grib_get_long(h, numberOfSubsets_, &numberOfSubsets);
        if (ret)
            return ret;

        ret = grib_get_long(h, simpleThinningStart_, &start);
        if (ret)
            return ret;

        ret = grib_get_long(h, simpleThinningSkip_, &skip);
        if (ret)
            return ret;
        if (skip <= 0)
            return GRIB_INVALID_KEY_VALUE;

        ret = grib_get_long(h, simpleThinningMissingRadius_, &radius);
        if (ret)
            return ret;

        subsets = grib_iarray_new(numberOfSubsets / skip + 1, 10);
        for (long i = 0; i < numberOfSubsets; i += skip + 1) {
            grib_iarray_push(subsets, i + 1);
        }

        nsubsets = grib_iarray_used_size(subsets);

        if (nsubsets != 0) {
            subsets_ar = grib_iarray_get_array(subsets);
            ret        = grib_set_long_array(h, extractSubsetList_, subsets_ar, nsubsets);
            grib_context_free(c, subsets_ar);
            if (ret)
                return ret;

            ret = grib_set_long(h, "unpack", 1);
            if (ret)
                return ret;

            ret = grib_set_long(h, doExtractSubsets_, 1);
            if (ret)
                return ret;
        }
        grib_iarray_delete(subsets);
    }
    else {
        return GRIB_NOT_IMPLEMENTED;
    }

    return ret;
}

int BufrSimpleThinning::pack_long(const long* val, size_t* len)
{
    if (*len == 0)
        return GRIB_SUCCESS;
    int err = apply_thinning();
    if (err)
        return err;

    return grib_set_long(parent_->h, doExtractSubsets_, 1);
}

}  // namespace eccodes::accessor
