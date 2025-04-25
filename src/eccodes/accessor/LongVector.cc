/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "LongVector.h"
#include "AbstractLongVector.h"

eccodes::accessor::LongVector _grib_accessor_long_vector;
eccodes::Accessor* grib_accessor_long_vector = &_grib_accessor_long_vector;

namespace eccodes::accessor
{

void LongVector::init(const long l, grib_arguments* c)
{
    AbstractLongVector::init(l, c);
    grib_accessor* va = NULL;
    AbstractLongVector* v = NULL;
    int n = 0;

    vector_ = c->get_name(get_enclosing_handle(), n++);
    va      = (grib_accessor*)grib_find_accessor(get_enclosing_handle(), vector_);
    v       = (AbstractLongVector*)va;

    index_ = c->get_long(get_enclosing_handle(), n++);

    /* check index_ on init and never change it */
    ECCODES_ASSERT(index_ < v->number_of_elements_ && index_ >= 0);

    length_ = 0;
}

int LongVector::unpack_long(long* val, size_t* len)
{
    size_t size = 0;
    int err     = 0;

    grib_accessor* va = (grib_accessor*)grib_find_accessor(get_enclosing_handle(), vector_);
    AbstractLongVector*  v  = (AbstractLongVector*)va;

    /*TODO implement the dirty mechanism to avoid to unpack every time */
    err = grib_get_size(get_enclosing_handle(), vector_, &size);
    if (err) return err;
    DEBUG_ASSERT(size > 0);
    long* vector = (long*)grib_context_malloc(context_, sizeof(long) * size);
    err    = va->unpack_long(vector, &size);
    grib_context_free(context_, vector);
    if (err) return err;

    *val = v->v_[index_];

    return GRIB_SUCCESS;
}

int LongVector::unpack_double(double* val, size_t* len)
{
    long lval = 0;
    int err = 0;
    grib_accessor* va = NULL;
    AbstractLongVector* v = NULL;
    va = (grib_accessor*)grib_find_accessor(get_enclosing_handle(), vector_);
    v  = (AbstractLongVector*)va;

    err = unpack_long(&lval, len);

    *val = (double)v->v_[index_];

    return err;
}

int LongVector::pack_long(const long* val, size_t* len)
{
    int err = 0;
    grib_accessor* va = NULL;
    AbstractLongVector* v = NULL;

    va = (grib_accessor*)grib_find_accessor(get_enclosing_handle(), vector_);
    v  = (AbstractLongVector*)va;

    v->pack_index_ = index_;

    err = va->pack_long(val, len);
    return err;
}

long LongVector::get_native_type()
{
    return GRIB_TYPE_LONG;
}

}  // namespace eccodes::accessor
