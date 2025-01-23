/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "MessageIsValid.h"
#include <cstdio>

grib_accessor_message_is_valid_t _grib_accessor_message_is_valid{};
grib_accessor* grib_accessor_message_is_valid = &_grib_accessor_message_is_valid;

namespace eccodes::accessor
{

void grib_accessor_message_is_valid_t::init(const long l, grib_arguments* arg)
{
    grib_accessor_long_t::init(l, arg);

    grib_handle* h = grib_handle_of_accessor(this);
    product_ = arg->get_name(h, 0);

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

static int check_field_values(grib_handle* h)
{
    //printf("DEBUG  %s \n", __func__);
    int ret = GRIB_SUCCESS;
    double* values = NULL;
    size_t size = 0;
    grib_context* c = h->context;

    if ((ret = grib_get_size(h, "values", &size)) != GRIB_SUCCESS)
        return ret;
    values = (double*)grib_context_malloc_clear(c, size * sizeof(double));
    if (!values)
        return GRIB_OUT_OF_MEMORY;

    if ((ret = grib_get_double_array(h, "values", values, &size)) != GRIB_SUCCESS) {
        grib_context_free(c, values);
        return ret;
    }
    grib_context_free(c, values);
    return GRIB_SUCCESS;
}

static int check_grid_pl_array(grib_handle* h)
{
    // printf("DEBUG  %s \n", __func__);
    int ret = GRIB_SUCCESS;
    long Ni = 0,plpresent  = 0;
    long* pl = NULL; // pl array
    size_t plsize = 0;
    grib_context* c = h->context;

    // is there a PL array?
    ret = grib_get_long(h, "PLPresent", &plpresent);
    if (ret != GRIB_SUCCESS || plpresent == 0)
        return GRIB_SUCCESS; // No PL array. So nothing to do

    if ((ret = grib_get_size(h, "pl", &plsize)) != GRIB_SUCCESS)
        return ret;
    if (plsize == 0) { // pl array must have at least one element
        return GRIB_WRONG_GRID;
    }

    // If we have a PL array and PLPresent=true, then Ni must be missing
    ret = grib_get_long(h, "Ni", &Ni);
    if (ret == GRIB_SUCCESS && Ni != GRIB_MISSING_LONG) {
        grib_context_log(c, GRIB_LOG_ERROR, "Invalid Ni: If there is a PL array, Ni must be set to MISSING");
        return GRIB_WRONG_GRID;
    }

    pl = (long*)grib_context_malloc_clear(c, sizeof(long) * plsize);
    if (!pl) return GRIB_OUT_OF_MEMORY;
    if ((ret = grib_get_long_array(h, "pl", pl, &plsize)) != GRIB_SUCCESS)
        return ret;

    for (size_t j = 0; j < plsize; j++) {
        if (pl[j] == 0) {
            grib_context_log(c, GRIB_LOG_ERROR, "Invalid PL array: entry at index=%d is zero", j);
            grib_context_free(c, pl);
            return GRIB_WRONG_GRID;
        }
    }
    grib_context_free(c, pl);

    return GRIB_SUCCESS;
}

static int check_geoiterator(grib_handle* h)
{
    //printf("DEBUG  %s \n", __func__);
    int err = 0;

#if defined(HAVE_GEOGRAPHY)
    grib_iterator* iter = grib_iterator_new(h, 0, &err);
    if (err == GRIB_NOT_IMPLEMENTED || err == GRIB_SUCCESS) {
        grib_iterator_delete(iter);
        return GRIB_SUCCESS; // GRIB_NOT_IMPLEMENTED is OK e.g., for spectral fields
    }

    grib_context_log(h->context, GRIB_LOG_ERROR, "%s", grib_get_error_message(err));
    grib_iterator_delete(iter);
#endif

    return err;
}

static int check_7777(grib_handle* h)
{
    if (!grib_is_defined(h, "7777")) {
        return GRIB_7777_NOT_FOUND;
    }
    return GRIB_SUCCESS;
}

static int check_steps(grib_handle* h)
{
    char stepType[32] = {0,};
    size_t size = sizeof(stepType) / sizeof(*stepType);
    int err = grib_get_string_internal(h, "stepType", stepType, &size);
    if (err) return err;
    if (!STR_EQUAL(stepType, "instant")) {
        // check start and end steps
        long startStep = 0;
        err = grib_get_long_internal(h, "startStep", &startStep);
        if (err) return err;
        long endStep = 0;
        err = grib_get_long_internal(h, "endStep", &endStep);
        if (err) return err;
        if (startStep > endStep) {
            grib_context_log(h->context, GRIB_LOG_ERROR, "Invalid step: startStep > endStep (%ld > %ld)", startStep, endStep);
            return GRIB_WRONG_STEP;
        }
    }
    return GRIB_SUCCESS;
}

static int check_section_numbers(grib_handle* h, long edition, const int* sec_nums, size_t N)
{
    for (size_t i = 0; i < N; ++i) {
        char sname[16] = {0,};
        const int sec_num = sec_nums[i];
        snprintf(sname, sizeof(sname), "section_%d", sec_num);
        if (!grib_is_defined(h, sname)) {
            grib_context_log(h->context, GRIB_LOG_ERROR, "GRIB%ld: Section %d is missing!", edition, sec_num);
            return GRIB_INVALID_MESSAGE;
        }
    }
    return GRIB_SUCCESS;
}

static int check_namespace_keys(grib_handle* h)
{
    const char* ns = "ls";
    grib_keys_iterator* kiter = grib_keys_iterator_new(h, /*flags=*/0, ns);
    if (!kiter) return GRIB_DECODING_ERROR;
    int count = 0;
    while (grib_keys_iterator_next(kiter)) {
        ++count;
        const char* name = grib_keys_iterator_get_name(kiter);
        int type = 0;
        grib_get_native_type(h, name, &type);
        if ( STR_EQUAL(grib_get_type_name(type), "unknown") ) {
            grib_context_log(h->context, GRIB_LOG_ERROR, "Key %s has unknown type", name);
            return GRIB_DECODING_ERROR;
        }
    }
    if (count == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Message has no keys in the '%s' namespace", ns);
        return GRIB_DECODING_ERROR;
    }
    grib_keys_iterator_delete(kiter);
    return GRIB_SUCCESS;
}

static int check_sections(grib_handle* h)
{
    long edition = 0;
    int err = grib_get_long_internal(h, "edition", &edition);
    if (err) return err;

    if (edition == 1) {
        const int grib1_section_nums[] = {1, 2, 4}; // section 3 is optional
        const size_t N = sizeof(grib1_section_nums) / sizeof(grib1_section_nums[0]);
        err = check_section_numbers(h, edition, grib1_section_nums, N);
        if (err) return err;
    }
    else if (edition == 2) {
        const int grib2_section_nums[] = {1, 3, 4, 5, 6, 7, 8}; // section 2 is optional
        const size_t N = sizeof(grib2_section_nums) / sizeof(grib2_section_nums[0]);
        err = check_section_numbers(h, edition, grib2_section_nums, N);
        if (err) return err;
    }
    return GRIB_SUCCESS;
}

typedef int (*proj_func)(grib_handle*);
static proj_func check_functions[] = {
    check_sections,
    check_field_values,
    check_grid_pl_array,
    check_geoiterator,
    check_steps,
    check_7777,
    check_namespace_keys
};

int grib_accessor_message_is_valid_t::unpack_long(long* val, size_t* len)
{
    int ret = 0;
    grib_handle* h = grib_handle_of_accessor(this);

    *len = 1;
    *val = 1; // Assume message is valid

    char product[32] = {0,};
    size_t size = sizeof(product) / sizeof(*product);
    ret = grib_get_string(h, product_, product, &size);
    if (ret) return ret;

    if (!STR_EQUAL(product, "GRIB")) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "Validity checks only implemented for GRIB messages");
        *val = 0;
        return GRIB_NOT_IMPLEMENTED;
    }

    const size_t num_check_functions = sizeof(check_functions) / sizeof(check_functions[0]);
    for (size_t i = 0; i < num_check_functions; ++i) {
        proj_func cfunc = check_functions[i];
        if ((ret = cfunc(h)) != GRIB_SUCCESS) {
            *val = 0; // check failed
        }
    }

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
