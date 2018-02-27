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

bufr_descriptor* grib_bufr_descriptor_new(grib_accessor* tables_accessor,int code,int *err)
{
    bufr_descriptor* ret=accessor_bufr_elements_table_get_descriptor(tables_accessor,code,err);
    if (*err) grib_context_log(tables_accessor->context,GRIB_LOG_ERROR,
            "unable to get descriptor %06d from table",code);
    return ret;
}

bufr_descriptor* grib_bufr_descriptor_clone(bufr_descriptor* d)
{

    bufr_descriptor* cd;

    if (!d) return NULL;

    cd=(bufr_descriptor*)grib_context_malloc_clear(d->context,sizeof(bufr_descriptor));

    cd->code=d->code;
    cd->F=d->F;
    cd->X=d->X;
    cd->Y=d->Y;
    /* cd->name=grib_context_strdup(d->context,d->name); See ECC-489 */
    cd->shortName=grib_context_strdup(d->context,d->shortName);
    cd->units=grib_context_strdup(d->context,d->units);
    cd->scale=d->scale;
    cd->factor=d->factor;
    cd->width=d->width;
    cd->reference=d->reference;
    cd->type=d->type;
    cd->nokey=d->nokey;

    return cd;
}

int grib_bufr_descriptor_set_code(grib_accessor* tables_accessor,int code,bufr_descriptor* v)
{
    int err=0;
    grib_context* c;
    bufr_descriptor* d;

    if (!v) return GRIB_NULL_POINTER;

    c=v->context;

    if (v->type==BUFR_DESCRIPTOR_TYPE_REPLICATION || v->type==BUFR_DESCRIPTOR_TYPE_OPERATOR) {
        v->code=code;
        v->F=code/100000;
        v->X=(code-v->F*100000)/1000;
        v->Y=(code-v->F*100000)%1000;
    } else {
        if (tables_accessor==NULL) return GRIB_NULL_POINTER;
        d=accessor_bufr_elements_table_get_descriptor(tables_accessor,code,&err);
        v->code=d->code;
        v->F=d->F;
        v->X=d->X;
        v->Y=d->Y;
        /* grib_context_free(c,v->name); See ECC-489 */
        /* v->name=grib_context_strdup(c,d->name); See ECC-489 */
        grib_context_free(c,v->shortName);
        v->shortName=grib_context_strdup(c,d->shortName);
        grib_context_free(c,v->units);
        v->units=grib_context_strdup(c,d->units);
        v->scale=d->scale;
        v->factor=d->factor;
        v->width=d->width;
        v->reference=d->reference;
        v->type=d->type;
        v->nokey=d->nokey;
        grib_bufr_descriptor_delete(d);
    }
    return err;
}

void grib_bufr_descriptor_set_reference(bufr_descriptor* v,long reference)
{
    if (!v) return;
    v->reference=reference;
}

void grib_bufr_descriptor_set_width(bufr_descriptor* v,long width)
{
    if (!v) return;
    v->width=width;
}

void grib_bufr_descriptor_set_scale(bufr_descriptor* v,long scale)
{
    if (!v) return;
    v->scale=scale;
    if (scale!=0) v->type=BUFR_DESCRIPTOR_TYPE_DOUBLE;
    v->factor=grib_power(-scale,10);
}

int grib_bufr_descriptor_can_be_missing(bufr_descriptor* v)
{
    if (v->code==31031 || v->code==999999 ) return 0;
    if (v->width == 1) return 0;
    return 1;
}

void grib_bufr_descriptor_delete(bufr_descriptor* v)
{
    grib_context* c=NULL;
    if (!v) return;
    c=v->context;

    /* grib_context_free(c,v->name); See ECC-489 */
    grib_context_free(c,v->shortName);
    grib_context_free(c,v->units);
    grib_context_free(c,v);
}
