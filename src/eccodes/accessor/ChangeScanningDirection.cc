/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "ChangeScanningDirection.h"

eccodes::accessor::ChangeScanningDirection _grib_accessor_change_scanning_direction;
eccodes::Accessor* grib_accessor_change_scanning_direction = &_grib_accessor_change_scanning_direction;

namespace eccodes::accessor
{

void ChangeScanningDirection::init(const long len, grib_arguments* args)
{
    Gen::init(len, args);
    grib_handle* h = get_enclosing_handle();
    int n          = 0;

    values_             = args->get_name(h, n++);
    Ni_                 = args->get_name(h, n++);
    Nj_                 = args->get_name(h, n++);
    i_scans_negatively_ = args->get_name(h, n++);
    j_scans_positively_ = args->get_name(h, n++);
    first_              = args->get_name(h, n++);
    last_               = args->get_name(h, n++);
    axis_               = args->get_name(h, n++);

    flags_ |= GRIB_ACCESSOR_FLAG_FUNCTION;
    length_ = 0;
}

int ChangeScanningDirection::pack_long(const long* val, size_t* len)
{
    int err = 0;
    long i, j, jr, theEnd, Ni, Nj, k, kp;
    double tmp;
    long iScansNegatively   = 0;
    long jScansPositively   = 0;
    double first            = 0;
    double last             = 0;
    size_t size             = 0;
    double* values          = NULL;
    const grib_context* c   = context_;
    grib_handle* h          = get_enclosing_handle();

    if (*val == 0)
        return GRIB_SUCCESS;

    /* Make sure Ni / Nj are not missing */
    if (grib_is_missing(h, Ni_, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing'!", class_name_, Ni_);
        return GRIB_WRONG_GRID;
    }
    if (grib_is_missing(h, Nj_, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing'!", class_name_, Nj_);
        return GRIB_WRONG_GRID;
    }

    if ((err = grib_get_long_internal(h, Ni_, &Ni)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, Nj_, &Nj)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(h, i_scans_negatively_, &iScansNegatively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, j_scans_positively_, &jScansPositively)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(h, first_, &first)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, last_, &last)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_size(h, values_, &size)) != GRIB_SUCCESS)
        return err;

    if (size > (size_t)(Ni * Nj)) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Wrong values size!=Ni*Nj (%zu!=%ld*%ld)", class_name_, size, Ni, Nj);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, values_, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    ECCODES_ASSERT(axis_);
    ECCODES_ASSERT(strcmp(axis_, "x") == 0 || strcmp(axis_, "y") == 0);

    if (axis_[0] == 'x') {
        theEnd = Ni / 2;
        for (j = 0; j < Nj; j++) {
            jr = Ni * j;
            for (i = 0; i < theEnd; i++) {
                k          = jr + i;
                kp         = jr + Ni - i - 1;
                tmp        = values[k];
                values[k]  = values[kp];
                values[kp] = tmp;
            }
        }
        iScansNegatively = !iScansNegatively;
        if ((err = grib_set_long_internal(h, i_scans_negatively_, iScansNegatively)) != GRIB_SUCCESS)
            return err;
    }
    else {
        long kpj;
        theEnd = Nj / 2;
        for (i = 0; i < Ni; i++) {
            kpj = Ni * (Nj - 1);
            for (j = 0; j < theEnd; j++) {
                k          = Ni * j + i;
                kp         = kpj - Ni * j + i;
                tmp        = values[k];
                values[k]  = values[kp];
                values[kp] = tmp;
            }
        }
        jScansPositively = !jScansPositively;
        if ((err = grib_set_long_internal(h, j_scans_positively_, jScansPositively)) != GRIB_SUCCESS)
            return err;
    }

    if ((err = grib_set_double_array_internal(h, values_, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_double_internal(h, first_, last)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_double_internal(h, last_, first)) != GRIB_SUCCESS)
        return err;

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}

long ChangeScanningDirection::get_native_type()
{
    return GRIB_TYPE_LONG;
}

int ChangeScanningDirection::unpack_long(long* v, size_t* len)
{
    /* ECC-976: decoding this accessor doesn't make sense so we return a dummy value */
    *v = -1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
