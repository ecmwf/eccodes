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
#define PNG_ANYBITS

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_values
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*   number_of_values
   MEMBERS=const char*   reference_value
   MEMBERS=const char*   binary_scale_factor
   MEMBERS=const char*   decimal_scale_factor
   MEMBERS=const char*   bits_per_value
   MEMBERS=const char*   ni
   MEMBERS=const char*   nj
   MEMBERS=const char*   list_defining_points
   MEMBERS=const char*   number_of_data_points
   MEMBERS=const char*   scanning_mode

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

typedef struct grib_accessor_data_png_packing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in values */
	int  carg;
	const char* seclen;
	const char* offsetdata;
	const char* offsetsection;
	int dirty;
/* Members defined in data_png_packing */
	const char*   number_of_values;
	const char*   reference_value;
	const char*   binary_scale_factor;
	const char*   decimal_scale_factor;
	const char*   bits_per_value;
	const char*   ni;
	const char*   nj;
	const char*   list_defining_points;
	const char*   number_of_data_points;
	const char*   scanning_mode;
} grib_accessor_data_png_packing;

extern grib_accessor_class* grib_accessor_class_values;

static grib_accessor_class _grib_accessor_class_data_png_packing = {
    &grib_accessor_class_values,                      /* super                     */
    "data_png_packing",                      /* name                      */
    sizeof(grib_accessor_data_png_packing),  /* size                      */
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
    0,     /* unpack only ith value          */
    0,     /* unpack a subarray         */
    0,              		/* clear          */
    0,               		/* clone accessor          */
};


grib_accessor_class* grib_accessor_class_data_png_packing = &_grib_accessor_class_data_png_packing;


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
	c->unpack_double_element	=	(*(c->super))->unpack_double_element;
	c->unpack_double_subarray	=	(*(c->super))->unpack_double_subarray;
	c->clear	=	(*(c->super))->clear;
	c->make_clone	=	(*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a,const long v, grib_arguments* args)
{
    grib_accessor_data_png_packing *self =(grib_accessor_data_png_packing*)a;

    self->number_of_values      = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->reference_value       = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->binary_scale_factor   = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->decimal_scale_factor  = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->bits_per_value           = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);

    self->ni                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->nj                     = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);

    self->list_defining_points   = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->number_of_data_points  = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    self->scanning_mode      = grib_arguments_get_name(grib_handle_of_accessor(a),args,self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a,long* n_vals)
{
    grib_accessor_data_png_packing *self =(grib_accessor_data_png_packing*)a;
    *n_vals= 0;
    return grib_get_long_internal(grib_handle_of_accessor(a),self->number_of_values,n_vals);
}

#if HAVE_LIBPNG

#include "png.h"

typedef struct png_read_callback_data {
    unsigned char* buffer;
    size_t         length;
    size_t         offset;
} png_read_callback_data;

static void png_read_callback(png_structp png,png_bytep data, png_size_t length)
{
    png_read_callback_data* p = (png_read_callback_data*)png_get_io_ptr(png);
    Assert(p->offset + length <= p->length);
    memcpy(data,p->buffer+p->offset,length);
    p->offset += length;
}

static void png_write_callback(png_structp png,png_bytep data, png_size_t length)
{
    png_read_callback_data* p = (png_read_callback_data*)png_get_io_ptr(png);
    Assert(p->offset + length <= p->length);
    memcpy(p->buffer+p->offset,data,length);
    p->offset += length;
}

static void png_flush_callback(png_structp png)
{
    /* Empty */
}


static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_png_packing *self =(grib_accessor_data_png_packing*)a;

    int err = GRIB_SUCCESS;
    int i,j;
    size_t buflen = grib_byte_count(a);

    double bscale = 0;
    double dscale = 0;
    unsigned char* buf = NULL;
    size_t n_vals = 0;

    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    double reference_value = 0;
    long bits_per_value =0;
    long bits8;
    long nn=0;

    png_structp png  = 0;
    png_infop info = 0,theEnd = 0;
    png_bytepp rows = 0;
    int interlace = 0,colour = 0,compression = 0,filter = 0,depth = 0;

    png_uint_32 width = 0,height = 0;

    png_read_callback_data callback_data;

    self->dirty=0;

    n_vals = 0;
    err=grib_value_count(a,&nn);
    n_vals=nn;
    if (err) return err;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

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


    if ( png_sig_cmp(buf,0,8) != 0)
        return GRIB_INVALID_MESSAGE;


    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
            NULL, NULL);
    if (!png)
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    info = png_create_info_struct(png);
    if (!info)
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    theEnd = png_create_info_struct(png);
    if (!theEnd)
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    callback_data.buffer = buf;
    callback_data.offset = 0;
    callback_data.length = buflen;

    /* printf("buflen=%d\n",buflen); */

    png_set_read_fn(png,&callback_data,png_read_callback);
    png_read_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    Assert(callback_data.offset == callback_data.length);


    rows = png_get_rows(png, info);

    png_get_IHDR(png, info,
            &width, &height,
            &depth, &colour,
            &interlace,
            &compression,
            &filter);


    if (colour == PNG_COLOR_TYPE_RGB)      depth=24;
    if (colour == PNG_COLOR_TYPE_RGB_ALPHA) depth=32;
    bits8 = ((bits_per_value+7)/8)*8;

#ifdef PNG_ANYBITS
    Assert(depth == bits8);
#else
    Assert( bits_per_value % 8 == 0 );
#endif

    i = 0;

    /* printf("bscale=%g dscale=%g reference_value=%g\n",bscale,dscale,reference_value); */

    for(j=0; j<height;j++)
    {
        png_byte* row = rows[j];
        long pos = 0;
        int k;
        for(k = 0 ; k < width; k++)
            val[i++] = (double) (((grib_decode_unsigned_long(row, &pos, bits8)*bscale)+reference_value)*dscale);
    }
    /*-------------------------------------------*/
    *len = n_vals;


    cleanup:
    if(png)
        png_destroy_read_struct(&png, info?&info:NULL, theEnd?&theEnd:NULL);
    return err;

}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_png_packing *self =(grib_accessor_data_png_packing*)a;

    int err = GRIB_SUCCESS;
    int i,j;
    size_t buflen = grib_byte_count(a);

    unsigned char* buf = NULL;
    unsigned char* encoded = NULL;
    size_t n_vals = 0;

    long binary_scale_factor = 0;
    long decimal_scale_factor = 0;
    double reference_value = 0;
    long bits8;
    long bits_per_value =0;
    double max,min;
    long bytes;
    double d;

    png_structp png  = 0;
    png_infop info = 0;
    png_bytepp rows = 0;
    int colour = 0,depth = 0;

    png_uint_32 width = 0,height = 0;
    unsigned char *p;
    double divisor;

    png_read_callback_data callback_data;
    long ni,nj;
    long scanning_mode;
    long list_defining_points;
    long number_of_data_points;
    long nn=0;


    self->dirty=1;

    n_vals = 0;
    err=grib_value_count(a,&nn);
    n_vals=nn;
    if (err) return err;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value, &reference_value)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->binary_scale_factor, &binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->decimal_scale_factor, &decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    /* Special case */

    if(*len == 0) {
        grib_buffer_replace(a, NULL, 0,1,1);
        return GRIB_SUCCESS;
    }

    if(bits_per_value == 0)
    {
        int i;
        /* constant field */
        for(i = 1 ; i < n_vals; i++)
            Assert(val[i] == val[0]);

        if((err = grib_set_double_internal(grib_handle_of_accessor(a),self->reference_value, val[0])) != GRIB_SUCCESS)
            return err;
        {
            /* Make sure we can decode it again */
            double ref = 1e-100;
            grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value,&ref);
            printf("%g %g %g\n",reference_value,ref,reference_value-ref);
            Assert(ref == reference_value);
        }

        if((err = grib_set_long_internal(grib_handle_of_accessor(a),self->number_of_values, n_vals)) !=  GRIB_SUCCESS)
            return err;

        grib_buffer_replace(a, NULL, 0,1,1);

        return GRIB_SUCCESS;
    }

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->ni,&ni)) != GRIB_SUCCESS)
        return err;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->nj,&nj)) != GRIB_SUCCESS)
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
        fprintf(stderr,"width=%ld height=%ld len=%ld\n", (long)width, (long)height, (long)(*len) );
        Assert(width*height == *len);
    }


    d = grib_power(decimal_scale_factor,10) ;


    max = val[0];
    min = max;
    for(i=1;i< n_vals;i++)
    {
        if (val[i] > max )
            max = val[i];
        if (val[i] < min )
            min = val[i];
    }
    min *= d;
    max *= d;

    if (grib_get_nearest_smaller_value(grib_handle_of_accessor(a),self->reference_value,min,&reference_value)
            !=GRIB_SUCCESS) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "unable to find nearest_smaller_value of %g for %s",min,self->reference_value);
        return GRIB_INTERNAL_ERROR;
    }

    if(reference_value > min)
    {
        fprintf(stderr,"reference_value=%g min_value=%g diff=%g\n",reference_value,min,reference_value-min);
        Assert(reference_value <= min);
    }

    binary_scale_factor = grib_get_binary_scale_fact(max,reference_value,bits_per_value,&err);
    divisor = grib_power(-binary_scale_factor,2);

#ifndef PNG_ANYBITS
    Assert(bits_per_value % 8 == 0);
#endif
    bits8 = (bits_per_value+7)/8*8;
    encoded = grib_context_buffer_malloc_clear(a->context,bits8/8*n_vals);

    if(!encoded) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }

    buflen = 0;
    p = encoded;
    for(i=0;i< n_vals;i++){
        long blen = bits8;
        unsigned long unsigned_val = (unsigned long)((((val[i]*d)-(reference_value))*divisor)+0.5);
        while(blen >= 8)
        {
            blen   -= 8;
            *p = (unsigned_val >> blen);
            p++;
            buflen++;
        }
    }
    /*       buflen = n_vals*(bits_per_value/8);*/

    grib_context_log(a->context, GRIB_LOG_DEBUG,
            "grib_accessor_data_png_packing : pack_double : packing %s, %d values", a->name, n_vals);
    buf = grib_context_buffer_malloc_clear(a->context,buflen);

    if(!buf) {
        err = GRIB_OUT_OF_MEMORY;
        goto cleanup;
    }


    if((err = grib_set_double_internal(grib_handle_of_accessor(a),self->reference_value, reference_value)) != GRIB_SUCCESS)
        return err;
    {
        /* Make sure we can decode it again */
        double ref = 1e-100;
        grib_get_double_internal(grib_handle_of_accessor(a),self->reference_value,&ref);
        Assert(ref == reference_value);
    }
    if((err = grib_set_long_internal(grib_handle_of_accessor(a),self->binary_scale_factor, binary_scale_factor)) != GRIB_SUCCESS)
        return err;
    if((err = grib_set_long_internal(grib_handle_of_accessor(a),self->decimal_scale_factor, decimal_scale_factor)) != GRIB_SUCCESS)
        return err;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->ni,&ni)) != GRIB_SUCCESS)
        return err;

    if((err = grib_get_long_internal(grib_handle_of_accessor(a),self->nj,&nj)) != GRIB_SUCCESS)
        return err;

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    info = png_create_info_struct(png);
    if (!info)
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        err = GRIB_DECODING_ERROR;
        goto cleanup;
    }


    callback_data.buffer = buf;
    callback_data.offset = 0;
    callback_data.length = buflen;

    /* printf("buflen=%d\n",buflen); */


    png_set_write_fn(png,&callback_data,png_write_callback,png_flush_callback);

    depth = bits8;

    colour = PNG_COLOR_TYPE_GRAY;
    if (bits8 == 24)
    {
        depth=8;
        colour=PNG_COLOR_TYPE_RGB;
    }

    if(bits8 == 32 )
    {
        depth=8;
        colour=PNG_COLOR_TYPE_RGB_ALPHA;
    }

    png_set_IHDR(png, info, width, height,
            depth, colour, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


    /*bytes=bit_depth/8;*/
    bytes = bits8/8;

    rows = grib_context_buffer_malloc_clear(a->context,sizeof(png_bytep)*height);
    /*rows  = malloc(height*sizeof(png_bytep));*/
    Assert(rows);
    for (j=0;j<height;j++)
        rows[j] = &encoded[j*width*bytes];

    png_set_rows(png, info,rows);

    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

    Assert(callback_data.offset <= callback_data.length);


    grib_buffer_replace(a, buf, callback_data.offset,1,1);

    cleanup:
    if(png)
        png_destroy_write_struct(&png, info?&info:NULL);


    grib_context_buffer_free(a->context,buf);
    grib_context_buffer_free(a->context,encoded);
    grib_context_buffer_free(a->context,rows);

    if(err == GRIB_SUCCESS)
        err = grib_set_long_internal(grib_handle_of_accessor(a),self->number_of_values, *len);

    return err;
}
#else

static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR,
            "grib_accessor_data_png_packing: PNG support not enabled.");
    return GRIB_NOT_IMPLEMENTED;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_context_log(a->context, GRIB_LOG_ERROR,
            "grib_accessor_data_png_packing: PNG support not enabled.");
    return GRIB_NOT_IMPLEMENTED;
}

#endif
