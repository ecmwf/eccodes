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
#include <math.h>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = init;next
   MEMBERS     =   double *las
   MEMBERS     =   double *los
   MEMBERS     =   long nam
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
static int next               (grib_iterator* i, double *lat, double *lon, double *val);
static int destroy            (grib_iterator* i);


typedef struct grib_iterator_gaussian_reduced{
  grib_iterator it;
/* Members defined in gen */
	long carg;
	const char* missingValue;
/* Members defined in gaussian_reduced */
	double *las;
	double *los;
	long nam;
} grib_iterator_gaussian_reduced;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_gaussian_reduced = {
    &grib_iterator_class_gen,                    /* super                     */
    "gaussian_reduced",                    /* name                      */
    sizeof(grib_iterator_gaussian_reduced),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_gaussian_reduced = &_grib_iterator_class_gaussian_reduced;


static void init_class(grib_iterator_class* c)
{
	c->previous	=	(*(c->super))->previous;
	c->reset	=	(*(c->super))->reset;
	c->has_next	=	(*(c->super))->has_next;
}
/* END_CLASS_IMP */

static int next(grib_iterator* i, double *lat, double *lon, double *val)
{

  grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)i;

  if((long)i->e >= (long)(i->nv-1))
    return 0;
  i->e++;

  *lat = self->las[i->e];
  *lon = self->los[i->e];
  *val = i->data[i->e];

  return 1;
}


static int init(grib_iterator* iter,grib_handle* h,grib_arguments* args)
{
  int ret=GRIB_SUCCESS,j;
  double lat_first=0,lon_first=0,lat_last=0,lon_last=0,d=0;
  double* lats;
  size_t plsize=0;
  int l=0;
  long* pl;
  long nj=0,order=0,ilon_first,ilon_last,i;
  long row_count=0;
  grib_context* c=h->context;
  grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)iter;
  const char* slat_first   = grib_arguments_get_name(h,args,self->carg++);
  const char* slon_first   = grib_arguments_get_name(h,args,self->carg++);
  const char* slat_last    = grib_arguments_get_name(h,args,self->carg++);
  const char* slon_last    = grib_arguments_get_name(h,args,self->carg++);
  const char* sorder       = grib_arguments_get_name(h,args,self->carg++);
  const char* spl          = grib_arguments_get_name(h,args,self->carg++);
  const char* snj          = grib_arguments_get_name(h,args,self->carg++);

  if((ret = grib_get_double_internal(h, slat_first,&lat_first)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_double_internal(h, slon_first,&lon_first)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_double_internal(h, slat_last,&lat_last)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_double_internal(h, slon_last,&lon_last)) != GRIB_SUCCESS)
    return ret;

  if((ret = grib_get_long_internal(h, sorder,&order)) != GRIB_SUCCESS)
    return ret;
  if((ret = grib_get_long_internal(h, snj,&nj)) != GRIB_SUCCESS)
    return ret;

  lats=(double*)grib_context_malloc(h->context,sizeof(double)*order*2);
  if((ret = grib_get_gaussian_latitudes(order, lats)) != GRIB_SUCCESS)
      return ret;

  if((ret = grib_get_size(h,spl,&plsize)) != GRIB_SUCCESS)
      return ret;

  pl=(long*)grib_context_malloc(c,sizeof(long)*plsize);
  grib_get_long_array_internal(h,spl,pl, &plsize);

  self->las = grib_context_malloc(h->context,iter->nv*sizeof(double));
  self->los = grib_context_malloc(h->context,iter->nv*sizeof(double));

  while (lon_last<0) lon_last+=360;
  while (lon_first<0) lon_first+=360;

  d=fabs(lats[0]-lats[1]);
  if ( (fabs(lat_first-lats[0]) >= d ) ||
       (fabs(lat_last+lats[0]) >= d )  ||
       lon_first != 0                 ||
       fabs(lon_last  - (360.0-90.0/order)) > 90.0/order
       ) {
    /*sub area*/
	/*find starting latitude */
	while (fabs(lat_first-lats[l]) > d ) {l++;}
    iter->e=0;
    for (j=0;j<plsize;j++) {
        row_count=0;
		/*printf("lat=%g\n",lats[j+l]);*/
		grib_get_reduced_row(pl[j],lon_first,lon_last,
                  &row_count,&ilon_first,&ilon_last);
        if (ilon_first>ilon_last) ilon_first-=pl[j];
        for (i=ilon_first;i<=ilon_last;i++) {
         self->los[iter->e]=((i)*360.0)/pl[j];
         self->las[iter->e]=lats[j+l];
         iter->e++;
        }
     }
  } else {
    /*global*/
    iter->e=0;
    for (j=0;j<plsize;j++) {
        row_count=pl[j];
        for (i=0;i<row_count;i++) {
         self->los[iter->e]=(i*360.0)/row_count;
         self->las[iter->e]=lats[j];
         iter->e++;
        }
     }
  }

  iter->e = -1;
  grib_context_free(h->context,lats);
  grib_context_free(h->context,pl);

  return ret;
}

static int destroy(grib_iterator* i)
{
  grib_iterator_gaussian_reduced* self = (grib_iterator_gaussian_reduced*)i;
  const grib_context *c = i->h->context;

  grib_context_free(c,self->las);
  grib_context_free(c,self->los);
  return 1;
}

