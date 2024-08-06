/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_run_length_packing.h"
#include "grib_scaling.h"

grib_accessor_class_data_run_length_packing_t _grib_accessor_class_data_run_length_packing{ "data_run_length_packing" };
grib_accessor_class* grib_accessor_class_data_run_length_packing = &_grib_accessor_class_data_run_length_packing;


void grib_accessor_class_data_run_length_packing_t::init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_class_values_t::init(a, v, args);
    grib_accessor_data_run_length_packing_t* self = (grib_accessor_data_run_length_packing_t*)a;
    grib_handle* gh                               = grib_handle_of_accessor(a);
    self->number_of_values                        = grib_arguments_get_name(gh, args, self->carg++);
    self->bits_per_value                          = grib_arguments_get_name(gh, args, self->carg++);
    self->max_level_value                         = grib_arguments_get_name(gh, args, self->carg++);
    self->number_of_level_values                  = grib_arguments_get_name(gh, args, self->carg++);
    self->decimal_scale_factor                    = grib_arguments_get_name(gh, args, self->carg++);
    self->level_values                            = grib_arguments_get_name(gh, args, self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

int grib_accessor_class_data_run_length_packing_t::value_count(grib_accessor* a, long* number_of_values)
{
    grib_accessor_data_run_length_packing_t* self = (grib_accessor_data_run_length_packing_t*)a;
    *number_of_values                             = 0;
    return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_values, number_of_values);
}

int grib_accessor_class_data_run_length_packing_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_data_run_length_packing_t* self = (grib_accessor_data_run_length_packing_t*)a;
    grib_handle* gh                               = grib_handle_of_accessor(a);
    const char* cclass_name                       = a->cclass->name;
    int err                                       = GRIB_SUCCESS;
    long seclen, number_of_values, bits_per_value, max_level_value, number_of_level_values, decimal_scale_factor;
    long* level_values       = NULL;
    size_t level_values_size = 0;
    long i = 0, number_of_compressed_values = 0, range = 0, offsetBeforeData = 0, pos = 0;
    long v, n, factor, k, j;
    long* compressed_values   = NULL;
    double level_scale_factor = 0;
    double* levels            = NULL;
    unsigned char* buf        = NULL;
    double missingValue       = 9999.0;

    if ((err = grib_get_long_internal(gh, self->seclen, &seclen)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->number_of_values, &number_of_values)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->max_level_value, &max_level_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->number_of_level_values, &number_of_level_values)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double(gh, "missingValue", &missingValue)) != GRIB_SUCCESS)
        return err;

    level_values      = (long*)grib_context_malloc_clear(a->context, sizeof(long) * number_of_level_values);
    level_values_size = number_of_level_values;
    if ((err = grib_get_long_array_internal(gh, self->level_values, level_values, &level_values_size)) != GRIB_SUCCESS)
        return err;
    *len                        = number_of_values;
    i                           = 0;
    number_of_compressed_values = ((seclen - 5) * 8) / bits_per_value;
    if (number_of_compressed_values == 0 || max_level_value == 0) {
        for (i = 0; i < number_of_values; i++) {
            val[i] = missingValue;
        }
        return GRIB_SUCCESS;
    }
    range = (1 << bits_per_value) - 1 - max_level_value;
    if ((max_level_value <= 0) || (number_of_level_values <= 0) || (max_level_value > number_of_level_values) || (range <= 0)) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: parameters are invalid: max_level_value=%ld(>0, <=number_of_level_values), "
                         "number_of_level_values=%ld(>0, >=max_level_value), range=%ld(>0)",
                         cclass_name, max_level_value, number_of_level_values, range);
        return GRIB_DECODING_ERROR;
    }
    if (decimal_scale_factor > 127) {
        decimal_scale_factor = -(decimal_scale_factor - 128);
    }
    level_scale_factor = codes_power<double>(-decimal_scale_factor, 10.0);
    levels             = (double*)grib_context_malloc_clear(a->context, sizeof(double) * (number_of_level_values + 1));
    levels[0]          = missingValue;
    for (i = 0; i < number_of_level_values; i++) {
        levels[i + 1] = level_values[i] * level_scale_factor;
    }
    compressed_values = (long*)grib_context_malloc_clear(a->context, sizeof(long) * number_of_compressed_values);
    buf               = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    offsetBeforeData  = a->byte_offset();
    buf += offsetBeforeData;
    pos = 0;
    grib_decode_long_array(buf, &pos, bits_per_value, number_of_compressed_values, compressed_values);
    j = 0;
    i = 0;
    while (i < number_of_compressed_values) {
        if (compressed_values[i] > max_level_value) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "%s: numberOfValues mismatch: i=%ld, "
                             "compressed_values[i]=%ld, max_level_value=%ld",
                             cclass_name, i, compressed_values[i], max_level_value);
            break;
        }
        v      = compressed_values[i++];
        n      = 1;
        factor = 1;
        while (i < number_of_compressed_values && compressed_values[i] > max_level_value) {
            n += factor * (compressed_values[i] - max_level_value - 1);
            factor = factor * range;
            i++;
        }
        if (n > number_of_values) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s: numberOfValues mismatch: n=%ld, number_of_values=%ld",
                             cclass_name, n, number_of_values);
            break;
        }
        for (k = 0; k < n; k++) {
            val[j++] = levels[v];
        }
    }
    grib_context_free(a->context, level_values);
    grib_context_free(a->context, levels);
    grib_context_free(a->context, compressed_values);
    if (j != number_of_values) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s: numberOfValues mismatch: j=%ld, number_of_values=%ld",
                         cclass_name, j, number_of_values);
        return GRIB_DECODING_ERROR;
    }
    return err;
}

int grib_accessor_class_data_run_length_packing_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_run_length_packing_t* self = (grib_accessor_data_run_length_packing_t*)a;
    grib_handle* gh                               = grib_handle_of_accessor(a);
    const char* cclass_name                       = a->cclass->name;
    int err                                       = GRIB_SUCCESS;
    long number_of_values, bits_per_value, max_level_value, number_of_level_values, decimal_scale_factor;
    long* level_values       = NULL;
    size_t level_values_size = 0;
    long i = 0, range = 0, pos = 0, n = 0, j = 0, k = 0, l = 0, missingValueLong = 0;
    double level_scale_factor = 0;
    unsigned char* buf        = NULL;
    double missingValue       = 9999.0;
    size_t n_vals             = *len;

    if ((err = grib_get_long_internal(gh, self->number_of_values, &number_of_values)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->max_level_value, &max_level_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->number_of_level_values, &number_of_level_values)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(gh, self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double(gh, "missingValue", &missingValue)) != GRIB_SUCCESS)
        return err;

    if (n_vals != number_of_values) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s: Parameters are invalid: n_vals=%ld(==number_of_values), number_of_values=%ld(==n_vals)",
                         cclass_name, n_vals, number_of_values);
        return GRIB_ENCODING_ERROR;
    }

    if (bits_per_value == 0) {
        // TODO(masn): What are we meant to do with constant fields?
        return GRIB_SUCCESS;
    }

    level_values      = (long*)grib_context_malloc_clear(a->context, sizeof(long) * number_of_level_values);
    level_values_size = number_of_level_values;
    if ((err = grib_get_long_array_internal(gh, self->level_values, level_values, &level_values_size)) != GRIB_SUCCESS)
        return err;
    if (decimal_scale_factor > 127) {
        decimal_scale_factor = -(decimal_scale_factor - 128);
    }
    level_scale_factor = grib_power(-decimal_scale_factor, 10.0);
    missingValueLong   = (long)(round(missingValue / level_scale_factor));
    for (i = 0; i < number_of_level_values; i++) {
        if (missingValueLong == level_values[i]) {
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s: Parameters are invalid: level_values[%ld]=%ld, missingValueLong=%ld",
                             cclass_name, i, level_values[i], missingValueLong);
            return GRIB_ENCODING_ERROR;
        }
    }
    range = (1 << bits_per_value) - 1 - max_level_value;
    if ((max_level_value <= 0) || (number_of_level_values <= 0) || (max_level_value > number_of_level_values) || (range <= 0)) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s: Parameters are invalid: max_level_value=%ld(>0, <=number_of_level_values), "
                         "number_of_level_values=%ld(>0, >=max_level_value), range=%ld(>0)",
                         cclass_name, max_level_value, number_of_level_values, range);
        return GRIB_ENCODING_ERROR;
    }
    buf = (unsigned char*)grib_context_malloc(a->context, 2 * number_of_values);
    for (i = 0; i < number_of_values; i++) {
        k   = (long)(round(val[i] / level_scale_factor));
        err = GRIB_ENCODING_ERROR;
        if (missingValueLong == k) {
            k   = 0;
            err = GRIB_SUCCESS;
        }
        else {
            for (j = 0; j < max_level_value; j++) {
                if (level_values[j] == k) {
                    k   = j + 1;
                    err = GRIB_SUCCESS;
                    break;
                }
            }
        }
        if (err != GRIB_SUCCESS) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "%s: Values and/or parameters are invalid: val[%ld]=%lf, level_value=%ld, max_level_value=%ld",
                             cclass_name, i, val[i], k, max_level_value);
            return GRIB_ENCODING_ERROR;
        }
        if (i == 0) {
            grib_encode_unsigned_longb(buf, k, &pos, bits_per_value);
        }
        else if (i == number_of_values - 1) {
            if (k != l) {
                if (n == 0) {
                    grib_encode_unsigned_longb(buf, k, &pos, bits_per_value);
                }
                else {
                    while (n >= range) {
                        grib_encode_unsigned_longb(buf, (n % range) + max_level_value + 1, &pos, bits_per_value);
                        n = n / range;
                    }
                    grib_encode_unsigned_longb(buf, n + max_level_value + 1, &pos, bits_per_value);
                    grib_encode_unsigned_longb(buf, k, &pos, bits_per_value);
                }
            }
            else {
                n = n + 1;
                while (n >= range) {
                    grib_encode_unsigned_longb(buf, (n % range) + max_level_value + 1, &pos, bits_per_value);
                    n = n / range;
                }
                grib_encode_unsigned_longb(buf, n + max_level_value + 1, &pos, bits_per_value);
            }
        }
        else {
            if (k != l) {
                if (n == 0) {
                    grib_encode_unsigned_longb(buf, k, &pos, bits_per_value);
                }
                else {
                    while (n >= range) {
                        grib_encode_unsigned_longb(buf, (n % range) + max_level_value + 1, &pos, bits_per_value);
                        n = n / range;
                    }
                    grib_encode_unsigned_longb(buf, n + max_level_value + 1, &pos, bits_per_value);
                    grib_encode_unsigned_longb(buf, k, &pos, bits_per_value);
                    n = 0;
                }
            }
            else {
                n = n + 1;
            }
        }
        l = k;
    }
    grib_context_free(a->context, level_values);
    grib_buffer_replace(a, buf, pos / 8, 1, 1);
    grib_context_buffer_free(a->context, buf);
    return err;
}
