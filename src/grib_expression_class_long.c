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
   CLASS      = expression
   IMPLEMENTS = native_type;pack_missing
   IMPLEMENTS = destroy
   IMPLEMENTS = evaluate_long
   IMPLEMENTS = evaluate_double
   IMPLEMENTS = print
   IMPLEMENTS = add_dependency
   MEMBERS    = long value
   END_CLASS_DEF

 */
/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "expression.class" and rerun ./make_class.pl

*/

typedef const char* string; /* to keep make_class.pl happy */


static void init_class              (grib_expression_class*);

static void        destroy(grib_context*,grib_expression* e);

static void        print(grib_context*,grib_expression*,grib_handle*);
static void        add_dependency(grib_expression* e, grib_accessor* observer);

static int        native_type(grib_expression*,grib_handle*);

static int        evaluate_long(grib_expression*,grib_handle*,long*);
static int      evaluate_double(grib_expression*,grib_handle*,double*);

typedef struct grib_expression_long{
  grib_expression base;
    /* Members defined in long */
    long value;
} grib_expression_long;


static grib_expression_class _grib_expression_class_long = {
    0,                    /* super                     */
    "long",                    /* name                      */
    sizeof(grib_expression_long),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    &add_dependency,       

	&native_type,
	0,

	&evaluate_long,
	&evaluate_double,
	0,
};

grib_expression_class* grib_expression_class_long = &_grib_expression_class_long;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static int evaluate_long(grib_expression* g, grib_handle* h, long* lres)
{
    grib_expression_long* e = (grib_expression_long*)g;
    *lres                   = e->value;
    return GRIB_SUCCESS;
}

static int evaluate_double(grib_expression* g, grib_handle* h, double* dres)
{
    grib_expression_long* e = (grib_expression_long*)g;
    *dres                   = e->value;
    return GRIB_SUCCESS;
}

static void print(grib_context* c, grib_expression* g, grib_handle* f)
{
    grib_expression_long* e = (grib_expression_long*)g;
    printf("long(%ld)", e->value);
}

static void destroy(grib_context* c, grib_expression* g)
{
    /* grib_expression_long* e = (grib_expression_long*)g; */
}

static void add_dependency(grib_expression* g, grib_accessor* observer)
{
    /* grib_expression_long* e = (grib_expression_long*)g; */
}

grib_expression* new_long_expression(grib_context* c, long value)
{
    grib_expression_long* e = (grib_expression_long*)grib_context_malloc_clear_persistent(c, sizeof(grib_expression_long));
    e->base.cclass          = grib_expression_class_long;
    e->value                = value;
    return (grib_expression*)e;
}

static int native_type(grib_expression* g, grib_handle* h)
{
    return GRIB_TYPE_LONG;
}
