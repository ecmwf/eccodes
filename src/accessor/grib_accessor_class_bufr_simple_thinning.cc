
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_bufr_simple_thinning.h"

grib_accessor_class_bufr_simple_thinning_t _grib_accessor_class_bufr_simple_thinning{ "bufr_simple_thinning" };
grib_accessor_class* grib_accessor_class_bufr_simple_thinning = &_grib_accessor_class_bufr_simple_thinning;


void grib_accessor_class_bufr_simple_thinning_t::init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_class_gen_t::init(a, len, arg);
    grib_accessor_bufr_simple_thinning_t* self = (grib_accessor_bufr_simple_thinning_t*)a;
    grib_handle* h                             = grib_handle_of_accessor(a);
    int n                                      = 0;

    a->length                         = 0;
    self->doExtractSubsets            = grib_arguments_get_name(h, arg, n++);
    self->numberOfSubsets             = grib_arguments_get_name(h, arg, n++);
    self->extractSubsetList           = grib_arguments_get_name(h, arg, n++);
    self->simpleThinningStart         = grib_arguments_get_name(h, arg, n++);
    self->simpleThinningMissingRadius = grib_arguments_get_name(h, arg, n++);
    self->simpleThinningSkip          = grib_arguments_get_name(h, arg, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

int grib_accessor_class_bufr_simple_thinning_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

static int apply_thinning(grib_accessor* a)
{
    const grib_accessor_bufr_simple_thinning_t* self = (grib_accessor_bufr_simple_thinning_t*)a;

    long skip;
    grib_handle* h  = grib_handle_of_accessor(a);
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
        ret                  = grib_get_long(h, self->numberOfSubsets, &numberOfSubsets);
        if (ret)
            return ret;

        ret = grib_get_long(h, self->simpleThinningStart, &start);
        if (ret)
            return ret;

        ret = grib_get_long(h, self->simpleThinningSkip, &skip);
        if (ret)
            return ret;
        if (skip <= 0)
            return GRIB_INVALID_KEY_VALUE;

        ret = grib_get_long(h, self->simpleThinningMissingRadius, &radius);
        if (ret)
            return ret;

        subsets = grib_iarray_new(c, numberOfSubsets / skip + 1, 10);
        for (long i = 0; i < numberOfSubsets; i += skip + 1) {
            grib_iarray_push(subsets, i + 1);
        }

        nsubsets = grib_iarray_used_size(subsets);

        if (nsubsets != 0) {
            subsets_ar = grib_iarray_get_array(subsets);
            ret        = grib_set_long_array(h, self->extractSubsetList, subsets_ar, nsubsets);
            grib_context_free(c, subsets_ar);
            if (ret)
                return ret;

            ret = grib_set_long(h, "unpack", 1);
            if (ret)
                return ret;

            ret = grib_set_long(h, self->doExtractSubsets, 1);
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

int grib_accessor_class_bufr_simple_thinning_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    const grib_accessor_bufr_simple_thinning_t* self = (grib_accessor_bufr_simple_thinning_t*)a;

    if (*len == 0)
        return GRIB_SUCCESS;
    int err = apply_thinning(a);
    if (err)
        return err;

    return grib_set_long(a->parent->h, self->doExtractSubsets, 1);
}
