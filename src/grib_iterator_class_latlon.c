/*
 * Copyright 2005-2013 ECMWF.
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
   CLASS      = iterator
   SUPER      = grib_iterator_class_regular
   IMPLEMENTS = init
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class              (grib_iterator_class*);

static int init               (grib_iterator* i,grib_handle*,grib_arguments*);


typedef struct grib_iterator_latlon{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in regular */
	double   *las;
	double   *los;
	long      nap;
	long      nam;
	long iScansNegatively;
/* Members defined in latlon */
} grib_iterator_latlon;

extern grib_iterator_class* grib_iterator_class_regular;

static grib_iterator_class _grib_iterator_class_latlon = {
    &grib_iterator_class_regular,                    /* super                     */
    "latlon",                    /* name                      */
    sizeof(grib_iterator_latlon),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    0,                  /* destructor                */
    0,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_latlon = &_grib_iterator_class_latlon;


static void init_class(grib_iterator_class* c)
{
	c->next	=	(*(c->super))->next;
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */


static int init(grib_iterator* i,grib_handle* h,grib_arguments* args)
{
  grib_iterator_latlon* self = (grib_iterator_latlon*)i;
  int ret = GRIB_SUCCESS;
  double jdir;
  double laf;
  long jScansPositively;

  long lai;

  const char* latofirst   = grib_arguments_get_name(h,args,self->carg++);
  const char* jdirec      = grib_arguments_get_name(h,args,self->carg++);
  const char* s_jScansPositively   = grib_arguments_get_name(h,args,self->carg++);

  if((ret = grib_get_double_internal(h,latofirst,     &laf))) return ret;
  if((ret = grib_get_double_internal(h,jdirec,        &jdir))) return ret;
  if((ret = grib_get_long_internal(h,s_jScansPositively,&jScansPositively)))
      return ret;

  if (jScansPositively) jdir=-jdir;

  for( lai = 0; lai <  self->nam; lai++ )  {
    self->las[lai] = laf;
    laf -= jdir ;
  }

  i->e = -1;
  return ret;
}
