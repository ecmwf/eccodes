/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_variable.h"
#include <limits.h>


grib_accessor_class_variable_t _grib_accessor_class_variable{"variable"};
grib_accessor_class* grib_accessor_class_variable = &_grib_accessor_class_variable;

//
// This accessor is used for:
//  constant
//  transient
//

#define MAX_VARIABLE_STRING_LENGTH 255

void grib_accessor_class_variable_t::init(grib_accessor* a, const long length, grib_arguments* args)
{
    grib_accessor_class_gen_t::init(a, length, args);

    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    grib_handle* hand            = grib_handle_of_accessor(a);
    grib_expression* expression  = grib_arguments_get_expression(hand, args, 0);
    const char* p                = 0;
    size_t len                   = 1;
    long l = 0;
    int ret = 0;
    double d = 0;
    self->cname = NULL;

    a->length = 0;
    if (self->type == GRIB_TYPE_UNDEFINED && expression) {
        self->type = grib_expression_native_type(hand, expression);

        switch (self->type) {
            case GRIB_TYPE_DOUBLE:
                grib_expression_evaluate_double(hand, expression, &d);
                a->pack_double(&d, &len);
                break;

            case GRIB_TYPE_LONG:
                grib_expression_evaluate_long(hand, expression, &l);
                a->pack_long(&l, &len);
                break;

            default: {
                char tmp[1024];
                len = sizeof(tmp);
                p   = grib_expression_evaluate_string(hand, expression, tmp, &len, &ret);
                if (ret != GRIB_SUCCESS) {
                    grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to evaluate %s as string: %s",
                                     a->name, grib_get_error_message(ret));
                    return;
                }
                len = strlen(p) + 1;
                a->pack_string(p, &len);
                break;
            }
        }
    }
}

void accessor_variable_set_type(grib_accessor* a, int type)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    self->type                   = type;
}

void grib_accessor_class_variable_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    switch (self->type) {
        case GRIB_TYPE_DOUBLE:
            grib_dump_double(dumper, a, NULL);
            break;

        case GRIB_TYPE_LONG:
            grib_dump_long(dumper, a, NULL);
            break;

        default:
            grib_dump_string(dumper, a, NULL);
            break;
    }
}

int grib_accessor_class_variable_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    const double dval = *val;

    if (*len != 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains 1 value", a->name);
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    //if (std::isnan(dval)) {
    //    grib_context_log(a->context, GRIB_LOG_ERROR, "%s: Invalid number for %s: %g", __func__, a->name, dval);
    //    return GRIB_INVALID_ARGUMENT;
    //}

    self->dval = dval;
    if (dval < (double)LONG_MIN || dval > (double)LONG_MAX)
        self->type = GRIB_TYPE_DOUBLE;
    else
        self->type = ((long)dval == dval) ? GRIB_TYPE_LONG : GRIB_TYPE_DOUBLE;

    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::pack_float(grib_accessor* a, const float* val, size_t* len)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    const double fval = *val;

    if (*len != 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains 1 value", a->name);
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    self->fval = fval;
    if (fval < (float)LONG_MIN || fval > (float)LONG_MAX)
        self->type = GRIB_TYPE_DOUBLE;
    else
        self->type = ((long)fval == fval) ? GRIB_TYPE_LONG : GRIB_TYPE_DOUBLE;

    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;

    if (*len != 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains 1 value", a->name);
        *len = 1;
        return GRIB_ARRAY_TOO_SMALL;
    }

    self->dval = *val;
    self->fval = *val;
    self->type = GRIB_TYPE_LONG;

    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %d values", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *val = self->dval;
    *len = 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::unpack_float(grib_accessor* a, float* val, size_t* len)
{
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %d values", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *val = self->fval;
    *len = 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::unpack_long(grib_accessor* a, long* val, size_t* len)
{
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;

    if (*len < 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %d values ", a->name, 1);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }
    *val = (long)self->dval;
    *len = 1;
    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::get_native_type(grib_accessor* a)
{
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    return self->type;
}

void grib_accessor_class_variable_t::destroy(grib_context* c, grib_accessor* a)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    int i                        = 0;

    grib_context_free(c, self->cval);
    if (self->cname)
        grib_context_free(c, self->cname); /* ECC-765 */

    /* Note: BUFR operator descriptors are variables and have attributes so need to free them */
    while (i < MAX_ACCESSOR_ATTRIBUTES && a->attributes[i]) {
        a->attributes[i]->destroy(c);
        a->attributes[i] = NULL;
        ++i;
    }

    grib_accessor_class_gen_t::destroy(c, a);
}

int grib_accessor_class_variable_t::unpack_string(grib_accessor* a, char* val, size_t* len)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;

    char buf[80];
    char* p = buf;
    size_t slen = 0;

    if (self->type == GRIB_TYPE_STRING) {
        p = self->cval;
    }
    else {
        snprintf(p, 64, "%g", self->dval);
    }

    slen = strlen(p) + 1;
    if (*len < slen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         a->cclass->name, a->name, slen, *len);
        *len = slen;
        return GRIB_BUFFER_TOO_SMALL;
    }
    strcpy(val, p);
    *len = slen;

    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::pack_string(grib_accessor* a, const char* val, size_t* len)
{
    grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    const grib_context* c = a->context;

    grib_context_free(c, self->cval);
    self->cval  = grib_context_strdup(c, val);
    self->dval  = atof(val);
    self->fval  = atof(val);
    self->type  = GRIB_TYPE_STRING;
    self->cname = NULL;
    return GRIB_SUCCESS;
}

int grib_accessor_class_variable_t::value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}

size_t grib_accessor_class_variable_t::string_length(grib_accessor* a)
{
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    if (self->type == GRIB_TYPE_STRING)
        return strlen(self->cval);
    else
        return MAX_VARIABLE_STRING_LENGTH;
}

long grib_accessor_class_variable_t::byte_count(grib_accessor* a)
{
    return a->length;
}

int grib_accessor_class_variable_t::compare(grib_accessor* a, grib_accessor* b)
{
    int retval   = GRIB_SUCCESS;
    double* aval = 0;
    double* bval = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = a->value_count(&count);
    if (err)
        return err;
    alen = count;

    err = b->value_count(&count);
    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (double*)grib_context_malloc(a->context, alen * sizeof(double));
    bval = (double*)grib_context_malloc(b->context, blen * sizeof(double));

    a->unpack_double(aval, &alen);
    b->unpack_double(bval, &blen);

    retval = GRIB_SUCCESS;
    retval = GRIB_SUCCESS;
    for (size_t i=0; i<alen && retval == GRIB_SUCCESS; ++i) {
        if (aval[i] != bval[i]) retval = GRIB_DOUBLE_VALUE_MISMATCH;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}

grib_accessor* grib_accessor_class_variable_t::make_clone(grib_accessor* a, grib_section* s, int* err)
{
    grib_accessor* the_clone = NULL;
    const grib_accessor_variable_t* self = (grib_accessor_variable_t*)a;
    grib_accessor_variable_t* variableAccessor = NULL;
    grib_action creator = {0,};
    creator.op = (char*)"variable";
    creator.name_space = (char*)"";
    creator.set = 0;

    creator.name            = grib_context_strdup(a->context, a->name);
    the_clone               = grib_accessor_factory(s, &creator, 0, NULL);
    the_clone->parent       = NULL;
    the_clone->h            = s->h;
    the_clone->flags        = a->flags;
    variableAccessor        = (grib_accessor_variable_t*)the_clone;
    variableAccessor->cname = creator.name; /* ECC-765: Store for later freeing memory */

    *err                   = 0;
    variableAccessor->type = self->type;
    if (self->type == GRIB_TYPE_STRING && self->cval != NULL) {
        variableAccessor->cval = grib_context_strdup(a->context, self->cval);
    }
    else {
        variableAccessor->dval = self->dval;
        variableAccessor->fval = self->fval;
    }

    return the_clone;
}
