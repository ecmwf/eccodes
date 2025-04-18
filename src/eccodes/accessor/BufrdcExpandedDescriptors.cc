/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "BufrdcExpandedDescriptors.h"

eccodes::accessor::BufrdcExpandedDescriptors _grib_accessor_bufrdc_expanded_descriptors;
eccodes::Accessor* grib_accessor_bufrdc_expanded_descriptors = &_grib_accessor_bufrdc_expanded_descriptors;

namespace eccodes::accessor
{

void BufrdcExpandedDescriptors::init(const long len, grib_arguments* args)
{
    Long::init(len, args);
    int n                        = 0;
    expandedDescriptors_         = args->get_name(get_enclosing_handle(), n++);
    expandedDescriptorsAccessor_ = 0;
    length_                      = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

grib_accessor* BufrdcExpandedDescriptors::get_accessor()
{
    if (!expandedDescriptorsAccessor_) {
        expandedDescriptorsAccessor_ = grib_find_accessor(get_enclosing_handle(), expandedDescriptors_);
    }
    return expandedDescriptorsAccessor_;
}

int BufrdcExpandedDescriptors::unpack_long(long* val, size_t* len)
{
    grib_accessor* descriptors = 0;
    size_t rlen                = 0, l;
    long lenall                = 0;
    size_t i                   = 0;
    long* v                    = 0;
    grib_context* c            = context_;

    descriptors = get_accessor();
    if (!descriptors) return GRIB_NOT_FOUND;

    value_count(&lenall);
    v = (long*)grib_context_malloc_clear(c, sizeof(long) * lenall);
    l = lenall;
    descriptors->unpack_long(v, &l);
    rlen = 0;
    for (i = 0; i < l; i++) {
        if ((v[i] < 100000 || v[i] > 221999))
            val[rlen++] = v[i];
    }
    *len = rlen;
    grib_context_free(c, v);

    return GRIB_SUCCESS;
}

int BufrdcExpandedDescriptors::unpack_string_array(char** buffer, size_t* len)
{
    int err                    = 0;
    grib_accessor* descriptors = 0;
    size_t l                   = 0;
    long lenall                = 0;
    size_t i                   = 0;
    long* v                    = 0;
    char buf[25]               = {0,};
    grib_context* c = context_;

    descriptors = get_accessor();
    if (!descriptors) return GRIB_NOT_FOUND;

    err = value_count(&lenall);
    if (err) return err;
    l = lenall;
    if (l > *len) return GRIB_ARRAY_TOO_SMALL;

    v   = (long*)grib_context_malloc_clear(c, sizeof(long) * l);
    err = descriptors->unpack_long(v, &l);
    if (err) return err;

    for (i = 0; i < l; i++) {
        snprintf(buf, sizeof(buf), "%06ld", v[i]);
        buffer[i] = grib_context_strdup(c, buf);
    }
    *len = l;
    grib_context_free(c, v);

    return GRIB_SUCCESS;
}

int BufrdcExpandedDescriptors::value_count(long* rlen)
{
    grib_accessor* descriptors = get_accessor();

    return descriptors->value_count(rlen);
}

void BufrdcExpandedDescriptors::destroy(grib_context* c)
{
    Long::destroy(c);
}

}  // namespace eccodes::accessor
