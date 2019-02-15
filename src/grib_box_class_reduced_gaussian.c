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
   MEMBERS = double* lats
   MEMBERS = size_t nlats
   MEMBERS = double** lons
   MEMBERS = long* nlons
   MEMBERS = size_t size

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

typedef struct grib_box_reduced_gaussian{
  grib_box box;
/* Members defined in gen */
/* Members defined in reduced_gaussian */
	double* lats;
	size_t nlats;
	double** lons;
	long* nlons;
	size_t size;
} grib_box_reduced_gaussian;

extern grib_box_class* grib_box_class_gen;

static grib_box_class _grib_box_class_reduced_gaussian = {
    &grib_box_class_gen,                         /* super                     */
    "reduced_gaussian",                         /* name                      */
    sizeof(grib_box_reduced_gaussian),      /* size of instance          */
    0,                              /* inited */
    &init_class,                    /* init_class */
    &init,                          /* constructor               */
    &destroy,                       /* destructor                */
    &get_points,                    /* get points           */
};

grib_box_class* grib_box_class_reduced_gaussian = &_grib_box_class_reduced_gaussian;


static void init_class(grib_box_class* c)
{
}
/* END_CLASS_IMP */

static int init(grib_box* box,grib_handle* h,grib_arguments* args)
{
	grib_box_reduced_gaussian* self=(grib_box_reduced_gaussian*)box;
	int n=1;
	int ret=0;
	char* key;
	long* pl;
	double lat_first,lat_last,lon_first,lon_last;
	long order;
	double *lats;
	long j,l,ilon_first,ilon_last,i;
	long row_count;
	grib_context* c=box->context;

	key = (char*)grib_arguments_get_name(h,args,n++);
	ret=grib_get_double(h,key,&lat_first);  
	if (ret) return ret;

	key = (char*)grib_arguments_get_name(h,args,n++);
	ret=grib_get_double(h,key,&lon_first);  
	if (ret) return ret;

	key = (char*)grib_arguments_get_name(h,args,n++);
	ret=grib_get_double(h,key,&lat_last);  
	if (ret) return ret;

	key = (char*)grib_arguments_get_name(h,args,n++);
	ret=grib_get_double(h,key,&lon_last);  
	if (ret) return ret;

	key = (char*)grib_arguments_get_name(h,args,n++);
	ret=grib_get_long(h,key,&order);  
	if (ret) return ret;

	key = (char*)grib_arguments_get_name(h,args,n++);
	ret=grib_get_size(h,key,&self->nlats);
	if (ret) return ret;
	pl=(long*)grib_context_malloc(h->context,self->nlats*sizeof(long));
	ret=grib_get_long_array(h,key,pl,&self->nlats);  
	if (ret) return ret;

	lats=(double*)grib_context_malloc(h->context,sizeof(double)*order*2);
	if((ret = grib_get_gaussian_latitudes(order, lats)) != GRIB_SUCCESS)
	        return ret;

	if (self->nlats==order*2) {
		/*global (latitudes)*/
		self->lats=lats;
	} else {
		/*sub area (latitudes)*/
		int ll=0;
		double d=fabs(lats[0]-lats[1]);
		while (fabs(lat_first-lats[ll]) > d ) {ll++;}
		self->lats=(double*)grib_context_malloc(h->context,sizeof(double)*self->nlats);
		for (i=0;i<self->nlats;i++) self->lats[i]=lats[ll++];
		grib_context_free(box->context,lats);
	}

	self->lons=(double**)grib_context_malloc_clear(box->context,sizeof(double*)*self->nlats);
	self->size=0;
	if (lon_first != 0                 ||
			fabs(lon_last  - (360.0-90.0/order)) > 90.0/order) {
		/* sub area (longitudes) */
		for (j=0;j<self->nlats;j++) {
			row_count=0;
			grib_get_reduced_row_wrapper(h, pl[j],lon_first,lon_last,
                  &row_count,&ilon_first,&ilon_last);
			self->size+=row_count;
			if (ilon_first>ilon_last) ilon_first-=pl[j];
			l=0;
			self->lons[j]=(double*)grib_context_malloc_clear(c,sizeof(double)*row_count);
			for (i=ilon_first;i<=ilon_last;i++) 
				self->lons[j][l++]=((i)*360.0)/pl[j];
			pl[j]=row_count;
		}
	} else {
		/* global (longitudes) */
		for (j=0;j<self->nlats;j++) {
			self->lons[j]=(double*)grib_context_malloc_clear(c,sizeof(double)*pl[j]);
			self->size+=pl[j];
			for (i=0;i<pl[j];i++) 
				self->lons[j][i]=(i*360.0)/pl[j];
		}

	}
	self->nlons=pl;

	return 0;
}

static int destroy(grib_box* box) {
	grib_box_reduced_gaussian* self=(grib_box_reduced_gaussian*)box;
	grib_context* c=box->context;
	int i;

	grib_points_delete(box->points);

	grib_context_free(c,self->lats);
	for (i=0;i<self->nlats;i++) 
		grib_context_free(c,self->lons[i]);
	grib_context_free(c,self->lons);
	
  return GRIB_SUCCESS;
}

static grib_points* get_points(grib_box* box, double north, double west, double south,double east, int *err) {
	long j,i,index,l;
	double lat,lon;
	size_t group_count;
	int first=1;
	grib_box_reduced_gaussian* self=(grib_box_reduced_gaussian*)box;
	grib_points* points=NULL;

	grib_context* c=box->context;
	points=grib_points_new(c,self->size);
	if (!points) 
		grib_context_log(c,GRIB_LOG_FATAL,"unable to create grib_points\n");

	index=0;
	l=0;
	group_count=0;
	for (j=0;j<self->nlats;j++) {
		lat=self->lats[j];
		for (i=0;i<self->nlons[j];i++)  {
			lon=self->lons[j][i];
			if (lat<north && lat>south && lon>west && lon<east) {
				points->latitudes[l]=lat;
				points->longitudes[l]=lon;
				points->indexes[l]=index;
				points->group_len[group_count]++;
				if (first) {
					points->group_start[group_count]=index;
					points->group_len[group_count]=0;
					first=0;
				} else {
					if (index!=points->group_start[group_count]+points->group_len[group_count]) {
						group_count++;
						points->group_start[group_count]=index;
						points->group_len[group_count]=0;
						points->n_groups=group_count+1;
					} 
				}
				l++;
			}
			index++;
		}
	}
	points->group_len[group_count]++;
	points->n=l;
	if (box->points) grib_points_delete(box->points);
	box->points=points;

	*err=0;

	return points;
}



