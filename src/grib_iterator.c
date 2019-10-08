/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Enrico Fucile
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void init_mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void init_mutex()
{
    GRIB_OMP_CRITICAL(lock_iterator_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif


int grib_get_data(const grib_handle* h,double* lats, double* lons,double* values)
{
    int err=0;
    grib_iterator* iter=NULL;
    double *lat,*lon,*val;

    iter=grib_iterator_new(h,0,&err);
    if (!iter || err!=GRIB_SUCCESS) return err;

    lat=lats; lon=lons; val=values;
    while(grib_iterator_next(iter,lat++,lon++,val++)) {}

    grib_iterator_delete( iter);

    return err;
}

int grib_iterator_next(grib_iterator *i,double* lat,double* lon,double* value)
{
    grib_iterator_class *c = i->cclass;
    while(c)
    {
        grib_iterator_class *s = c->super ? *(c->super) : NULL;
        if(c->next) return  c->next(i, lat, lon, value);
        c = s;
    }
    Assert(0);
    return 0;
}

int grib_iterator_has_next(grib_iterator *i)
{
    grib_iterator_class *c = i->cclass;
    while(c)
    {
        grib_iterator_class *s = c->super ? *(c->super) : NULL;
        if(c->has_next) return  c->has_next(i);
        c = s;
    }
    Assert(0);
    return 0;
}

int grib_iterator_previous(grib_iterator *i,double* lat,double* lon,double* value)
{
    grib_iterator_class *c = i->cclass;
    while(c)
    {
        grib_iterator_class *s = c->super ? *(c->super) : NULL;
        if(c->previous) return  c->previous(i, lat, lon, value);
        c = s;
    }
    Assert(0);
    return 0;
}

int grib_iterator_reset(grib_iterator *i)
{
    grib_iterator_class *c = i->cclass;
    while(c)
    {
        grib_iterator_class *s = c->super ? *(c->super) : NULL;
        if(c->reset)  return  c->reset(i);
        c = s;
    }
    Assert(0);
    return 0;
}

/* For this one, ALL init are called */
static int init_iterator(grib_iterator_class* c,grib_iterator* i, grib_handle *h, grib_arguments* args)
{
    if(c) {
        int ret = GRIB_SUCCESS;
        grib_iterator_class *s = c->super ? *(c->super) : NULL;
        if(!c->inited)
        {
            if(c->init_class) c->init_class(c);
            c->inited = 1;
        }
        if(s) ret = init_iterator(s,i,h,args);

        if(ret != GRIB_SUCCESS) return ret;

        if(c->init) return c->init(i,h, args);
    }
    return GRIB_INTERNAL_ERROR;
}

int grib_iterator_init(grib_iterator* i, grib_handle *h, grib_arguments* args)
{
    int r = 0;
    GRIB_MUTEX_INIT_ONCE(&once,&init_mutex);
    GRIB_MUTEX_LOCK(&mutex);
    r = init_iterator(i->cclass,i,h,args);
    GRIB_MUTEX_UNLOCK(&mutex);
    return r;
}

/* For this one, ALL destroy are called */
int grib_iterator_delete(grib_iterator *i)
{
    if (i) {
        grib_iterator_class *c = i->cclass;
        while(c)
        {
            grib_iterator_class *s = c->super ? *(c->super) : NULL;
            if(c->destroy) c->destroy(i);
            c = s;
        }
        /* This should go in a top class */
        grib_context_free(i->h->context,i);
    } else {
        return GRIB_INVALID_ARGUMENT;
    }
    return 0;
}
