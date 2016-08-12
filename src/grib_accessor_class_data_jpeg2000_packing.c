/*
 * Copyright 2005-2016 ECMWF.
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
   SUPER      = grib_accessor_class_data_simple_packing
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = unpack_double_element
   IMPLEMENTS = value_count
   MEMBERS=const char*   type_of_compression_used
   MEMBERS=const char*   target_compression_ratio
   MEMBERS=const char*   ni
   MEMBERS=const char*   nj
   MEMBERS=const char*   list_defining_points
   MEMBERS=const char*   number_of_data_points
   MEMBERS=const char*   scanning_mode
   MEMBERS=int jpeg_lib
   MEMBERS=const char*   dump_jpg
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val,size_t *len);
static int unpack_double(grib_accessor*, double* val,size_t *len);
static int value_count(grib_accessor*,long*);
static void init(grib_accessor*,const long, grib_arguments* );
static void init_class(grib_accessor_class*);
static int unpack_double_element(grib_accessor*,size_t i, double* val);

typedef struct grib_accessor_data_jpeg2000_packing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in values */
	int  carg;
	const char* seclen;
	const char* offsetdata;
	const char* offsetsection;
	int dirty;
/* Members defined in data_simple_packing */
	int edition;
	const char*  units_factor;
	const char*  units_bias;
	const char*  changing_precision;
	const char*  number_of_values;
	const char*  bits_per_value;
	const char*  reference_value;
	const char*  binary_scale_factor;
	const char*  decimal_scale_factor;
/* Members defined in data_jpeg2000_packing */
	const char*   type_of_compression_used;
	const char*   target_compression_ratio;
	const char*   ni;
	const char*   nj;
	const char*   list_defining_points;
	const char*   number_of_data_points;
	const char*   scanning_mode;
	int jpeg_lib;
	const char*   dump_jpg;
} grib_accessor_data_jpeg2000_packing;

extern grib_accessor_class* grib_accessor_class_data_simple_packing;

static grib_accessor_class _grib_accessor_class_data_jpeg2000_packing = {
    &grib_accessor_class_data_simple_packing,                      /* super                     */
    "data_jpeg2000_packing",                      /* name                      */
    sizeof(grib_accessor_data_jpeg2000_packing),  /* size                      */
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
    0,                  /* grib_pack procedures long      */
    0,                /* grib_unpack procedures long    */
    &pack_double,                /* grib_pack procedures double    */
    &unpack_double,              /* grib_unpack procedures double  */
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
    &unpack_double_element,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_data_jpeg2000_packing = &_grib_accessor_class_data_jpeg2000_packing;


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
	c->pack_long	=	(*(c->super))->pack_long;
	c->unpack_long	=	(*(c->super))->unpack_long;
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
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static int first = 1;

#define JASPER_LIB 1
#define OPENJPEG_LIB 2

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
    const char * user_lib=NULL;
    grib_accessor_data_jpeg2000_packing *self =(grib_accessor_data_jpeg2000_packing*)a;

    self->jpeg_lib = 0;
    self->type_of_compression_used  = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->target_compression_ratio = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->ni                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->nj                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->list_defining_points   = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->number_of_data_points  = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->scanning_mode      = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->edition=2;
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;

#if HAVE_LIBJASPER
    self->jpeg_lib=JASPER_LIB;
#elif HAVE_LIBOPENJPEG
    self->jpeg_lib=OPENJPEG_LIB;
#endif

    if ((user_lib=codes_getenv("ECCODES_GRIB_JPEG"))!=NULL ) {
        if (!strcmp(user_lib,"jasper")) {
            self->jpeg_lib=JASPER_LIB;
        } else if (!strcmp(user_lib,"openjpeg")) {
            self->jpeg_lib=OPENJPEG_LIB;
        }
    }

    self->dump_jpg = codes_getenv("ECCODES_GRIB_DUMP_JPG_FILE");

    if(first) {
        if(self->dump_jpg)
            printf("GRIB JPEG dumping to %s\n",self->dump_jpg);
        first = 0;
    }
}

static int value_count(grib_accessor* a,long* n_vals)
{
    grib_accessor_data_jpeg2000_packing *self =(grib_accessor_data_jpeg2000_packing*)a;
    *n_vals= 0;

    return grib_get_long_internal(grib_handle_of_accessor(a),self->number_of_values,n_vals);
}

#define EXTRA_BUFFER_SIZE 10240

#if HAVE_JPEG
static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_jpeg2000_packing *self =(grib_accessor_data_jpeg2000_packing*)a;

    int err = GRIB_SUCCESS;
    int i;
    size_t buflen = grib_byte_count(a);

    double bscale = 0;
    double dscale = 0;
    unsigned char* buf = NULL;
    size_t n_vals = 0;
    long nn=0;

    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    double reference_value = 0;
    long bits_per_value =0;
    double units_factor=1.0;
    double units_bias=0.0;

    n_vals = 0;
    err=grib_value_count(a,&nn);
    n_vals=nn;
    if (err) return err;

    if(self->units_factor)
        grib_get_double_internal(grib_handle_of_accessor(a),self->units_factor,&units_factor);

    if(self->units_bias)
        grib_get_double_internal(grib_handle_of_accessor(a),self->units_bias,&units_bias);


    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    self->dirty=0;

    bscale = grib_power(binary_scale_factor,2);
    dscale = grib_power(-decimal_scale_factor,10);

    /* TODO: This should be called upstream */
    if(*len < n_vals)
        return GRIB_ARRAY_TOO_SMALL;

    /* Special case */

    if(bits_per_value == 0)
    {
        for(i = 0; i < n_vals; i++)
            val[i] = reference_value;
        *len = n_vals;
        return GRIB_SUCCESS;
    }

    buf = (unsigned char*)grib_handle_of_accessor(a)->buffer->data;
    buf += grib_byte_offset(a);

    switch (self->jpeg_lib) {
    case OPENJPEG_LIB:
        if ((err = grib_openjpeg_decode(a->context,buf,&buflen,val,&n_vals)) != GRIB_SUCCESS)
            return err;
        break;
    case JASPER_LIB:
        if ((err = grib_jasper_decode(a->context,buf,&buflen,val,&n_vals)) != GRIB_SUCCESS)
            return err;
        break;
    default:
        grib_context_log(a->context,GRIB_LOG_ERROR,"Unable to unpack. Invalid JPEG library.\n");
        return GRIB_DECODING_ERROR;
    }

    *len = n_vals;

    for (i = 0; i < n_vals; i++) {
        val[i] = (val[i] * bscale + reference_value) * dscale;
    }
    if (units_factor != 1.0) {
        if (units_bias != 0.0)
            for (i=0;i<n_vals;i++) val[i]=val[i]*units_factor+units_bias;
        else
            for (i=0;i<n_vals;i++) val[i]*=units_factor;
    } else if (units_bias != 0.0)
        for (i=0;i<n_vals;i++) val[i]+=units_bias;

    return err;
}

static int pack_double(grib_accessor* a, const double* cval, size_t *len)
{
    grib_accessor_data_jpeg2000_packing *self =(grib_accessor_data_jpeg2000_packing*)a;
    grib_accessor_class* super = *(a->cclass->super);
    size_t n_vals = *len;
    int err = 0;
    int i;
    double reference_value = 0;
    long   binary_scale_factor = 0;
    long   bits_per_value = 0;
    long   decimal_scale_factor = 0;
    double decimal = 1;
    size_t simple_packing_size = 0;
    unsigned char*  buf = NULL;
    double divisor = 1;
    long width;
    long height;
    long ni;
    long nj;
    long target_compression_ratio;
    long type_of_compression_used;
    long scanning_mode;
    long list_defining_points;
    long number_of_data_points;
    int ret =0;
    j2k_encode_helper helper;
    double units_factor=1.0;
    double units_bias=0.0;
    double* val=(double*)cval;

    self->dirty=1;

    if(*len == 0){
        grib_buffer_replace(a, NULL, 0, 1, 1);
        return GRIB_SUCCESS;
    }

    if(self->units_factor &&
            (grib_get_double_internal(grib_handle_of_accessor(a),self->units_factor,&units_factor)== GRIB_SUCCESS)) {
        grib_set_double_internal(grib_handle_of_accessor(a),self->units_factor,1.0);
    }

    if(self->units_bias &&
            (grib_get_double_internal(grib_handle_of_accessor(a),self->units_bias,&units_bias)== GRIB_SUCCESS)) {
        grib_set_double_internal(grib_handle_of_accessor(a),self->units_bias,0.0);
    }

    if (units_factor != 1.0) {
        if (units_bias != 0.0)
            for (i=0;i<n_vals;i++) val[i]=val[i]*units_factor+units_bias;
        else
            for (i=0;i<n_vals;i++) val[i]*=units_factor;
    } else if (units_bias != 0.0)
        for (i=0;i<n_vals;i++) val[i]+=units_bias;

    ret = super->pack_double(a,val,len);
    switch (ret) {
    case GRIB_CONSTANT_FIELD:
        grib_buffer_replace(a, NULL, 0,1,1);
        err = grib_set_long_internal(grib_handle_of_accessor(a), self->number_of_values, *len);
        return err;
        break;
    case GRIB_SUCCESS:
        break;
    default:
        grib_context_log(a->context,GRIB_LOG_ERROR,"unable to compute packing parameters\n");
        return ret;
    }

    if((ret = grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value, &reference_value))
            != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->binary_scale_factor, &binary_scale_factor))
            != GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->bits_per_value,&bits_per_value)) !=
            GRIB_SUCCESS)
        return ret;

    if((ret = grib_get_long_internal(grib_handle_of_accessor(a),self->decimal_scale_factor, &decimal_scale_factor))
            != GRIB_SUCCESS)
        return ret;

    decimal = grib_power(decimal_scale_factor,10) ;
    divisor = grib_power(-binary_scale_factor,2);

    simple_packing_size = (((bits_per_value*n_vals)+7)/8)*sizeof(unsigned char);
    buf  = (unsigned char*)grib_context_malloc_clear(a->context,simple_packing_size+EXTRA_BUFFER_SIZE);
    if(!buf) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->ni,&ni)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->nj,&nj)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->type_of_compression_used,&type_of_compression_used)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->target_compression_ratio,&target_compression_ratio)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->scanning_mode,&scanning_mode)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->list_defining_points,&list_defining_points)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->number_of_data_points,&number_of_data_points)) != GRIB_SUCCESS)
        return err;

    width  = ni;
    height = nj;

    if((scanning_mode & (1<<5)) != 0)
    {
        long tmp = width;
        width    = height;
        height   = tmp;
    }

    /* The grid is not regular */
    if(list_defining_points != 0)
    {
        width  = *len;
        height = 1;
    }

    /* There is a bitmap */
    if(*len != number_of_data_points)
    {
        width  = *len;
        height = 1;
    }

    if(width*height != *len)
    {
        /* fprintf(stderr,"width=%ld height=%ld len=%d\n",(long)width,(long)height,(long)*len); */
        Assert(width*height == *len);
    }

    switch( type_of_compression_used)
    {
    case 0:
        Assert(target_compression_ratio == 255);
        helper.compression = 0;
        break;

    case 1:
        Assert(target_compression_ratio != 255);
        Assert(target_compression_ratio != 0);
        helper.compression = target_compression_ratio;
        break;

    default:
        err = GRIB_NOT_IMPLEMENTED;
        goto cleanup;
    }

    helper.jpeg_buffer     = buf;
    helper.width           = width;
    helper.height          = height;

    /* See GRIB-438 */
    if (bits_per_value == 0) {
        const long bits_per_value_adjusted = 1;
        grib_context_log(a->context, GRIB_LOG_DEBUG,
                "grib_accessor_class_data_jpeg2000_packing(%s) : bits per value was zero, changed to %d",
                self->jpeg_lib==OPENJPEG_LIB ? "openjpeg" : "jasper", bits_per_value_adjusted);
        bits_per_value = bits_per_value_adjusted;
    }
    helper.bits_per_value  = bits_per_value;

    helper.buffer_size     = simple_packing_size + EXTRA_BUFFER_SIZE;
    helper.values          = val;
    helper.no_values       = n_vals;
    helper.reference_value = reference_value;
    helper.divisor         = divisor;
    helper.decimal         = decimal;
    helper.jpeg_length     = 0;

    switch (self->jpeg_lib) {
    case OPENJPEG_LIB:
        if ( (err = grib_openjpeg_encode(a->context,&helper)) != GRIB_SUCCESS ) goto cleanup;
        break;
    case JASPER_LIB:
        if ( (err = grib_jasper_encode(a->context,&helper)) != GRIB_SUCCESS ) goto cleanup;
        break;
    }

    if(helper.jpeg_length > simple_packing_size)
        grib_context_log(a->context, GRIB_LOG_WARNING,
                "grib_accessor_data_jpeg2000_packing(%s) : jpeg data (%ld) larger than input data (%ld)",
                self->jpeg_lib==OPENJPEG_LIB ? "openjpeg" : "jasper",
                        helper.jpeg_length, simple_packing_size);

    Assert( helper.jpeg_length <= helper.buffer_size);

    if(self->dump_jpg) {
        FILE *f = fopen(self->dump_jpg,"w");
        if(f) {
            if(fwrite(helper.jpeg_buffer,helper.jpeg_length,1,f) != 1)
                perror(self->dump_jpg);
            if (fclose(f) != 0)
                perror(self->dump_jpg);
        }
        else perror(self->dump_jpg);
    }

    grib_buffer_replace(a, helper.jpeg_buffer, helper.jpeg_length, 1, 1);

    cleanup:

    grib_context_free(a->context,buf);

    if(err == GRIB_SUCCESS)
        err = grib_set_long_internal(grib_handle_of_accessor(a),self->number_of_values, *len);
    return err;

}
#else

static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "JPEG support not enabled.");
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR, "JPEG support not enabled.");
    return GRIB_NOT_IMPLEMENTED;
}

#endif

static int unpack_double_element(grib_accessor* a, size_t idx, double* val)
{
    size_t size;
    double* values;
    int err=0;

    /* GRIB-564: The index idx relates to codedValues NOT values! */

    err=grib_get_size(grib_handle_of_accessor(a),"codedValues",&size);
    if (err) return err;
    if (idx > size) return GRIB_INVALID_NEAREST;

    values=(double*)grib_context_malloc_clear(a->context,size*sizeof(double));
    err=grib_get_double_array(grib_handle_of_accessor(a),"codedValues",values,&size);
    if (err) return err;
    *val=values[idx];
    grib_context_free(a->context,values);
    return err;
}
