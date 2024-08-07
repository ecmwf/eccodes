/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_data_jpeg2000_packing.h"

grib_accessor_class_data_jpeg2000_packing_t _grib_accessor_class_data_jpeg2000_packing{ "data_jpeg2000_packing" };
grib_accessor_class* grib_accessor_class_data_jpeg2000_packing = &_grib_accessor_class_data_jpeg2000_packing;


static int first = 1;

#define JASPER_LIB   1
#define OPENJPEG_LIB 2

void grib_accessor_class_data_jpeg2000_packing_t::init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_class_data_simple_packing_t::init(a, v, args);
    const char* user_lib                        = NULL;
    grib_accessor_data_jpeg2000_packing_t* self = (grib_accessor_data_jpeg2000_packing_t*)a;
    grib_handle* hand                           = grib_handle_of_accessor(a);

    self->jpeg_lib                 = 0;
    self->type_of_compression_used = grib_arguments_get_name(hand, args, self->carg++);
    self->target_compression_ratio = grib_arguments_get_name(hand, args, self->carg++);
    self->ni                       = grib_arguments_get_name(hand, args, self->carg++);
    self->nj                       = grib_arguments_get_name(hand, args, self->carg++);
    self->list_defining_points     = grib_arguments_get_name(hand, args, self->carg++);
    self->number_of_data_points    = grib_arguments_get_name(hand, args, self->carg++);
    self->scanning_mode            = grib_arguments_get_name(hand, args, self->carg++);
    self->edition                  = 2;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;

#if HAVE_LIBJASPER
    self->jpeg_lib = JASPER_LIB;
#elif HAVE_LIBOPENJPEG
    self->jpeg_lib = OPENJPEG_LIB;
#endif

    if ((user_lib = codes_getenv("ECCODES_GRIB_JPEG")) != NULL) {
        if (!strcmp(user_lib, "jasper")) {
            self->jpeg_lib = JASPER_LIB;
        }
        else if (!strcmp(user_lib, "openjpeg")) {
            self->jpeg_lib = OPENJPEG_LIB;
        }
    }

    if (a->context->debug) {
        switch (self->jpeg_lib) {
            case 0:
                fprintf(stderr, "ECCODES DEBUG jpeg2000_packing: jpeg_lib not set!\n");
                break;
            case JASPER_LIB:
                fprintf(stderr, "ECCODES DEBUG jpeg2000_packing: using JASPER_LIB\n");
                break;
            case OPENJPEG_LIB:
                fprintf(stderr, "ECCODES DEBUG jpeg2000_packing: using OPENJPEG_LIB\n");
                break;
            default:
                Assert(0);
                break;
        }
    }

    self->dump_jpg = codes_getenv("ECCODES_GRIB_DUMP_JPG_FILE");
    if (self->dump_jpg) {
        if (first) {
            printf("GRIB JPEG dumping to %s\n", self->dump_jpg);
            first = 0;
        }
    }
}

int grib_accessor_class_data_jpeg2000_packing_t::value_count(grib_accessor* a, long* n_vals)
{
    grib_accessor_data_jpeg2000_packing_t* self = (grib_accessor_data_jpeg2000_packing_t*)a;
    *n_vals                                     = 0;

    return grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_values, n_vals);
}

#define EXTRA_BUFFER_SIZE 10240

#if HAVE_JPEG
int grib_accessor_class_data_jpeg2000_packing_t::unpack_float(grib_accessor*, float* val, size_t* len)
{
    return GRIB_NOT_IMPLEMENTED;
}

int grib_accessor_class_data_jpeg2000_packing_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_data_jpeg2000_packing_t* self = (grib_accessor_data_jpeg2000_packing_t*)a;

    int err            = GRIB_SUCCESS;
    size_t i           = 0;
    size_t buflen      = a->byte_count();
    double bscale      = 0;
    double dscale      = 0;
    unsigned char* buf = NULL;
    size_t n_vals      = 0;
    long nn            = 0;

    long binary_scale_factor  = 0;
    long decimal_scale_factor = 0;
    double reference_value    = 0;
    long bits_per_value       = 0;
    double units_factor       = 1.0;
    double units_bias         = 0.0;

    n_vals = 0;
    err    = a->value_count(&nn);
    n_vals = nn;
    if (err)
        return err;

    if (self->units_factor)
        grib_get_double_internal(grib_handle_of_accessor(a), self->units_factor, &units_factor);

    if (self->units_bias)
        grib_get_double_internal(grib_handle_of_accessor(a), self->units_bias, &units_bias);


    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    self->dirty = 0;

    bscale = codes_power<double>(binary_scale_factor, 2);
    dscale = codes_power<double>(-decimal_scale_factor, 10);

    /* TODO: This should be called upstream */
    if (*len < n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    /* Special case */

    if (bits_per_value == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    buf += a->byte_offset();
    switch (self->jpeg_lib) {
        case OPENJPEG_LIB:
            if ((err = grib_openjpeg_decode(a->context, buf, &buflen, val, &n_vals)) != GRIB_SUCCESS)
                return err;
            break;
        case JASPER_LIB:
            if ((err = grib_jasper_decode(a->context, buf, &buflen, val, &n_vals)) != GRIB_SUCCESS)
                return err;
            break;
        default:
            grib_context_log(a->context, GRIB_LOG_ERROR, "Unable to unpack. Invalid JPEG library.\n");
            return GRIB_DECODING_ERROR;
    }

    *len = n_vals;

    for (i = 0; i < n_vals; i++) {
        val[i] = (val[i] * bscale + reference_value) * dscale;
    }
    if (units_factor != 1.0) {
        if (units_bias != 0.0) {
            for (i = 0; i < n_vals; i++) {
                val[i] = val[i] * units_factor + units_bias;
            }
        }
        else {
            for (i = 0; i < n_vals; i++) {
                val[i] *= units_factor;
            }
        }
    }
    else if (units_bias != 0.0) {
        for (i = 0; i < n_vals; i++) {
            val[i] += units_bias;
        }
    }

    return err;
}

int grib_accessor_class_data_jpeg2000_packing_t::pack_double(grib_accessor* a, const double* cval, size_t* len)
{
    grib_accessor_data_jpeg2000_packing_t* self = (grib_accessor_data_jpeg2000_packing_t*)a;
    size_t n_vals                               = *len;
    int err                                     = 0;
    int i;
    double reference_value     = 0;
    long binary_scale_factor   = 0;
    long bits_per_value        = 0;
    long decimal_scale_factor  = 0;
    double decimal             = 1;
    size_t simple_packing_size = 0;
    unsigned char* buf         = NULL;
    double divisor             = 1;
    long width;
    long height;
    long ni;
    long nj;
    long target_compression_ratio;
    long type_of_compression_used;
    long scanning_mode;
    long list_defining_points;
    long number_of_data_points;
    int ret = 0;
    j2k_encode_helper helper;
    double units_factor     = 1.0;
    double units_bias       = 0.0;
    double* val             = (double*)cval;
    const char* cclass_name = a->cclass->name;

    self->dirty = 1;

    if (*len == 0) {
        grib_buffer_replace(a, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    if (self->units_factor &&
        (grib_get_double_internal(grib_handle_of_accessor(a), self->units_factor, &units_factor) == GRIB_SUCCESS)) {
        grib_set_double_internal(grib_handle_of_accessor(a), self->units_factor, 1.0);
    }

    if (self->units_bias &&
        (grib_get_double_internal(grib_handle_of_accessor(a), self->units_bias, &units_bias) == GRIB_SUCCESS)) {
        grib_set_double_internal(grib_handle_of_accessor(a), self->units_bias, 0.0);
    }

    if (units_factor != 1.0) {
        if (units_bias != 0.0)
            for (i = 0; i < n_vals; i++)
                val[i] = val[i] * units_factor + units_bias;
        else
            for (i = 0; i < n_vals; i++)
                val[i] *= units_factor;
    }
    else if (units_bias != 0.0)
        for (i = 0; i < n_vals; i++)
            val[i] += units_bias;

    ret = grib_accessor_class_data_simple_packing_t::pack_double(a, val, len);

    switch (ret) {
        case GRIB_CONSTANT_FIELD:
            grib_buffer_replace(a, NULL, 0, 1, 1);
            err = grib_set_long_internal(grib_handle_of_accessor(a), self->number_of_values, *len);
            return err;
        case GRIB_SUCCESS:
            break;
        default:
            grib_context_log(a->context, GRIB_LOG_ERROR, "%s %s: Unable to compute packing parameters", cclass_name, __func__);
            return ret;
    }

    if ((ret = grib_get_double_internal(grib_handle_of_accessor(a), self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->bits_per_value, &bits_per_value)) !=
        GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(grib_handle_of_accessor(a), self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return ret;

    decimal = codes_power<double>(decimal_scale_factor, 10);
    divisor = codes_power<double>(-binary_scale_factor, 2);

    simple_packing_size = (((bits_per_value * n_vals) + 7) / 8) * sizeof(unsigned char);
    buf                 = (unsigned char*)grib_context_malloc_clear(a->context, simple_packing_size + EXTRA_BUFFER_SIZE);
    if (!buf) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }

    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->ni, &ni)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->nj, &nj)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->type_of_compression_used, &type_of_compression_used)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->target_compression_ratio, &target_compression_ratio)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->scanning_mode, &scanning_mode)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->list_defining_points, &list_defining_points)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(grib_handle_of_accessor(a), self->number_of_data_points, &number_of_data_points)) != GRIB_SUCCESS)
        return err;

    width  = ni;
    height = nj;

    if ((scanning_mode & (1 << 5)) != 0) {
        long tmp = width;
        width    = height;
        height   = tmp;
    }

    /* The grid is not regular */
    if (list_defining_points != 0) {
        width  = *len;
        height = 1;
    }

    /* There is a bitmap */
    if (*len != number_of_data_points) {
        width  = *len;
        height = 1;
    }

    if (width * height != *len) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "%s %s: width=%ld height=%ld len=%zu. width*height should equal len!",
                         cclass_name, __func__, width, height, *len);
        /* ECC-802: We cannot bomb out here as the user might have changed Ni/Nj and the packingType
         * but has not yet submitted the new data values. So len will be out of sync!
         * So issue a warning but proceed.
         */
        /*return GRIB_INTERNAL_ERROR;*/
        grib_context_free(a->context, buf);
        return GRIB_SUCCESS;
    }

    switch (type_of_compression_used) {
        case 0:  // Lossless
            if (target_compression_ratio != 255) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "%s %s: When %s=0 (Lossless), %s must be set to 255",
                                 cclass_name, __func__, self->type_of_compression_used, self->target_compression_ratio);
                return GRIB_ENCODING_ERROR;
            }
            helper.compression = 0;
            break;

        case 1:  // Lossy
            if (target_compression_ratio == 255 || target_compression_ratio == 0) {
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "%s %s: When %s=1 (Lossy), %s must be specified",
                                 cclass_name, __func__, self->type_of_compression_used, self->target_compression_ratio);
                return GRIB_ENCODING_ERROR;
            }
            Assert(target_compression_ratio != 255);
            Assert(target_compression_ratio != 0);
            helper.compression = target_compression_ratio;
            break;

        default:
            err = GRIB_NOT_IMPLEMENTED;
            goto cleanup;
    }

    helper.jpeg_buffer = buf;
    helper.width       = width;
    helper.height      = height;

    /* See GRIB-438 */
    if (bits_per_value == 0) {
        const long bits_per_value_adjusted = 1;
        grib_context_log(a->context, GRIB_LOG_DEBUG,
                         "%s (%s) : bits per value was zero, changed to %ld",
                         cclass_name, self->jpeg_lib == OPENJPEG_LIB ? "openjpeg" : "jasper", bits_per_value_adjusted);
        bits_per_value = bits_per_value_adjusted;
    }
    helper.bits_per_value = bits_per_value;

    helper.buffer_size     = simple_packing_size + EXTRA_BUFFER_SIZE;
    helper.values          = val;
    helper.no_values       = n_vals;
    helper.reference_value = reference_value;
    helper.divisor         = divisor;
    helper.decimal         = decimal;
    helper.jpeg_length     = 0;

    switch (self->jpeg_lib) {
        case OPENJPEG_LIB:
            if ((err = grib_openjpeg_encode(a->context, &helper)) != GRIB_SUCCESS)
                goto cleanup;
            break;
        case JASPER_LIB:
            if ((err = grib_jasper_encode(a->context, &helper)) != GRIB_SUCCESS)
                goto cleanup;
            break;
    }

    if (helper.jpeg_length > simple_packing_size)
        grib_context_log(a->context, GRIB_LOG_WARNING,
                         "%s (%s) : jpeg data (%ld) larger than input data (%ld)",
                         cclass_name, self->jpeg_lib == OPENJPEG_LIB ? "openjpeg" : "jasper",
                         helper.jpeg_length, simple_packing_size);

    Assert(helper.jpeg_length <= helper.buffer_size);

    if (self->dump_jpg) {
        FILE* f = fopen(self->dump_jpg, "w");
        if (f) {
            if (fwrite(helper.jpeg_buffer, helper.jpeg_length, 1, f) != 1)
                perror(self->dump_jpg);
            if (fclose(f) != 0)
                perror(self->dump_jpg);
        }
        else
            perror(self->dump_jpg);
    }

    grib_buffer_replace(a, helper.jpeg_buffer, helper.jpeg_length, 1, 1);

cleanup:

    grib_context_free(a->context, buf);

    if (err == GRIB_SUCCESS)
        err = grib_set_long_internal(grib_handle_of_accessor(a), self->number_of_values, *len);
    return err;
}
#else

static void print_error_feature_not_enabled(grib_context* c)
{
    grib_context_log(c, GRIB_LOG_ERROR,
                     "JPEG support not enabled. Please rebuild with -DENABLE_JPG=ON");
}
int grib_accessor_class_data_jpeg2000_packing_t::unpack_float(grib_accessor* a, float* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}
int grib_accessor_class_data_jpeg2000_packing_t::unpack_double(grib_accessor* a, double* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}

int grib_accessor_class_data_jpeg2000_packing_t::pack_double(grib_accessor* a, const double* val, size_t* len)
{
    print_error_feature_not_enabled(a->context);
    return GRIB_FUNCTIONALITY_NOT_ENABLED;
}

#endif

int grib_accessor_class_data_jpeg2000_packing_t::unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    grib_accessor_data_jpeg2000_packing_t* self = (grib_accessor_data_jpeg2000_packing_t*)a;
    grib_handle* hand                           = grib_handle_of_accessor(a);
    size_t size                                 = 0;
    double* values                              = NULL;
    int err                                     = 0;
    double reference_value                      = 0;
    long bits_per_value                         = 0;

    if ((err = grib_get_long_internal(hand, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    /* Special case of constant field */
    if (bits_per_value == 0) {
        *val = reference_value;
        return GRIB_SUCCESS;
    }

    /* GRIB-564: The index idx relates to codedValues NOT values! */
    err = grib_get_size(hand, "codedValues", &size);
    if (err)
        return err;
    if (idx > size)
        return GRIB_INVALID_ARGUMENT;

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(hand, "codedValues", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    *val = values[idx];
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}

int grib_accessor_class_data_jpeg2000_packing_t::unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    grib_accessor_data_jpeg2000_packing_t* self = (grib_accessor_data_jpeg2000_packing_t*)a;
    grib_handle* hand                           = grib_handle_of_accessor(a);
    size_t size = 0, i = 0;
    double* values         = NULL;
    int err                = 0;
    double reference_value = 0;
    long bits_per_value    = 0;

    if ((err = grib_get_long_internal(hand, self->bits_per_value, &bits_per_value)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(hand, self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;

    /* Special case of constant field */
    if (bits_per_value == 0) {
        for (i = 0; i < len; i++)
            val_array[i] = reference_value;
        return GRIB_SUCCESS;
    }

    /* GRIB-564: The indexes in index_array relate to codedValues NOT values! */
    err = grib_get_size(hand, "codedValues", &size);
    if (err) return err;

    for (i = 0; i < len; i++) {
        if (index_array[i] > size) return GRIB_INVALID_ARGUMENT;
    }

    values = (double*)grib_context_malloc_clear(a->context, size * sizeof(double));
    err    = grib_get_double_array(hand, "codedValues", values, &size);
    if (err) {
        grib_context_free(a->context, values);
        return err;
    }
    for (i = 0; i < len; i++) {
        val_array[i] = values[index_array[i]];
    }
    grib_context_free(a->context, values);
    return GRIB_SUCCESS;
}
