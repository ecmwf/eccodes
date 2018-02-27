/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**************************************
 *  This pertains to GRIB edition 2
 **************************************/

#include "grib_api_internal.h"
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_unsigned
   IMPLEMENTS = unpack_long;pack_long; value_count
   IMPLEMENTS = init
   MEMBERS=const char*    productDefinitionTemplateNumber
   MEMBERS=const char*    productDefinitionTemplateNumberInternal
   MEMBERS=const char* grib2LocalSectionNumber
   MEMBERS=const char* type
   MEMBERS=const char* stream
   MEMBERS=const char* the_class
   MEMBERS=const char* eps
   MEMBERS=const char* stepType
   MEMBERS=const char* derivedForecast
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_long(grib_accessor*, const long* val,size_t *len);
static int unpack_long(grib_accessor*, long* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_local_definition {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in long */
/* Members defined in unsigned */
	long nbytes;
	grib_arguments* arg;
/* Members defined in local_definition */
	const char*    productDefinitionTemplateNumber;
	const char*    productDefinitionTemplateNumberInternal;
	const char* grib2LocalSectionNumber;
	const char* type;
	const char* stream;
	const char* the_class;
	const char* eps;
	const char* stepType;
	const char* derivedForecast;
} grib_accessor_local_definition;

extern grib_accessor_class* grib_accessor_class_unsigned;

static grib_accessor_class _grib_accessor_class_local_definition = {
    &grib_accessor_class_unsigned,                      /* super                     */
    "local_definition",                      /* name                      */
    sizeof(grib_accessor_local_definition),  /* size                      */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init                      */
    0,                  /* post_init                      */
    0,                    /* free mem                       */
    0,                       /* describes himself         */
    0,                /* get length of section     */
    0,              /* get length of string      */
    &value_count,                /* get number of values      */
    0,                 /* get number of bytes      */
    0,                /* get offset to bytes           */
    0,            /* get native type               */
    0,                /* get sub_section                */
    0,               /* grib_pack procedures long      */
    0,                 /* grib_pack procedures long      */
    &pack_long,                  /* grib_pack procedures long      */
    &unpack_long,                /* grib_unpack procedures long    */
    0,                /* grib_pack procedures double    */
    0,              /* grib_unpack procedures double  */
    0,                /* grib_pack procedures string    */
    0,              /* grib_unpack procedures string  */
    0,          /* grib_pack array procedures string    */
    0,        /* grib_unpack array procedures string  */
    0,                 /* grib_pack procedures bytes     */
    0,               /* grib_unpack procedures bytes   */
    0,            /* pack_expression */
    0,              /* notify_change   */
    0,                /* update_size   */
    0,            /* preferred_size   */
    0,                    /* resize   */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor    */
    0,                    /* compare vs. another accessor   */
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_local_definition = &_grib_accessor_class_local_definition;


static void init_class(grib_accessor_class* c)
{
	c->dump	=	(*(c->super))->dump;
	c->next_offset	=	(*(c->super))->next_offset;
	c->string_length	=	(*(c->super))->string_length;
	c->byte_count	=	(*(c->super))->byte_count;
	c->byte_offset	=	(*(c->super))->byte_offset;
	c->get_native_type	=	(*(c->super))->get_native_type;
	c->sub_section	=	(*(c->super))->sub_section;
	c->pack_missing	=	(*(c->super))->pack_missing;
	c->is_missing	=	(*(c->super))->is_missing;
	c->pack_double	=	(*(c->super))->pack_double;
	c->unpack_double	=	(*(c->super))->unpack_double;
	c->pack_string	=	(*(c->super))->pack_string;
	c->unpack_string	=	(*(c->super))->unpack_string;
	c->pack_string_array	=	(*(c->super))->pack_string_array;
	c->unpack_string_array	=	(*(c->super))->unpack_string_array;
	c->pack_bytes	=	(*(c->super))->pack_bytes;
	c->unpack_bytes	=	(*(c->super))->unpack_bytes;
	c->pack_expression	=	(*(c->super))->pack_expression;
	c->notify_change	=	(*(c->super))->notify_change;
	c->update_size	=	(*(c->super))->update_size;
	c->preferred_size	=	(*(c->super))->preferred_size;
	c->resize	=	(*(c->super))->resize;
	c->nearest_smaller_value	=	(*(c->super))->nearest_smaller_value;
	c->next	=	(*(c->super))->next;
	c->compare	=	(*(c->super))->compare;
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long l, grib_arguments* c)
{
    grib_accessor_local_definition* self = (grib_accessor_local_definition*)a;
    int n = 0;

    self->grib2LocalSectionNumber = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->productDefinitionTemplateNumber = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->productDefinitionTemplateNumberInternal = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->type = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->stream = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->the_class = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->eps = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->stepType = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
    self->derivedForecast = grib_arguments_get_name(grib_handle_of_accessor(a),c,n++);
}

static int unpack_long(grib_accessor* a, long* val, size_t *len)
{
    grib_accessor_local_definition* self = (grib_accessor_local_definition*)a;

    return grib_get_long(grib_handle_of_accessor(a), self->grib2LocalSectionNumber,val);
}

static int pack_long(grib_accessor* a, const long* val, size_t *len)
{
    grib_accessor_local_definition* self = (grib_accessor_local_definition*)a;
    long productDefinitionTemplateNumber=-1;
    long productDefinitionTemplateNumberInternal=-1;
    long productDefinitionTemplateNumberNew=-1;
    long grib2LocalSectionNumber=-1;
    long type=-1;
    long stream=-1;
    long the_class=-1;
    long eps=-1;
    char stepType[15]={0,};
    size_t slen=15;
    int localDefinitionNumber=*val;
    int isInstant=0;
    int tooEarly=0;
    long derivedForecast=-1;
    long editionNumber = 0;

    if (grib_get_long(grib_handle_of_accessor(a), "editionNumber", &editionNumber)==GRIB_SUCCESS) {
        Assert(editionNumber == 2);
    }

    if (grib_get_long(grib_handle_of_accessor(a), self->productDefinitionTemplateNumber,&productDefinitionTemplateNumber)!=GRIB_SUCCESS)
        tooEarly=1;
    grib_get_long(grib_handle_of_accessor(a), self->productDefinitionTemplateNumberInternal,&productDefinitionTemplateNumberInternal);
    grib_get_long(grib_handle_of_accessor(a), self->type,&type);
    grib_get_long(grib_handle_of_accessor(a), self->stream,&stream);
    grib_get_long(grib_handle_of_accessor(a), self->the_class,&the_class);
    grib_get_long(grib_handle_of_accessor(a), self->eps,&eps);
    grib_get_string(grib_handle_of_accessor(a), self->stepType,stepType,&slen);
    if (!strcmp(stepType,"instant")) isInstant=1;
    grib_get_long(grib_handle_of_accessor(a), self->grib2LocalSectionNumber,&grib2LocalSectionNumber);

    if (is_productDefinitionTemplateNumber_EPS(productDefinitionTemplateNumber))
        eps=1;
/*TODO chemicals*/
    switch (localDefinitionNumber) {
    case 0:
    case 300:
        productDefinitionTemplateNumberNew=productDefinitionTemplateNumber;
        break;

    case 500:
        productDefinitionTemplateNumberNew=0;
        break;

    case 1:  /* MARS labelling */
    case 36: /* MARS labelling for long window 4Dvar system */
    case 40: /* MARS labeling with domain and model (for LAM) */
    case 42: /* LC-WFV: Wave forecast verification */
        if (isInstant) {
            /* type=em || type=es  */
            if (type==17) {
                productDefinitionTemplateNumberNew=2;
                derivedForecast=0;
            } else if (type==18) {
                productDefinitionTemplateNumberNew=2;
                derivedForecast=4;
                /* eps or enda or elda or ewla */
            } else  if (eps==1 || stream==1030 || stream==1249 || stream==1250) {
                productDefinitionTemplateNumberNew=1;
            } else {
                productDefinitionTemplateNumberNew=0;
            }
        } else {
            /* type=em || type=es */
            if (type==17) {
                productDefinitionTemplateNumberNew=12;
                derivedForecast=0;
            } else if (type==18) {
                productDefinitionTemplateNumberNew=12;
                derivedForecast=4;
                /* eps or enda or elda or ewla */
            } else  if (eps==1 || stream==1030 || stream==1249 || stream==1250) {
                productDefinitionTemplateNumberNew=11;
            } else {
                productDefinitionTemplateNumberNew=8;
            }
        }
        break;
    case 41: /* EFAS: uses post-processing templates */
        if (isInstant) {
            if (eps==1) productDefinitionTemplateNumberNew=71;
            else        productDefinitionTemplateNumberNew=70;
        } else {
            /* non-instantaneous: accum etc */
            if (eps==1) productDefinitionTemplateNumberNew=73;
            else        productDefinitionTemplateNumberNew=72;
        }
        break;

    case 15: /* Seasonal forecast data */
    case 16: /* Seasonal forecast monthly mean data */
    case 18: /* Multianalysis ensemble data */
    case 26: /* MARS labelling or ensemble forecast data */
    case 30: /* Forecasting Systems with Variable Resolution */
        if (isInstant) {
            productDefinitionTemplateNumberNew=1;
        } else {
            productDefinitionTemplateNumberNew=11;
        }
        break;

    case 5: /* Forecast probability data */
    case 7:  /* Sensitivity data */
    case 9:  /* Singular vectors and ensemble perturbations */
    case 11: /* Supplementary data used by the analysis */
    case 14: /* Brightness temperature */
    case 20: /* 4D variational increments */
    case 21: /* Sensitive area predictions */
    case 23: /* Coupled atmospheric, wave and ocean means */
    case 24: /* Satellite Channel Number Data */
    case 25:
    case 28: /* COSMO local area EPS */
    case 38: /* 4D variational increments for long window 4Dvar system */
    case 39: /* 4DVar model errors for long window 4Dvar system */
    case 192: /* Multiple ECMWF local definitions */
        if (isInstant) {
            productDefinitionTemplateNumberNew=0;
        } else {
            productDefinitionTemplateNumberNew=8;
        }
        break;

    default:
        grib_context_log(a->context,GRIB_LOG_ERROR,"Invalid localDefinitionNumber %d",localDefinitionNumber);
        return GRIB_ENCODING_ERROR;
        break;
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        if (tooEarly)
            grib_set_long(grib_handle_of_accessor(a), self->productDefinitionTemplateNumberInternal,productDefinitionTemplateNumberNew);
        else
            grib_set_long(grib_handle_of_accessor(a), self->productDefinitionTemplateNumber,productDefinitionTemplateNumberNew);
    }
    if (derivedForecast>=0)
        grib_set_long(grib_handle_of_accessor(a), self->derivedForecast,derivedForecast);

    grib_set_long(grib_handle_of_accessor(a), self->grib2LocalSectionNumber,*val);

  return 0;
}

static int value_count(grib_accessor* a,long* count)
{
    *count=1;
    return 0;
}

