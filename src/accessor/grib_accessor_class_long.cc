/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_long.h"

grib_accessor_class_long_t _grib_accessor_class_long{"long"};
grib_accessor_class* grib_accessor_class_long = &_grib_accessor_class_long;

void grib_accessor_class_long_t::init(grib_accessor* a, const long len, grib_arguments* arg) {
    grib_accessor_class_gen_t::init(a, len, arg);
}

int grib_accessor_class_long_t::get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_LONG;
}

void grib_accessor_class_long_t::dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

int grib_accessor_class_long_t::unpack_string(grib_accessor* a, char* v, size_t* len)
{
    long val = 0;
    size_t l = 1;
    char repres[1024];
    char format[32] = "%ld";
    grib_handle* h = grib_handle_of_accessor(a);

    a->unpack_long(&val, &l);
    /* TODO: We should catch all errors but in this case the test ERA_Gen.sh will fail
     * as the output from grib_ls will be different */

    if ((val == GRIB_MISSING_LONG) && ((a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING) != 0)) {
        snprintf(repres, sizeof(repres), "MISSING");
    } else {
        size_t size = sizeof(format);
        grib_get_string(h, "formatForLongs", format, &size);
        snprintf(repres, sizeof(repres), format, val);
    }

    l = strlen(repres) + 1;

    if (*len < l) {
        const char* cclass_name = a->cclass->name;
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         cclass_name, a->name, l, *len);
        *len = l;
        return GRIB_BUFFER_TOO_SMALL;
    }
    /*grib_context_log(a->context,GRIB_LOG_DEBUG, "grib_accessor_long: Casting long %s to string ", a->name);*/

    *len = l;

    strcpy(v, repres);
    return GRIB_SUCCESS;
}

int grib_accessor_class_long_t::pack_missing(grib_accessor* a)
{
    size_t len = 1;
    long value = GRIB_MISSING_LONG;

    if (a->flags & GRIB_ACCESSOR_FLAG_CAN_BE_MISSING)
        return a->pack_long(&value, &len);

    return GRIB_VALUE_CANNOT_BE_MISSING;
}

int grib_accessor_class_long_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    size_t rlen     = 0;
    long count      = 0;
    unsigned long i = 0;
    long* values    = NULL;
    long oneval     = 0;
    int ret         = GRIB_SUCCESS;

    ret = a->value_count(&count);
    if (ret)
        return ret;
    rlen = count;

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s, it contains %lu values", a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (rlen == 1) {
        ret = a->unpack_long(&oneval, &rlen);
        if (ret != GRIB_SUCCESS)
            return ret;
        *val = oneval;
        *len = 1;
        return GRIB_SUCCESS;
    }

    values = (long*)grib_context_malloc(a->context, rlen * sizeof(long));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    ret = a->unpack_long(values, &rlen);
    if (ret != GRIB_SUCCESS) {
        grib_context_free(a->context, values);
        return ret;
    }
    for (i = 0; i < rlen; i++)
        val[i] = values[i];

    grib_context_free(a->context, values);

    *len = rlen;
    return GRIB_SUCCESS;
}

int grib_accessor_class_long_t::compare(grib_accessor* a, grib_accessor* b)
{
    int retval = 0;
    long* aval = 0;
    long* bval = 0;
    long count = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;

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

    aval = (long*)grib_context_malloc(a->context, alen * sizeof(long));
    bval = (long*)grib_context_malloc(b->context, blen * sizeof(long));

    a->unpack_long(aval, &alen);
    b->unpack_long(bval, &blen);

    retval = GRIB_SUCCESS;
    for (size_t i=0; i<alen && retval == GRIB_SUCCESS; ++i) {
        if (aval[i] != bval[i]) retval = GRIB_LONG_VALUE_MISMATCH;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}

int grib_accessor_class_long_t::pack_string(grib_accessor* a, const char* val, size_t* len)
{
    long v = 0; /* The converted value */

    // ECC-1722
    if (STR_EQUAL_NOCASE(val, "missing")) {
        return pack_missing(a);
    }

    if (string_to_long(val, &v, 1) != GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                "Trying to pack \"%s\" as long. String cannot be converted to an integer", val);
        return GRIB_WRONG_TYPE;
    }
    return a->pack_long(&v, len);
}
