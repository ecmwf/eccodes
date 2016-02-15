/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  Enrico Fucile
 **************************************/


#include "grib_api_internal.h"

/* 
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = expression
   IMPLEMENTS = init_class
   IMPLEMENTS = destroy
   IMPLEMENTS = native_type
   IMPLEMENTS = evaluate;print
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


typedef struct grib_expression_constant{
  grib_expression base;
/* Members defined in constant */
	long value;
} grib_expression_constant;


static grib_expression_class _grib_expression_class_constant = {
    0,                    /* super                     */
    "constant",                    /* name                      */
    sizeof(grib_expression_constant),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    0,                     /* constructor               */
    &destroy,                  /* destructor                */
    &print,                 
    0,                 
    &add_dependency,       

	&native_type,
	0,

	0,
	0,
	0,
};

grib_expression_class* grib_expression_class_constant = &_grib_expression_class_constant;


static void init_class(grib_expression_class* c)
{
}
/* END_CLASS_IMP */

static long evaluate(grib_expression* g,grib_handle *h)
{
	grib_expression_constant* e = (grib_expression_constant*)g;
	return e->value;
}

static void print(grib_context* c,grib_expression* g,grib_handle* f)
{
	grib_expression_constant* e = (grib_expression_constant*)g;
	printf("constant(%ld)",e->value);
}

static void destroy(grib_context* c,grib_expression* g)
{
	/* grib_expression_constant* e = (grib_expression_constant*)g; */
}


static void  add_dependency(grib_expression* g, grib_accessor* observer){
	/* grib_expression_constant* e = (grib_expression_constant*)g; */
}


