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

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_shsimple_packing
   IMPLEMENTS = unpack_double
   IMPLEMENTS = value_count
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int unpack_double(grib_accessor*, double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_g1shsimple_packing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in data_shsimple_packing */
    const char*  coded_values;
    const char*  real_part;
    int dirty;
    /* Members defined in data_g1shsimple_packing */
} grib_accessor_data_g1shsimple_packing;

extern grib_accessor_class* grib_accessor_class_data_shsimple_packing;

static grib_accessor_class _grib_accessor_class_data_g1shsimple_packing = {
    &grib_accessor_class_data_shsimple_packing,                      /* super */
    "data_g1shsimple_packing",                      /* name */
    sizeof(grib_accessor_data_g1shsimple_packing),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    0,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    0,                  /* grib_pack procedures long */
    0,                /* grib_unpack procedures long */
    0,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    0,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_g1shsimple_packing = &_grib_accessor_class_data_g1shsimple_packing;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->get_native_type    =    (*(c->super))->get_native_type;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_long    =    (*(c->super))->pack_long;
    c->unpack_long    =    (*(c->super))->unpack_long;
    c->pack_double    =    (*(c->super))->pack_double;
    c->pack_string    =    (*(c->super))->pack_string;
    c->unpack_string    =    (*(c->super))->unpack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */


static int value_count(grib_accessor* a, long* count)
{
    grib_accessor_data_g1shsimple_packing* self = (grib_accessor_data_g1shsimple_packing*)a;
    size_t len                                  = 0;
    int err                                     = 0;

    err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &len);
    len += 1;

    *count = len;
    return err;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_data_g1shsimple_packing* self = (grib_accessor_data_g1shsimple_packing*)a;
    int err                                     = GRIB_SUCCESS;

    size_t coded_n_vals = 0;
    size_t n_vals       = 0;

    if ((err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    n_vals = coded_n_vals + 1;

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->real_part, val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_get_double_array_internal(grib_handle_of_accessor(a), self->coded_values, val, &coded_n_vals)) != GRIB_SUCCESS)
        return err;

    grib_context_log(a->context, GRIB_LOG_DEBUG,
                     "grib_accessor_data_g1shsimple_packing_bitmap : unpack_double : creating %s, %d values",
                     a->name, n_vals);

    *len = n_vals;

    return err;
}
