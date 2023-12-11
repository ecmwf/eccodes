/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_value.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double;unpack_double_element;unpack_double_element_set
   IMPLEMENTS = unpack_float
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   IMPLEMENTS = dump;get_native_type
   IMPLEMENTS = compare
   MEMBERS=const char*  coded_values
   MEMBERS=const char*  bitmap
   MEMBERS=const char*  missing_value
   MEMBERS=const char*  number_of_data_points
   MEMBERS=const char*  number_of_values
   MEMBERS=const char*  binary_scale_factor
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_double(grib_accessor*, const double* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_float(grib_accessor*, float* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static int compare(grib_accessor*, grib_accessor*);
static int unpack_double_element(grib_accessor*, size_t i, double* val);
static int unpack_double_element_set(grib_accessor*, const size_t* index_array, size_t len, double* val_array);

typedef struct grib_accessor_data_apply_bitmap
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in data_apply_bitmap */
    const char*  coded_values;
    const char*  bitmap;
    const char*  missing_value;
    const char*  number_of_data_points;
    const char*  number_of_values;
    const char*  binary_scale_factor;
} grib_accessor_data_apply_bitmap;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_data_apply_bitmap = {
    &grib_accessor_class_gen,                      /* super */
    "data_apply_bitmap",                      /* name */
    sizeof(grib_accessor_data_apply_bitmap),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    &dump,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    &unpack_float,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    &compare,                    /* compare vs. another accessor */
    &unpack_double_element,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    &unpack_double_element_set,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_apply_bitmap = &_grib_accessor_class_data_apply_bitmap;

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    int n                                 = 0;
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;

    self->coded_values          = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->bitmap                = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->missing_value         = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->binary_scale_factor   = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->number_of_data_points = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);
    self->number_of_values      = grib_arguments_get_name(grib_handle_of_accessor(a), args, n++);

    a->length = 0;
}
static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_values(dumper, a);
}

static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;
    size_t len                            = 0;
    int ret                               = 0;

    if (grib_find_accessor(grib_handle_of_accessor(a), self->bitmap))
        ret = grib_get_size(grib_handle_of_accessor(a), self->bitmap, &len);
    else
        ret = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &len);

    *count = len;

    return ret;
}

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;
    grib_handle* gh      = grib_handle_of_accessor(a);
    int err = 0, i = 0;
    size_t cidx          = 0;
    double missing_value = 0;
    double* bvals        = NULL;
    size_t n_vals        = 0;
    long nn              = 0;

    err    = grib_value_count(a, &nn);
    n_vals = nn;
    if (err)
        return err;

    if (!grib_find_accessor(gh, self->bitmap))
        return grib_get_double_element_internal(gh, self->coded_values, idx, val);

    if ((err = grib_get_double_internal(gh, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_element_internal(gh, self->bitmap, idx, val)) != GRIB_SUCCESS)
        return err;

    if (*val == 0) {
        *val = missing_value;
        return GRIB_SUCCESS;
    }

    bvals = (double*)grib_context_malloc(a->context, n_vals * sizeof(double));
    if (bvals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, self->bitmap, bvals, &n_vals)) != GRIB_SUCCESS)
        return err;

    cidx = 0;
    for (i = 0; i < idx; i++) {
        cidx += bvals[i];
    }

    grib_context_free(a->context, bvals);

    return grib_get_double_element_internal(gh, self->coded_values, cidx, val);
}

static int unpack_double_element_set(grib_accessor* a, const size_t* index_array, size_t len, double* val_array)
{
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;
    grib_handle* gh = grib_handle_of_accessor(a);
    int err = 0, all_missing = 1;
    size_t cidx        = 0; /* index into the coded_values array */
    size_t* cidx_array = NULL; /* array of indexes into the coded_values */
    double* cval_array = NULL; /* array of values of the coded_values */
    double missing_value = 0;
    double* bvals        = NULL;
    size_t n_vals = 0, i = 0, j = 0, idx = 0, count_1s = 0, ci = 0;
    long nn = 0;

    err    = grib_value_count(a, &nn);
    n_vals = nn;
    if (err) return err;

    if (!grib_find_accessor(gh, self->bitmap))
        return grib_get_double_element_set_internal(gh, self->coded_values, index_array, len, val_array);

    if ((err = grib_get_double_internal(gh, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    err = grib_get_double_element_set_internal(gh, self->bitmap, index_array, len, val_array);
    if (err) return err;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 0) {
            val_array[i] = missing_value;
        } else {
            all_missing = 0;
            count_1s++;
        }
    }

    if (all_missing) {
        return GRIB_SUCCESS;
    }

    /* At this point val_array contains entries which are either missing_value or 1 */
    /* Now we need to dig into the codes values with index array of count_1s */

    bvals = (double*)grib_context_malloc(a->context, n_vals * sizeof(double));
    if (!bvals) return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_double_array_internal(gh, self->bitmap, bvals, &n_vals)) != GRIB_SUCCESS)
        return err;

    cidx_array = (size_t*)grib_context_malloc(a->context, count_1s * sizeof(size_t));
    cval_array = (double*)grib_context_malloc(a->context, count_1s * sizeof(double));

    ci = 0;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 1) {
            idx = index_array[i];
            cidx = 0;
            for (j = 0; j < idx; j++) {
                cidx += bvals[j];
            }
            Assert(ci < count_1s);
            cidx_array[ci++] = cidx;
        }
    }
    err = grib_get_double_element_set_internal(gh, self->coded_values, cidx_array, count_1s, cval_array);
    if (err) return err;

    /* Transfer from cval_array to our result val_array */
    ci = 0;
    for (i = 0; i < len; i++) {
        if (val_array[i] == 1) {
            val_array[i] = cval_array[ci++];
        }
    }

    grib_context_free(a->context, bvals);
    grib_context_free(a->context, cidx_array);
    grib_context_free(a->context, cval_array);

    return GRIB_SUCCESS;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;
    int err                               = 0;
    size_t bmaplen                        = *len;
    long coded_n_vals                     = 0;
    double* coded_vals                    = NULL;
    long i                                = 0;
    long j                                = 0;
    double missing_value                  = 0;
    grib_handle* hand = grib_handle_of_accessor(a);
    grib_context* ctxt = a->context;

    if (*len == 0)
        return GRIB_NO_VALUES;

    if (!grib_find_accessor(hand, self->bitmap)) {
        /*printf("SETTING TOTAL number_of_data_points %s %ld\n",self->number_of_data_points,*len);*/
        if (self->number_of_data_points)
            grib_set_long_internal(hand, self->number_of_data_points, *len);

        err = grib_set_double_array_internal(hand, self->coded_values, val, *len);
        return err;
    }

    if ((err = grib_get_double_internal(hand, self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_set_double_array_internal(hand, self->bitmap, val, bmaplen)) != GRIB_SUCCESS)
        return err;

    coded_n_vals = *len;

    if (coded_n_vals < 1) {
        err = grib_set_double_array_internal(hand, self->coded_values, NULL, 0);
        return err;
    }

    coded_vals = (double*)grib_context_malloc_clear(ctxt, coded_n_vals * sizeof(double));
    if (!coded_vals)
        return GRIB_OUT_OF_MEMORY;

    for (i = 0; i < *len; i++) {
        if (val[i] != missing_value) {
            coded_vals[j++] = val[i];
        }
    }

    err = grib_set_double_array_internal(hand, self->coded_values, coded_vals, j);
    grib_context_free(ctxt, coded_vals);
    if (j == 0) {
        if (self->number_of_values)
            err = grib_set_long_internal(hand, self->number_of_values, 0);
        if (self->binary_scale_factor)
            err = grib_set_long_internal(hand, self->binary_scale_factor, 0);
    }

    return err;
}

template <typename T>
static int unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    grib_accessor_data_apply_bitmap* self = (grib_accessor_data_apply_bitmap*)a;

    size_t i             = 0;
    size_t j             = 0;
    size_t n_vals        = 0;
    long nn              = 0;
    int err              = 0;
    size_t coded_n_vals  = 0;
    T* coded_vals   = NULL;
    double missing_value = 0;

    err    = grib_value_count(a, &nn);
    n_vals = nn;
    if (err)
        return err;

    if (!grib_find_accessor(grib_handle_of_accessor(a), self->bitmap))
        return grib_get_array<T>(grib_handle_of_accessor(a), self->coded_values, val, len);

    if ((err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->missing_value, &missing_value)) != GRIB_SUCCESS)
        return err;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if (coded_n_vals == 0) {
        for (i = 0; i < n_vals; i++)
            val[i] = missing_value;

        *len = n_vals;
        return GRIB_SUCCESS;
    }

    if ((err = grib_get_array_internal<T>(grib_handle_of_accessor(a), self->bitmap, val, &n_vals)) != GRIB_SUCCESS)
        return err;

    coded_vals = (T*)grib_context_malloc(a->context, coded_n_vals * sizeof(T));
    if (coded_vals == NULL)
        return GRIB_OUT_OF_MEMORY;

    if ((err = grib_get_array<T>(grib_handle_of_accessor(a), self->coded_values, coded_vals, &coded_n_vals)) != GRIB_SUCCESS) {
        grib_context_free(a->context, coded_vals);
        return err;
    }

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_class_data_apply_bitmap: %s : creating %s, %d values",
                     __func__,
                     a->name, n_vals);

    for (i = 0; i < n_vals; i++) {
        if (val[i] == 0) {
            val[i] = missing_value;
        }
        else {
            val[i] = coded_vals[j++];
            if (j > coded_n_vals) {
                grib_context_free(a->context, coded_vals);
                grib_context_log(a->context, GRIB_LOG_ERROR,
                                 "grib_accessor_class_data_apply_bitmap [%s]:"
                                 " %s :  number of coded values does not match bitmap %ld %ld",
                                 a->name, __func__, coded_n_vals, n_vals);

                return GRIB_ARRAY_TOO_SMALL;
            }
        }
    }

    *len = n_vals;

    grib_context_free(a->context, coded_vals);
    return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    return unpack<double>(a, val, len);
}

static int unpack_float(grib_accessor* a, float* val, size_t* len)
{
    return unpack<float>(a, val, len);
}

static int get_native_type(grib_accessor* a)
{
    //grib_accessor_data_apply_bitmap* self =  (grib_accessor_data_apply_bitmap*)a;
    //return grib_accessor_get_native_type(grib_find_accessor(grib_handle_of_accessor(a),self->coded_values));

    return GRIB_TYPE_DOUBLE;
}

static int compare(grib_accessor* a, grib_accessor* b)
{
    int retval   = 0;
    double* aval = 0;
    double* bval = 0;

    size_t alen = 0;
    size_t blen = 0;
    int err     = 0;
    long count  = 0;

    err = grib_value_count(a, &count);
    if (err)
        return err;
    alen = count;

    err = grib_value_count(b, &count);
    if (err)
        return err;
    blen = count;

    if (alen != blen)
        return GRIB_COUNT_MISMATCH;

    aval = (double*)grib_context_malloc(a->context, alen * sizeof(double));
    bval = (double*)grib_context_malloc(b->context, blen * sizeof(double));

    grib_unpack_double(a, aval, &alen);
    grib_unpack_double(b, bval, &blen);

    retval = GRIB_SUCCESS;
    while (alen != 0) {
        if (*bval != *aval)
            retval = GRIB_DOUBLE_VALUE_MISMATCH;
        alen--;
    }

    grib_context_free(a->context, aval);
    grib_context_free(b->context, bval);

    return retval;
}

