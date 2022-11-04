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
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = unpack_long;pack_long; value_count
   IMPLEMENTS = init
   MEMBERS=const char* productDefinitionTemplateNumber
   MEMBERS=const char* stepType
   MEMBERS=int chemical_type
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_g2_chemical
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in unsigned */
    long nbytes;
    grib_arguments* arg;
    /* Members defined in g2_chemical */
    const char* productDefinitionTemplateNumber;
    const char* stepType;
    int chemical_type;
} grib_accessor_g2_chemical;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_g2_chemical = {
    &grib_accessor_class_unsigned,                      /* super */
    "g2_chemical",                      /* name */
    sizeof(grib_accessor_g2_chemical),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
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
    &pack_long,                  /* grib_pack procedures long */
    &unpack_long,                /* grib_unpack procedures long */
    0,                /* grib_pack procedures double */
    0,              /* grib_unpack procedures double */
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


grib_accessor_class* grib_accessor_class_g2_chemical = &_grib_accessor_class_g2_chemical;


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
    c->pack_double    =    (*(c->super))->pack_double;
    c->unpack_double    =    (*(c->super))->unpack_double;
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

static void init(grib_accessor* a, const long l, grib_arguments* c)
{
    grib_accessor_g2_chemical* self = (grib_accessor_g2_chemical*)a;
    grib_handle* hand               = grib_handle_of_accessor(a);
    int n                           = 0;

    self->productDefinitionTemplateNumber = grib_arguments_get_name(hand, c, n++);
    self->stepType                        = grib_arguments_get_name(hand, c, n++);
    self->chemical_type                    = grib_arguments_get_long(hand, c, n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_g2_chemical* self      = (grib_accessor_g2_chemical*)a;
    long productDefinitionTemplateNumber = 0;
    grib_get_long(grib_handle_of_accessor(a), self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber);

    /*
     * 0 = atmospheric chemical constituents
     * 1 = atmospheric chemical constituents based on a distribution function
     * 2 = atmospheric chemical constituents with source or sink
     */
    Assert(self->chemical_type == 0 || self->chemical_type == 1 || self->chemical_type == 2);
    if (self->chemical_type == 1)
        *val = grib2_is_PDTN_ChemicalDistFunc(productDefinitionTemplateNumber);
    else if (self->chemical_type == 2)
        *val = grib2_is_PDTN_ChemicalSourceSink(productDefinitionTemplateNumber);
    else
        *val = grib2_is_PDTN_Chemical(productDefinitionTemplateNumber);

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_g2_chemical* self         = (grib_accessor_g2_chemical*)a;
    grib_handle* hand                       = grib_handle_of_accessor(a);
    long productDefinitionTemplateNumber    = -1;
    long productDefinitionTemplateNumberNew = -1;
    /*long type=-1;
    long stream=-1;*/
    long eps          = -1;
    char stepType[15] = {0,};
    size_t slen = 15;
    /*int chemical = *val;*/
    int isInstant = 0;
    /*long derivedForecast=-1;*/
    int ret = 0;

    if (grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber) != GRIB_SUCCESS)
        return GRIB_SUCCESS;

    /*
     grib_get_long(hand, self->type,&type);
     grib_get_long(hand, self->stream,&stream);
     */
    ret = grib_get_string(hand, self->stepType, stepType, &slen);
    Assert(ret == GRIB_SUCCESS);

    eps = grib2_is_PDTN_EPS(productDefinitionTemplateNumber);

    if (!strcmp(stepType, "instant"))
        isInstant = 1;

    /*
     * 0 = atmospheric chemical constituents
     * 1 = atmospheric chemical constituents based on a distribution function
     * 2 = atmospheric chemical constituents with source or sink
     */
    Assert(self->chemical_type == 0 || self->chemical_type == 1 || self->chemical_type == 2);

    if (eps == 1) {
        if (isInstant) {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=41;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=58;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=77;
        }
        else {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=43;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=68;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=79;
        }
    }
    else {
        /* deterministic */
        if (isInstant) {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=40;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=57;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=76;
        }
        else {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=42;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=67;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=78;
        }
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        grib_set_long(hand, self->productDefinitionTemplateNumber, productDefinitionTemplateNumberNew);
        /*if (derivedForecast>=0) grib_set_long(hand, self->derivedForecast,derivedForecast);*/
    }

    return 0;
}

static int value_count(grib_accessor* a, long* count)
{
    *count = 1;
    return 0;
}
