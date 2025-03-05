/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Vector.h"
#include "AbstractVector.h"

eccodes::accessor::Vector _grib_accessor_vector;
eccodes::Accessor* grib_accessor_vector = &_grib_accessor_vector;

namespace eccodes::accessor
{

void Vector::init(const long l, grib_arguments* c)
{
    AbstractVector::init(l, c);
    int n = 0;

    vector_ = c->get_name(grib_handle_of_accessor(this), n++);
    index_  = c->get_long(grib_handle_of_accessor(this), n++);
    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int Vector::unpack_double(double* val, size_t* len)
{
    int err     = 0;
    size_t size = 0;
    double* stat;
    grib_accessor* va                  = (grib_accessor*)grib_find_accessor(grib_handle_of_accessor(this), vector_);
    accessor::AbstractVector* v = (accessor::AbstractVector*)va;

    ECCODES_ASSERT(index_ >= 0);

    if (index_ >= v->number_of_elements_) {
        grib_context_log(context_, GRIB_LOG_FATAL, "index=%d number_of_elements=%d for %s", index_, v->number_of_elements_, name_);
        ECCODES_ASSERT(index_ < v->number_of_elements_);
    }

    if (va->dirty_) {
        // printf("\nClassVector::unpack_double  accessor=%s is DIRTY\n", name_);
        grib_get_size(grib_handle_of_accessor(this), vector_, &size);
        stat = (double*)grib_context_malloc_clear(context_, sizeof(double) * size);
        err  = va->unpack_double(stat, &size);
        grib_context_free(context_, stat);
        if (err)
            return err;
    }
    else {
        // printf("\nClassVector::unpack_double  accessor=%s is CLEAN\n",a->name);
    }

    *val = v->v_[index_];

    return err;
}

}  // namespace eccodes::accessor
