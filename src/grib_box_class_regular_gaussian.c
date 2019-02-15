/*
 * Copyright 2005-2018 ECMWF.
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
   CLASS      = box
   SUPER      = grib_box_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = get_points
   IMPLEMENTS = init
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "box.class" and rerun ./make_class.pl

*/


static void init_class              (grib_box_class*);

static int init               (grib_box* box,grib_handle* h,grib_arguments* args);
static grib_points* get_points(grib_box* box, double north, double west, double south,double east,int *err);
static int destroy            (grib_box* box);

typedef struct grib_box_regular_gaussian{
  grib_box box;
/* Members defined in gen */
/* Members defined in regular_gaussian */
} grib_box_regular_gaussian;

extern grib_box_class* grib_box_class_gen;

static grib_box_class _grib_box_class_regular_gaussian = {
    &grib_box_class_gen,                         /* super                     */
    "regular_gaussian",                         /* name                      */
    sizeof(grib_box_regular_gaussian),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &get_points,                    /* get points           */
};

grib_box_class* grib_box_class_regular_gaussian = &_grib_box_class_regular_gaussian;


static void init_class(grib_box_class* c)
{
}
/* END_CLASS_IMP */

static int init(grib_box* box,grib_handle* h,grib_arguments* args)
{
  return 0;
}

static int destroy(grib_box* box) {
  return GRIB_SUCCESS;
}

static grib_points* get_points(grib_box* box, double north, double west, double south,double east, int *err) {
	*err=GRIB_NOT_IMPLEMENTED;
	return NULL;
}



