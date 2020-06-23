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
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = init;unpack_string; get_native_type
   MEMBERS    = const char* stepUnits
   MEMBERS    = const char* step
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_string(grib_accessor*, char*, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_step_human_readable
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in step_human_readable */
    const char* stepUnits;
    const char* step;
} grib_accessor_step_human_readable;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_step_human_readable = {
    &grib_accessor_class_gen,                      /* super                     */
    "step_human_readable",                      /* name                      */
    sizeof(grib_accessor_step_human_readable),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    0,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    &get_native_type,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    &unpack_string,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    0,                /* update_size   */
    0,             /* preferred_size   */
    0,                     /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,      /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,                      /* clear          */
    0,                 /* clone accessor          */
};


grib_accessor_class* grib_accessor_class_step_human_readable = &_grib_accessor_class_step_human_readable;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->value_count    =    (*(c->super))->value_count;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_long    =    (*(c->super))->pack_long;
    c->unpack_long    =    (*(c->super))->unpack_long;
    c->pack_double    =    (*(c->super))->pack_double;
    c->unpack_double    =    (*(c->super))->unpack_double;
    c->pack_string    =    (*(c->super))->pack_string;
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
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* params)
{
    grib_accessor_step_human_readable* self = (grib_accessor_step_human_readable*)a;
    int n                              = 0;
    grib_handle* h = grib_handle_of_accessor(a);

    self->stepUnits                    = grib_arguments_get_name(h, params, n++);
    self->step                         = grib_arguments_get_name(h, params, n++);
    a->length                          = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

static int get_step_human_readable(grib_handle* h, char* result, size_t* length)
{
    int err = 0;
    size_t slen = 2;
    long step, hour, minute, second;

    /* Change units to seconds (highest resolution)
     * before computing the step value
     */
    err = grib_set_string(h, "stepUnits", "s", &slen);
    if (err) return err;
    err = grib_get_long(h, "step", &step);
    if (err) return err;

    hour = step/3600;
    minute = step/60 % 60;
    second = step % 60;
    /* sprintf(result, "%ld:%ld:%ld", hour, minute, second); */

    if (second) {
        sprintf(result, "%ldh %ldm %lds", hour, minute, second);
    } else {
        if (minute) sprintf(result, "%ldh %ldm", hour, minute);
        else sprintf(result, "%ldh", hour);
    }

    *length = strlen(result);
    return GRIB_SUCCESS;
}

static int unpack_string(grib_accessor* a, char* buffer, size_t* len)
{
    grib_accessor_step_human_readable* self = (grib_accessor_step_human_readable*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    long stepUnits;
    int err = 0;

    /* Save the current value of stepUnits */
    err = grib_get_long_internal(h, self->stepUnits, &stepUnits);
    if (err) return err;

    /* This will change stepUnits to seconds for its calculation */
    err = get_step_human_readable(h, buffer, len);

    /* Restore stepUnits */
    grib_set_long(h, self->stepUnits, stepUnits);
    return err;
}
