
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_change_scanning_direction.h"

grib_accessor_class_change_scanning_direction_t _grib_accessor_class_change_scanning_direction{"change_scanning_direction"};
grib_accessor_class* grib_accessor_class_change_scanning_direction = &_grib_accessor_class_change_scanning_direction;


void grib_accessor_class_change_scanning_direction_t::init(grib_accessor* a, const long len, grib_arguments* args){
    grib_accessor_class_gen_t::init(a, len, args);
    grib_accessor_change_scanning_direction_t* self = (grib_accessor_change_scanning_direction_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    int n = 0;

    self->values             = grib_arguments_get_name(h, args, n++);
    self->Ni                 = grib_arguments_get_name(h, args, n++);
    self->Nj                 = grib_arguments_get_name(h, args, n++);
    self->i_scans_negatively = grib_arguments_get_name(h, args, n++);
    self->j_scans_positively = grib_arguments_get_name(h, args, n++);
    self->first              = grib_arguments_get_name(h, args, n++);
    self->last               = grib_arguments_get_name(h, args, n++);
    self->axis               = grib_arguments_get_name(h, args, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
    a->length = 0;
}

int grib_accessor_class_change_scanning_direction_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    int err = 0;
    long i, j, jr, theEnd, Ni, Nj, k, kp;
    double tmp;
    long iScansNegatively                         = 0;
    long jScansPositively                         = 0;
    double first                                  = 0;
    double last                                   = 0;
    size_t size                                   = 0;
    double* values                                = NULL;
    grib_accessor_change_scanning_direction_t* self = (grib_accessor_change_scanning_direction_t*)a;
    const grib_context* c                         = a->context;
    grib_handle* h                                = grib_handle_of_accessor(a);
    const char* cclass_name                       = a->cclass->name;

    if (*val == 0)
        return GRIB_SUCCESS;

    /* Make sure Ni / Nj are not missing */
    if (grib_is_missing(h, self->Ni, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing'!", cclass_name, self->Ni);
        return GRIB_WRONG_GRID;
    }
    if (grib_is_missing(h, self->Nj, &err) && !err) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Key %s cannot be 'missing'!", cclass_name, self->Nj);
        return GRIB_WRONG_GRID;
    }

    if ((err = grib_get_long_internal(h, self->Ni, &Ni)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->Nj, &Nj)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_long_internal(h, self->i_scans_negatively, &iScansNegatively)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, self->j_scans_positively, &jScansPositively)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(h, self->first, &first)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, self->last, &last)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_size(h, self->values, &size)) != GRIB_SUCCESS)
        return err;

    if (size > Ni * Nj) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Wrong values size!=Ni*Nj (%ld!=%ld*%ld)", cclass_name, size, Ni, Nj);
        return GRIB_WRONG_ARRAY_SIZE;
    }

    values = (double*)grib_context_malloc(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(h, self->values, values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    Assert(self->axis);
    Assert(strcmp(self->axis, "x") == 0 || strcmp(self->axis, "y") == 0);

    if (self->axis[0] == 'x') {
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
        if ((err = grib_set_long_internal(h, self->i_scans_negatively, iScansNegatively)) != GRIB_SUCCESS)
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
        if ((err = grib_set_long_internal(h, self->j_scans_positively, jScansPositively)) != GRIB_SUCCESS)
            return err;
    }

    if ((err = grib_set_double_array_internal(h, self->values, values, size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return err;
    }

    if ((err = grib_set_double_internal(h, self->first, last)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_double_internal(h, self->last, first)) != GRIB_SUCCESS)
        return err;

    grib_context_free(c, values);

    return GRIB_SUCCESS;
}

int grib_accessor_class_change_scanning_direction_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}

int grib_accessor_class_change_scanning_direction_t::unpack_long(grib_accessor* a, long* v, size_t* len){
    /* ECC-976: decoding this accessor doesn't make sense so we return a dummy value */
    *v = -1;
    return GRIB_SUCCESS;
}
