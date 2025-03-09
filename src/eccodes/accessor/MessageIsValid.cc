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

eccodes::accessor::MessageIsValid _grib_accessor_message_is_valid;
eccodes::Accessor* grib_accessor_message_is_valid = &_grib_accessor_message_is_valid;

namespace eccodes::accessor
{

static const char* TITLE = "Message validity checks";

void MessageIsValid::init(const long l, grib_arguments* arg)
{
    Long::init(l, arg);

    grib_handle* h = grib_handle_of_accessor(this);
    product_ = arg->get_name(h, 0);
    edition_ = 0;

    flags_ |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    length_ = 0;
}

int MessageIsValid::check_date()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    long year, month, day, hour, min, sec;
    int err = grib_get_long_internal(handle_, "year", &year);
    if (err) return err;
    err = grib_get_long_internal(handle_, "month", &month);
    if (err) return err;
    err = grib_get_long_internal(handle_, "day", &day);
    if (err) return err;
    err = grib_get_long_internal(handle_, "hour", &hour);
    if (err) return err;
    err = grib_get_long_internal(handle_, "minute", &min);
    if (err) return err;
    err = grib_get_long_internal(handle_, "second", &sec);
    if (err) return err;

    if (!is_date_valid(year, month, day, hour, min, sec)) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Invalid date/time: %ld-%ld-%ld %ld:%ld:%ld",
                         TITLE, year, month, day, hour, min, sec);
        return GRIB_INVALID_MESSAGE ;
    }
    return GRIB_SUCCESS;
}

int MessageIsValid::check_grid_and_packing_type()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    char gridType[128] = {0,};
    size_t len = sizeof(gridType);
    int err = grib_get_string_internal(handle_, "gridType", gridType, &len);
    if (err) return err;
    if (STR_EQUAL(gridType, "unknown") || STR_EQUAL(gridType, "unknown_PLPresent")) {
        grib_context_log(context_, GRIB_LOG_ERROR, "%s: Key gridType=%s", TITLE, gridType);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    char packing_type[128] = {0,};
    len = sizeof(packing_type);
    err = grib_get_string_internal(handle_, "packingType", packing_type, &len);
    if (err) return err;

    const bool is_spectral_grid = (STR_EQUAL(gridType, "sh") ||
                                   STR_EQUAL(gridType, "rotated_sh") ||
                                   STR_EQUAL(gridType, "stretched_sh") ||
                                   STR_EQUAL(gridType, "stretched_rotated_sh") ||
                                   STR_EQUAL(gridType, "lambert_bf") ||
                                   STR_EQUAL(gridType, "polar_stereographic_bf") ||
                                   STR_EQUAL(gridType, "mercator_bf"));
    const bool is_spectral_packing = (STR_EQUAL(packing_type, "spectral_complex") ||
                                      STR_EQUAL(packing_type, "spectral_simple") ||
                                      STR_EQUAL(packing_type, "spectral_ieee") ||
                                      STR_EQUAL(packing_type, "bifourier_complex"));

    if ( (is_spectral_grid && !is_spectral_packing) ||
         (!is_spectral_grid && is_spectral_packing) )
    {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Mismatch between gridType (=%s) and packingType (=%s)",
                         TITLE, gridType, packing_type);
        return GRIB_INVALID_MESSAGE;
    }

    return GRIB_SUCCESS;
}

// int grib_accessor_message_is_valid_t::check_field_values()
// {
//     if (handle_->context->debug)
//         fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);
//     int err = GRIB_SUCCESS;
//     double* values = NULL;
//     size_t size = 0;
//     grib_context* c = handle_->context;
//     if ((err = grib_get_size(handle_, "values", &size)) != GRIB_SUCCESS)
//         return err;
//     values = (double*)grib_context_malloc(c, size * sizeof(double));
//     if (!values)
//         return GRIB_OUT_OF_MEMORY;
//     if ((err = grib_get_double_array(handle_, "values", values, &size)) != GRIB_SUCCESS) {
//         if (err == GRIB_FUNCTIONALITY_NOT_ENABLED) {
//             err = GRIB_SUCCESS;
//         }
//         grib_context_free(c, values);
//         return err;
//     }
//     grib_context_free(c, values);
//     return GRIB_SUCCESS;
// }

int MessageIsValid::check_number_of_missing()
{
    if (edition_ != 2) return GRIB_SUCCESS;

    int err = 0;
    long isGridded = -1;
    err = grib_get_long_internal(handle_, "isGridded", &isGridded);
    if (!err && isGridded == 0)
        return GRIB_SUCCESS; // Spectral data does not have missing field values

    // Some packing types like grid_complex and run length encoding
    // do not store missing values in the bitmap
    long missingValueManagementUsed = -1;
    err = grib_get_long(handle_, "missingValueManagementUsed", &missingValueManagementUsed);
    if (!err && missingValueManagementUsed == 1)
        return GRIB_SUCCESS;

    char packing_type[100] = {0,};
    size_t len = sizeof(packing_type);
    err = grib_get_string(handle_, "packingType", packing_type, &len);
    if (!err && STR_EQUAL(packing_type, "grid_run_length"))
        return GRIB_SUCCESS;

    long numberOfDataPoints = 0;
    if ((err = grib_get_long_internal(handle_, "numberOfDataPoints", &numberOfDataPoints)) != GRIB_SUCCESS)
        return err;
    long numberOfCodedValues = 0;
    if ((err = grib_get_long_internal(handle_, "numberOfCodedValues", &numberOfCodedValues)) != GRIB_SUCCESS)
        return err;
    long numberOfMissing = 0;
    if ((err = grib_get_long_internal(handle_, "numberOfMissing", &numberOfMissing)) != GRIB_SUCCESS)
        return err;

    if (numberOfCodedValues + numberOfMissing != numberOfDataPoints) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                "%s: numberOfCodedValues + numberOfMissing != numberOfDataPoints (%ld + %ld != %ld)",
                TITLE, numberOfCodedValues, numberOfMissing, numberOfDataPoints);
        return GRIB_INVALID_MESSAGE;
    }
    return GRIB_SUCCESS;
}

int MessageIsValid::check_grid_pl_array()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    int err = GRIB_SUCCESS;
    long Ni = 0,plpresent  = 0;
    long* pl = NULL; // pl array
    size_t plsize = 0;
    grib_context* c = handle_->context;

    // is there a PL array?
    err = grib_get_long(handle_, "PLPresent", &plpresent);
    if (err != GRIB_SUCCESS || plpresent == 0)
        return GRIB_SUCCESS; // No PL array. So nothing to do

    char gridType[128] = {0,};
    size_t len = 128;
    err = grib_get_string_internal(handle_, "gridType", gridType, &len);
    if (err != GRIB_SUCCESS) return err;

    if ((err = grib_get_size(handle_, "pl", &plsize)) != GRIB_SUCCESS)
        return err;
    if (plsize == 0) { // pl array must have at least one element
        return GRIB_WRONG_GRID;
    }

    // If we have a PL array and PLPresent=true, then Ni must be missing
    err = grib_get_long(handle_, "Ni", &Ni);
    if (err == GRIB_SUCCESS && Ni != GRIB_MISSING_LONG) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Invalid Ni: If there is a PL array, Ni must be set to MISSING", TITLE);
        return GRIB_WRONG_GRID;
    }

    long interpretationOfNumberOfPoints = 0;
    err = grib_get_long_internal(handle_, "interpretationOfNumberOfPoints", &interpretationOfNumberOfPoints);
    if (interpretationOfNumberOfPoints != 1) {
        grib_context_log(c, GRIB_LOG_ERROR,
            "%s: For a reduced grid, interpretationOfNumberOfPoints should be 1 (See Code Table 3.11)", TITLE);
        return GRIB_WRONG_GRID;
    }

    pl = (long*)grib_context_malloc_clear(c, sizeof(long) * plsize);
    if (!pl) return GRIB_OUT_OF_MEMORY;
    if ((err = grib_get_long_array_internal(handle_, "pl", pl, &plsize)) != GRIB_SUCCESS)
        return err;

    long numberOfDataPoints = 0;
    if ((err = grib_get_long_internal(handle_, "numberOfDataPoints", &numberOfDataPoints)) != GRIB_SUCCESS)
        return err;
    size_t sum_pl = 0;
    for (size_t j = 0; j < plsize; j++) sum_pl += pl[j];

    if (STR_EQUAL(gridType, "reduced_ll")) {
        // For reduced_ll grids, sum(pl) must equal numberOfDataPoints in every situation
        if (sum_pl != (size_t)numberOfDataPoints) {
            grib_context_log(c, GRIB_LOG_ERROR, "%s: Sum of PL array (=%zu) must equal numberOfDataPoints (=%ld)",
                             TITLE, sum_pl, numberOfDataPoints);
            grib_context_free(c, pl);
            return GRIB_WRONG_GRID;
        }
    }
    else {
        // Other reduced grids
        // Unfortunately in our archive we have reduced_ll grids with zeroes in the pl!
        for (size_t j = 0; j < plsize; j++) {
            if (pl[j] == 0) {
                grib_context_log(c, GRIB_LOG_ERROR, "%s: Invalid PL array: entry at index=%zu is zero", TITLE, j);
                grib_context_free(c, pl);
                return GRIB_WRONG_GRID;
            }
        }
        if (sum_pl < (size_t)numberOfDataPoints) {
            grib_context_log(c, GRIB_LOG_ERROR, "%s: Sum of PL array (=%zu) cannot be less than numberOfDataPoints (=%ld)",
                    TITLE, sum_pl, numberOfDataPoints);
            grib_context_free(c, pl);
            return GRIB_WRONG_GRID;
        }
        // Must be symmetric i.e., northern and southern hemispheres must be the same
        long global = 0;
        err = grib_get_long(handle_, "global", &global);
        if (!err && global) {
            for (size_t i = 0; i < plsize / 2; ++i) {
                const long pl_start = pl[i];
                const long pl_end   = pl[plsize - 1 - i];
                if (pl_start != pl_end) {
                    grib_context_log(c, GRIB_LOG_ERROR, "%s: PL array is not symmetric: pl[%zu]=%ld, pl[%zu]=%ld (gridType=%s)\n",
                                    TITLE, i, pl_start, plsize - 1 - i, pl_end, gridType);
                }
            }
        }
    }

    grib_context_free(c, pl);

    return GRIB_SUCCESS;
}

int MessageIsValid::check_geoiterator()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    int err = 0;

#if defined(HAVE_GEOGRAPHY)
    grib_iterator* iter = grib_iterator_new(handle_, GRIB_GEOITERATOR_NO_VALUES, &err);
    if (err == GRIB_NOT_IMPLEMENTED || err == GRIB_SUCCESS || err == GRIB_FUNCTIONALITY_NOT_ENABLED) {
        grib_iterator_delete(iter);
        return GRIB_SUCCESS; // GRIB_NOT_IMPLEMENTED is OK e.g., for spectral fields
    }

    grib_context_log(handle_->context, GRIB_LOG_ERROR, "%s: %s", TITLE, grib_get_error_message(err));
    grib_iterator_delete(iter);
#endif

    return err;
}

int MessageIsValid::check_7777()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    if (!grib_is_defined(handle_, "7777")) {
        return GRIB_7777_NOT_FOUND;
    }
    return GRIB_SUCCESS;
}

int MessageIsValid::check_surface_keys()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    int err = 0;
    const grib_context* c = handle_->context;

    if (edition_ != 2) return GRIB_SUCCESS;

    if (!grib_is_defined(handle_, "typeOfFirstFixedSurface"))
        return GRIB_SUCCESS; // nothing to do

    long stype=0;
    grib_get_long_internal(handle_, "typeOfFirstFixedSurface", &stype);
    int sfac_missing = grib_is_missing(handle_, "scaleFactorOfFirstFixedSurface", &err);
    int sval_missing = grib_is_missing(handle_, "scaledValueOfFirstFixedSurface", &err);
    if ((stype == 255 && !sfac_missing) || (stype == 255 && !sval_missing)) {
        grib_context_log(c, GRIB_LOG_ERROR,
            "%s: First fixed surface: If the type of surface is missing so should its scaleFactor/scaledValue keys", TITLE);
        return GRIB_INVALID_KEY_VALUE;
    }
    if (sfac_missing != sval_missing) {
        grib_context_log(c, GRIB_LOG_ERROR,
            "%s: First fixed surface: If the scale factor is missing so should the scaled value and vice versa", TITLE);
        return GRIB_INVALID_KEY_VALUE;
    }
    if (stype != 255) {
        int required_val = codes_grib_surface_type_requires_value(edition_, stype, &err);
        if (err) return err;
        if (required_val && (sfac_missing || sval_missing)) {
            char name[128] = {0,};
            size_t len = 128;
            grib_get_string(handle_, "nameOfFirstFixedSurface", name, &len);
            grib_context_log(c, GRIB_LOG_ERROR, "%s: First fixed surface: Type %ld (%s) requires a level", TITLE, stype, name);
            return GRIB_INVALID_KEY_VALUE;
        }
        // TODO(masn): generalise this. Need to check with DGOV
        if (stype == 1 && (!sfac_missing || !sval_missing)) {
            grib_context_log(c, GRIB_LOG_ERROR,
                "%s: First fixed surface: If type=%ld, scaleFactor/scaledValue keys must be set to missing", TITLE, stype);
            return GRIB_INVALID_KEY_VALUE;
        }
    }

    grib_get_long_internal(handle_, "typeOfSecondFixedSurface", &stype);
    sfac_missing = grib_is_missing(handle_, "scaleFactorOfSecondFixedSurface", &err);
    sval_missing = grib_is_missing(handle_, "scaledValueOfSecondFixedSurface", &err);
    if ((stype == 255 && !sfac_missing) || (stype == 255 && !sval_missing)) {
        grib_context_log(c, GRIB_LOG_ERROR,
            "%s: Second fixed surface: If the type of surface is missing so should its scaleFactor/scaledValue keys", TITLE);
        return GRIB_INVALID_KEY_VALUE;
    }
    if (sfac_missing != sval_missing) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Second fixed surface: If the scale factor is missing so should the scaled value and vice versa", TITLE);
        return GRIB_INVALID_KEY_VALUE;
    }
    if (stype != 255) {
        int required_val = codes_grib_surface_type_requires_value(edition_, stype, &err);
        if (err) return err;
        if (required_val && (sfac_missing || sval_missing)) {
            char name[128] = {0,};
            size_t len = 128;
            grib_get_string(handle_, "nameOfSecondFixedSurface", name, &len);
            grib_context_log(c, GRIB_LOG_ERROR, "%s: Second fixed surface: Type %ld (%s) requires a level", TITLE, stype, name);
            return GRIB_INVALID_KEY_VALUE;
        }
    }

    return GRIB_SUCCESS;
}

int MessageIsValid::check_steps()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    char stepType[32] = {0,};
    size_t size = sizeof(stepType) / sizeof(*stepType);
    int err = grib_get_string_internal(handle_, "stepType", stepType, &size);
    if (err) return err;
    if (!STR_EQUAL(stepType, "instant")) {
        // check start and end steps
        long startStep = 0;
        err = grib_get_long_internal(handle_, "startStep", &startStep);
        if (err) return err;
        long endStep = 0;
        err = grib_get_long_internal(handle_, "endStep", &endStep);
        if (err) return err;
        if (startStep > endStep) {
            grib_context_log(handle_->context, GRIB_LOG_ERROR, "%s: Invalid step: startStep > endStep (%ld > %ld)", TITLE, startStep, endStep);
            return GRIB_WRONG_STEP;
        }
        // Accumulations, average etc
        // TODO(masn): Generalise this rule. Beware of index and stdanom where start == end!
        if ( STR_EQUAL(stepType, "accum") || STR_EQUAL(stepType, "avg") || STR_EQUAL(stepType, "min") || STR_EQUAL(stepType, "max") ) {
            if (startStep == endStep) {
                grib_context_log(handle_->context, GRIB_LOG_ERROR,
                    "%s: Invalid steps: stepType=%s but startStep=endStep", TITLE, stepType);
                return GRIB_WRONG_STEP;
            }
        }
    }
    return GRIB_SUCCESS;
}

int MessageIsValid::check_section_numbers(const int* sec_nums, size_t N)
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    for (size_t i = 0; i < N; ++i) {
        char sname[16] = {0,};
        const int sec_num = sec_nums[i];
        snprintf(sname, sizeof(sname), "section_%d", sec_num);
        if (!grib_is_defined(handle_, sname)) {
            grib_context_log(handle_->context, GRIB_LOG_ERROR, "%s: GRIB%ld: Section %d is missing!", TITLE, edition_, sec_num);
            return GRIB_INVALID_MESSAGE;
        }
    }
    return GRIB_SUCCESS;
}

int MessageIsValid::check_namespace_keys()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    const char* ns = "ls";
    grib_keys_iterator* kiter = grib_keys_iterator_new(handle_, /*flags=*/0, ns);
    if (!kiter) return GRIB_DECODING_ERROR;
    int count = 0;
    while (grib_keys_iterator_next(kiter)) {
        ++count;
        const char* name = grib_keys_iterator_get_name(kiter);
        int type = 0;
        grib_get_native_type(handle_, name, &type);
        if ( STR_EQUAL(grib_get_type_name(type), "unknown") ) {
            grib_context_log(handle_->context, GRIB_LOG_ERROR, "%s: Key %s has unknown type", TITLE, name);
            return GRIB_DECODING_ERROR;
        }
    }
    if (count == 0) {
        grib_context_log(handle_->context, GRIB_LOG_ERROR, "%s: Message has no keys in the '%s' namespace", TITLE, ns);
        return GRIB_DECODING_ERROR;
    }
    grib_keys_iterator_delete(kiter);
    return GRIB_SUCCESS;
}

int MessageIsValid::check_sections()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    int err = 0;
    if (edition_ == 1) {
        const int grib1_section_nums[] = {1, 2, 4}; // section 3 is optional
        const size_t N = sizeof(grib1_section_nums) / sizeof(grib1_section_nums[0]);
        err = check_section_numbers(grib1_section_nums, N);
        if (err) return err;
    }
    else if (edition_ == 2) {
        const int grib2_section_nums[] = {1, 3, 4, 5, 6, 7, 8}; // section 2 is optional
        const size_t N = sizeof(grib2_section_nums) / sizeof(grib2_section_nums[0]);
        err = check_section_numbers(grib2_section_nums, N);
        if (err) return err;
    }
    return GRIB_SUCCESS;
}

int MessageIsValid::check_parameter()
{
    if (handle_->context->debug)
        fprintf(stderr, "ECCODES DEBUG %s: %s\n", TITLE, __func__);

    int err = 0;
    long centre = 0;
    err = grib_get_long_internal(handle_, "centre", &centre);
    if (err) return err;
    if (centre == 98) {
        // make sure paramId is valid for ECMWF data
        long paramId = 0;
        err = grib_get_long_internal(handle_, "paramId", &paramId);
        if (err) return err;
        if (paramId == 0) {
            grib_context_log(handle_->context, GRIB_LOG_ERROR, "%s: paramId is 0 (parameter is not mapped)", TITLE);
            return GRIB_INVALID_MESSAGE;
        }
    }

    return err;
}

int MessageIsValid::unpack_long(long* val, size_t* len)
{
    typedef int (MessageIsValid::*check_func)();
    static check_func check_functions[] = {
        &MessageIsValid::check_date,
        &MessageIsValid::check_grid_and_packing_type,
        // &MessageIsValid::check_field_values,
        &MessageIsValid::check_number_of_missing,
        &MessageIsValid::check_grid_pl_array,
        &MessageIsValid::check_geoiterator,
        &MessageIsValid::check_7777,
        &MessageIsValid::check_surface_keys,
        &MessageIsValid::check_steps,
        &MessageIsValid::check_namespace_keys,
        &MessageIsValid::check_sections,
        &MessageIsValid::check_parameter,
    };

    int err = 0;
    handle_ = grib_handle_of_accessor(this);

    *len = 1;
    *val = 1; // Assume message is valid

    char product[32] = {0,};
    size_t size = sizeof(product) / sizeof(*product);
    err = grib_get_string_internal(handle_, product_, product, &size);
    if (err) return err;

    if (!STR_EQUAL(product, "GRIB")) {
        grib_context_log(handle_->context, GRIB_LOG_ERROR, "Validity checks only implemented for GRIB messages");
        *val = 0;
        return GRIB_NOT_IMPLEMENTED;
    }

    err = grib_get_long_internal(handle_, "edition", &edition_);
    if (err) return err;

    const size_t num_check_functions = sizeof(check_functions) / sizeof(check_functions[0]);
    for (size_t i = 0; i < num_check_functions; ++i) {
        check_func cfunc = check_functions[i];
        err = (this->*cfunc)();
        if (err) {
            *val = 0; // check failed
        }
    }

    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
