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
   CLASS      = accessor
   SUPER      = grib_accessor_class_values
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*  bits_per_value
   MEMBERS=const char*  reference_value
   MEMBERS=const char*  binary_scale_factor
   MEMBERS=const char*  decimal_scale_factor
   MEMBERS=const char*  half_byte
   MEMBERS=const char*  n1
   MEMBERS=const char*  n2
   MEMBERS=const char*  extraValues
   MEMBERS=const char*  p1
   MEMBERS=const char*  p2
   MEMBERS=const char*  matrix_values
   MEMBERS=const char*  snd_bitmap
   MEMBERS=const char*  snd_ordr_wdiff
   MEMBERS=const char*  general_ext
   MEMBERS=const char*  boustrophedonic
   MEMBERS=const char*  two_ordr_spd
   MEMBERS=const char*  plus1_spd
   MEMBERS=const char*  width_widths
   MEMBERS=const char*  width_lengths
   MEMBERS=const char*  octet_start_group
   MEMBERS=const char*  width_spd_sp_desc
   MEMBERS=const char*  nap
   MEMBERS=const char*  bitmap

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

typedef struct grib_accessor_data_2order_packing {
    grib_accessor          att;
/* Members defined in gen */
/* Members defined in values */
	int  carg;
	const char* seclen;
	const char* offsetdata;
	const char* offsetsection;
	int dirty;
/* Members defined in data_2order_packing */
	const char*  bits_per_value;
	const char*  reference_value;
	const char*  binary_scale_factor;
	const char*  decimal_scale_factor;
	const char*  half_byte;
	const char*  n1;
	const char*  n2;
	const char*  extraValues;
	const char*  p1;
	const char*  p2;
	const char*  matrix_values;
	const char*  snd_bitmap;
	const char*  snd_ordr_wdiff;
	const char*  general_ext;
	const char*  boustrophedonic;
	const char*  two_ordr_spd;
	const char*  plus1_spd;
	const char*  width_widths;
	const char*  width_lengths;
	const char*  octet_start_group;
	const char*  width_spd_sp_desc;
	const char*  nap;
	const char*  bitmap;
} grib_accessor_data_2order_packing;

extern grib_accessor_class* grib_accessor_class_values;

static grib_accessor_class _grib_accessor_class_data_2order_packing = {
    &grib_accessor_class_values,                      /* super                     */
    "data_2order_packing",                      /* name                      */
    sizeof(grib_accessor_data_2order_packing),  /* size                      */
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


grib_accessor_class* grib_accessor_class_data_2order_packing = &_grib_accessor_class_data_2order_packing;


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
    grib_handle* gh = grib_handle_of_accessor(a);
    grib_accessor_data_2order_packing *self =(grib_accessor_data_2order_packing*)a;

    self->bits_per_value  = grib_arguments_get_name(gh,args,self->carg++);

    self->reference_value = grib_arguments_get_name(gh,args,self->carg++);

    self->binary_scale_factor = grib_arguments_get_name(gh,args,self->carg++);

    self->decimal_scale_factor = grib_arguments_get_name(gh,args,self->carg++);

    self->half_byte = grib_arguments_get_name(gh,args,self->carg++);

    self->n1 = grib_arguments_get_name(gh,args,self->carg++);
    self->n2 = grib_arguments_get_name(gh,args,self->carg++);
    self->p1 = grib_arguments_get_name(gh,args,self->carg++);
    self->extraValues = grib_arguments_get_name(gh,args,self->carg++);
    self->p2 = grib_arguments_get_name(gh,args,self->carg++);

    self->matrix_values = grib_arguments_get_name(gh ,args,self->carg++);
    self->snd_bitmap = grib_arguments_get_name(gh ,args,self->carg++);
    self->snd_ordr_wdiff = grib_arguments_get_name(gh ,args,self->carg++);

    self->general_ext    = grib_arguments_get_name(gh ,args,self->carg++);
    self->boustrophedonic  = grib_arguments_get_name(gh ,args,self->carg++);
    self->two_ordr_spd          = grib_arguments_get_name(gh ,args,self->carg++);
    self->plus1_spd    = grib_arguments_get_name(gh ,args,self->carg++);

    self->width_widths = grib_arguments_get_name(gh ,args,self->carg++);
    self->width_lengths = grib_arguments_get_name(gh ,args,self->carg++);
    self->octet_start_group = grib_arguments_get_name(gh ,args,self->carg++);
    self->width_spd_sp_desc = grib_arguments_get_name(gh ,args,self->carg++);
    self->nap = grib_arguments_get_name(gh ,args,self->carg++);
    self->bitmap = grib_arguments_get_name(gh, args,self->carg++);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;

}

static const unsigned char BitsSetTable256[256] = 
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
        B6(0), B6(1), B6(1), B6(2)
};

static const unsigned char left_mask[8]={0,0x80,0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };
static const unsigned char right_mask[8]={0,0x80,0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

static unsigned char* bitmap_pop_line(unsigned char* bitmap,long* bitmap_len,int *bit_offset,
        long points_in_line,int *values_in_line)
{
    unsigned char* p=bitmap;
    int m=0,bits=0;
    int bytes=0;

    *values_in_line=0;

    if (*bit_offset) {
        bits=8 - *bit_offset;
        m=*(p++) & right_mask[bits];
        *values_in_line += BitsSetTable256[m];
        points_in_line-=bits;
        (*bitmap_len)--;
        *bit_offset=0;
    }

    bytes=points_in_line/8;

    while (bytes) {
        m=*(p++) & 0xff;
        *values_in_line += BitsSetTable256[m];
        (*bitmap_len)--;
        bytes--;
    }

    *bit_offset=points_in_line % 8;
    m=*p & left_mask[*bit_offset];
    *values_in_line += BitsSetTable256[m];

    return p;
}

static int reverse_rows (unsigned long* data, long  len, long number_along_parallel,
        unsigned char* bitmap,long bitmap_len)
{
    long i = 0;
    long left = 0;
    long right = number_along_parallel-1;
    long tmp = 0;
    long inc;

    if (bitmap_len==0) {
        /* NO BITMAP*/
        long count = 0;
        inc=number_along_parallel;
        count = number_along_parallel;

        while(count < len){
            left = 0;
            right = number_along_parallel-1;
            data += inc;

            for (i = 0;i<number_along_parallel/2;i++){
                tmp = data[left];
                data[left] = data[right];
                data[right] = tmp;

                right--;
                left++;
                Assert(left<len);
                Assert(right>0);
            }

            inc=number_along_parallel*2;
            count += inc;
        }
    } else {
        /*BITMAP present number of point per line not constant*/
        int line_len=0,bit_offset=0;
        long bitmap_left_len=bitmap_len;
        unsigned char* p=bitmap;
        long count=0;

        p=bitmap_pop_line(p,&bitmap_left_len,&bit_offset,number_along_parallel,&line_len);
        while(bitmap_left_len>0 && count>=len){
            count+=line_len;
            data += line_len;

            p=bitmap_pop_line(p,&bitmap_left_len,&bit_offset,number_along_parallel,&line_len);
            left = 0;
            right = line_len-1;

            for (i = 0;i<line_len/2;i++){
                tmp = data[left];
                data[left] = data[right];
                data[right] = tmp;

                right--;
                left++;
                Assert(left<len);
                Assert(right>0);
            }

            count+=line_len;
            Assert(count<len);

            data+=line_len;
            p=bitmap_pop_line(p,&bitmap_left_len,&bit_offset,number_along_parallel,&line_len);
        }
    }

    return 0;
}

static int spatial_difference (grib_context *c, unsigned long* vals, long  len, long order, long* bias)
{
    long j = 3;
    long *v = (long*)grib_context_malloc(c,(len)*sizeof(long));

    for(j = 0; j< len;j++)
        v[j] = vals[j];

    Assert(order == 2);

    *bias=v[order];

    for(j = order; j< len;j++){
        v[j]  -=  vals [j-1];
        v[j]  -=  vals [j-1] - vals[j-2];
        if(*bias>v[j])
            *bias = v[j];
    }

    for(j = order; j< len;j++){
        Assert(v[j]-*bias >=0);
        vals[j] = v[j]-*bias;
    }

    grib_context_free(c,v);
    return 0;
}

static int de_spatial_difference (grib_context *c, unsigned long* vals, long  len, long order, long bias)
{
    long j = 0;
    long i_origin = 0;
    long i_first_diff = 0;
    long i_second_diff = 0;
    if(order == 0)return 0;
    Assert(order > 0);
    Assert(order <= 3);

    i_origin=vals[order-1];

    if (order == 1) {
        for(j = 1; j< len;j++){
            i_origin=i_origin+(vals[j]+bias);
            vals[j]=i_origin;
        }
    }

    if (order == 2){
        i_first_diff=vals[1]-vals[0];
        for(j = 2; j< len;j++){
            i_first_diff=i_first_diff+(vals[j]+bias);
            i_origin=i_origin+i_first_diff;
            vals[j]=i_origin;
        }
    }

    if (order == 3){
        i_first_diff=vals[2]-vals[1];
        i_second_diff=i_first_diff-(vals[1]-vals[0]);
        for(j = 3; j< len;j++){
            i_second_diff=i_second_diff+(vals[j]+bias);
            i_first_diff=i_first_diff+i_second_diff;
            i_origin=i_origin+i_first_diff;
            /*             if(j<10) fprintf(stdout,"> %d = %d\n",j,i_origin);
             */             vals[j]=i_origin;

        }
    }
    return 0;
}

#define KEEP_OLD 0

#if KEEP_OLD == 1
second_order_packed* sd = NULL;
#endif

static int  unpack_double(grib_accessor* a, double* val, size_t *len)
{
    grib_accessor_data_2order_packing* self =  (grib_accessor_data_2order_packing*)a;

    size_t i = 0;
    size_t j = 0;
    size_t n_vals = 0;
    int err=0;

    long     vcount = 0, bias = 0;
    double   reference_value;
    long     binary_scale_factor;
    long     bits_per_value, decimal_scale_factor;
    long     n1 = 0, n2 = 0, extraValues = 0, p1 = 0, p2 = 0;

    long     offsetsection = 0, snd_bitmap = 0, snd_ordr_wdiff = 0;

    long  matrix_values    =0;
    long  general_ext    =0;
    long  boustrophedonic =0;
    long  two_ordr_spd =0;
    long  plus1_spd  =0;

    long  nbits_per_width =0;
    long  nbits_per_group_size =0;
    long  octet_start_group =0;
    long  width_spd_sp_desc =0;
    grib_handle* gh = grib_handle_of_accessor(a);

    unsigned char* buf_size_of_groups = (unsigned char*)gh->buffer->data;
    unsigned char* buf_width_of_group = (unsigned char*)gh->buffer->data;
    unsigned char* bufrefs = (unsigned char*)gh->buffer->data;
    unsigned char* bufvals = (unsigned char*)gh->buffer->data;

    double s = 0;
    double d = 0;

    double max = 0;
    double min = 0;

    unsigned long*  sec_val    = NULL;
    long  ref_vals    = 0;
    short  n_sp_diff = 0;

    short f_size_of_group = 0;
    short f_width_of_group = 0;

    long bitp = 0;
    long pointer_of_group_size = 0;
    long pointer_of_group_width = 0;
    long refsp = 0;
    long nap = 0;
    long nn=0;
    unsigned char* bitmap=NULL;
    grib_accessor* abitmap=NULL;
    size_t bitmap_len=0;

    err=grib_value_count(a,&nn);
    n_vals=nn;

    if((err = grib_get_long_internal(gh,self->offsetsection,&offsetsection))
            != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->bits_per_value,&bits_per_value))
            != GRIB_SUCCESS) return err;
    if((err = grib_get_double_internal(gh,self->reference_value, &reference_value))
            != GRIB_SUCCESS)return err;
    if((err = grib_get_long_internal(gh,self->binary_scale_factor, &binary_scale_factor))
            != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->decimal_scale_factor, &decimal_scale_factor))
            != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->n1,&n1))  != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->n2, &n2)) != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->p1, &p1)) != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->extraValues, &extraValues)) != GRIB_SUCCESS) return err;

    p1=p1+65536*extraValues;

    if((err = grib_get_long_internal(gh,self->p2, &p2)) != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->matrix_values, &matrix_values))
            != GRIB_SUCCESS)return err;
    if((err = grib_get_long_internal(gh,self->snd_bitmap, &snd_bitmap))
            != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->snd_ordr_wdiff, &snd_ordr_wdiff))
            != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->general_ext, &general_ext))
            != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->boustrophedonic, &boustrophedonic))
            != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->two_ordr_spd, &two_ordr_spd))
            != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->plus1_spd, &plus1_spd))
            != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->width_widths, &nbits_per_width))
            != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->width_lengths, &nbits_per_group_size))
            != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->octet_start_group, &octet_start_group))
            != GRIB_SUCCESS) return err;

    if((err = grib_get_long_internal(gh,self->width_spd_sp_desc, &width_spd_sp_desc))
            != GRIB_SUCCESS) width_spd_sp_desc=-1;

    if((err = grib_get_long_internal(gh,self->nap, &nap)) != GRIB_SUCCESS) return err;

    self->dirty=0;

    n_sp_diff = two_ordr_spd*2+plus1_spd;

    Assert(bits_per_value < (sizeof(unsigned long)*8)-1);

    if ((abitmap=grib_find_accessor(gh,self->bitmap))!=NULL) {
        bitmap_len=grib_byte_count(abitmap);
        bitmap=(unsigned char*)grib_context_malloc_clear(a->context,sizeof(char)*bitmap_len);
        err=grib_unpack_bytes(abitmap,bitmap,&bitmap_len);
        if (err) {grib_context_free(a->context,bitmap); return err;}
    }

    if(bits_per_value == 0)
        return GRIB_NOT_IMPLEMENTED;

    /* I have everything now start decoding       */
    /*
    fprintf(stdout,"\n****************************************\n");
    fprintf(stdout," bits_per_value = %ld\n", bits_per_value);
    fprintf(stdout," reference_value = %g\n", reference_value);
    fprintf(stdout," binary_scale_factor = %ld\n", binary_scale_factor);
    fprintf(stdout," decimal_scale_factor = %ld\n", decimal_scale_factor);
    fprintf(stdout," n1 = %ld\n", n1);
    fprintf(stdout," n2 = %ld\n", n2);
    fprintf(stdout," p1 = %ld\n", p1);
    fprintf(stdout," p2 = %ld\n", p2);
    fprintf(stdout," matrix_values = %ld\n", matrix_values);
    fprintf(stdout," snd_bitmap = %ld\n", snd_bitmap);
    fprintf(stdout," snd_ordr_wdiff = %ld\n", snd_ordr_wdiff);
    fprintf(stdout," general_ext = %ld\n", general_ext);
    fprintf(stdout," boustrophedonic = %ld\n", boustrophedonic);
    fprintf(stdout," two_ordr_spd = %ld \n", two_ordr_spd);

    fprintf(stdout," plus1_spd = %ld\n", plus1_spd);

    fprintf(stdout," n_sp_diff = %d\n", n_sp_diff);
    fprintf(stdout," width_widths = %ld\n", nbits_per_group_size);
    fprintf(stdout," width_lengths = %ld\n", nbits_per_width);
    fprintf(stdout," octet_start_group = %ld\n", octet_start_group);
    fprintf(stdout," width_spd_sp_desc = %ld\n", width_spd_sp_desc);

    fprintf(stdout," offsetsection = %ld\n", offsetsection);
    fprintf(stdout," offset w = %ld\n", octet_start_group + offsetsection);
    fprintf(stdout,"\n****************************************\n");
    */
    if(snd_bitmap || matrix_values)
        return GRIB_NOT_IMPLEMENTED;

    sec_val  = (unsigned long*)grib_context_malloc(a->context,(n_vals)*sizeof(unsigned long));

    buf_width_of_group  +=  a->offset;
    buf_size_of_groups +=  offsetsection+(octet_start_group-1); /* -1 because of documented starting at 1(and not 0)*/
    bufrefs +=  offsetsection+n1-1;                  /* -1 because of documented starting at 1(and not 0)*/

    pointer_of_group_size  = 0;
    pointer_of_group_width    = 0;
    refsp   = 0;

    for(i=0;i < n_sp_diff;i++)
        sec_val[i] =  grib_decode_unsigned_long(buf_width_of_group,  &pointer_of_group_width,  width_spd_sp_desc);


    bias   =  grib_decode_signed_longb(buf_width_of_group,  &pointer_of_group_width,  width_spd_sp_desc);

    bufvals += offsetsection+n2-1;
    bitp   =  0;
    vcount =  n_sp_diff;

    if(pointer_of_group_width%8)
        pointer_of_group_width = 8+(pointer_of_group_width-(pointer_of_group_width%8));
#if KEEP_OLD == 1
    if(sd == NULL){
        sd = grib_context_malloc_clear(a->context,sizeof(second_order_packed));
        sd->packed_byte_count      = 0;
        sd->nbits_per_group_size   = nbits_per_group_size;
        sd->nbits_per_widths       = nbits_per_width;
        sd->size_of_group_array    = p1;
        sd->array_of_group_size    = grib_context_malloc_clear(a->context,sizeof(unsigned long)*sd->size_of_group_array);
        sd->array_of_group_width   = grib_context_malloc_clear(a->context,sizeof(unsigned long)*sd->size_of_group_array);
        sd->array_of_group_refs    = grib_context_malloc_clear(a->context,sizeof( long)*sd->size_of_group_array);
    }
#endif
    for(i=0;i < p1;i++){
        f_width_of_group    = (short) grib_decode_unsigned_long(buf_width_of_group,  &pointer_of_group_width,     nbits_per_width);
        f_size_of_group     = (short) grib_decode_unsigned_long(buf_size_of_groups,  &pointer_of_group_size, nbits_per_group_size);
        ref_vals            =         grib_decode_unsigned_long(bufrefs,  &refsp,    bits_per_value);
#if KEEP_OLD == 1

        if(sd->packed_byte_count == 0){
            sd->array_of_group_width[i]  = f_width_of_group;
            sd->array_of_group_size[i]   = f_size_of_group;
            sd->array_of_group_refs[i]   = ref_vals;
        }
#endif
        for(j=0; j < f_size_of_group;j++){
            sec_val[vcount+j] = ref_vals + grib_decode_unsigned_long(bufvals,  &bitp, f_width_of_group);
        }
        vcount += f_size_of_group;
    }
#if KEEP_OLD == 1
    if(sd->packed_byte_count == 0) sd->packed_byte_count = (bitp+7)/8;
#endif

    Assert (n_vals == vcount);
    /*for(i=0;i < 10;i++)
    printf("readvalue [%d] %ld     %ld bias %ld <<\n", i,sec_val[i],binary_scale_factor,bias );*/

    if(snd_ordr_wdiff)
        de_spatial_difference(a->context,sec_val, n_vals, n_sp_diff, bias);

    if(boustrophedonic)
        reverse_rows(sec_val,n_vals,nap,bitmap,bitmap_len);

    s = grib_power(binary_scale_factor,2);
    d = grib_power(-decimal_scale_factor,10) ;

    for(i=0;i < n_vals;i++)
        val[i] = (double) ((((double)sec_val[i])*s)+reference_value)*d;

    max = val[0];
    min = max;
    for(i=0;i< n_vals;i++)
    {
        if ( val[i] > max )
            max = val[i];
        else if ( val[i] < min )
            min = val[i];
    }
    min *= d;
    max *= d;

    grib_context_free(a->context,sec_val);
    if (bitmap!=NULL) grib_context_free(a->context,bitmap);

    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_2order_packing* self =  (grib_accessor_data_2order_packing*)a;
    grib_handle* gh = grib_handle_of_accessor(a);

    size_t i = 0;
    size_t j = 0;
    size_t n_vals = *len;
    size_t buff_len = 0;
    double divisor = 1;
    long     vcount = 0;
    int      err = 0;
    long     bias = 0;
    double   reference_value;
    long     binary_scale_factor;
    long     bits_per_value;
    /*long     bit_per_val_rectified_for_gribex;*/
    long     decimal_scale_factor;
    long     n1 = 0;
    long     n2 = 0;
    long     p1 = 0;
    long    n_unused_bits = 0;
    long   used_bits = 0;
    long   offsetsection  = 0;
    long   snd_bitmap     = 0;
    long   snd_ordr_wdiff = 0;

    long   matrix_values  = 0;

    long   general_ext       = 0;
    long   boustrophedonic   = 0;
    long   two_ordr_spd      = 0;
    long   plus1_spd         = 0;

    long   octet_start_group = 0;
    long   width_spd_sp_desc = 0;

    unsigned char* buf        = NULL;
    unsigned char* buf_size_of_groups   = NULL;
    unsigned char* buf_width_of_group = NULL;
    unsigned char* buf_refs   = NULL;
    unsigned char* buf_vals   = NULL;

    double d = 0;

    unsigned long*  sec_val    = NULL;
    unsigned long*  group_val  = NULL;

    short  n_sp_diff = 0;
    unsigned char* bitmap=NULL;
    grib_accessor* abitmap=NULL;
    size_t bitmap_len=0;

#if KEEP_OLD == 1

#else
    second_order_packed* sd = NULL;
#endif

    long   bitp = 0;
    long   pointer_of_group_size = 0;
    long   pointer_of_group_width = 0;
    long   refsp = 0;
    long   nap = 0;
    long offsetdata = 0;

    double max;
    double min;
    int extraValues=0;

    size_t nv =0;

    if (*len ==0) return GRIB_NO_VALUES;

    if((err = grib_get_long_internal(gh,self->offsetsection,&offsetsection)) != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->offsetdata,&offsetdata)) != GRIB_SUCCESS)
        return err;
    if((err = grib_get_long_internal(gh,self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->decimal_scale_factor, &decimal_scale_factor))  != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->matrix_values, &matrix_values)) != GRIB_SUCCESS)return err;
    if((err = grib_get_long_internal(gh,self->snd_bitmap, &snd_bitmap)) != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->snd_ordr_wdiff, &snd_ordr_wdiff)) != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->general_ext, &general_ext)) != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->boustrophedonic, &boustrophedonic)) != GRIB_SUCCESS)  return err;
    if((err = grib_get_long_internal(gh,self->width_spd_sp_desc, &width_spd_sp_desc)) != GRIB_SUCCESS) return err;
    if((err = grib_get_long_internal(gh,self->nap, &nap)) != GRIB_SUCCESS) return err;

    if ((abitmap=grib_find_accessor(gh,self->bitmap))!=NULL) {
        bitmap_len=grib_byte_count(abitmap);
        bitmap=(unsigned char*)grib_context_malloc_clear(a->context,sizeof(char)*bitmap_len);
        err=grib_unpack_bytes(abitmap,bitmap,&bitmap_len);
        if (err) {grib_context_free(a->context,bitmap); return err;}
    }

    two_ordr_spd = 1;
    plus1_spd = 0;

    if(bits_per_value == 0)
        return GRIB_NOT_IMPLEMENTED;

    n_sp_diff = two_ordr_spd*2+plus1_spd;
    /*     calculation of integer array   */

    sec_val  = (unsigned long*)grib_context_malloc(a->context,(n_vals)*sizeof(long));
    d = grib_power(decimal_scale_factor,10) ;
    max = val[0];
    min = max;
    for(i=0;i< n_vals;i++)
    {
        if      ( val[i] > max ) max = val[i];
        else if ( val[i] < min ) min = val[i];
    }
    min *= d;
    max *= d;

    /*bit_per_val_rectified_for_gribex = bits_per_value+8-bits_per_value%8;*/
    if (grib_get_nearest_smaller_value(gh,self->reference_value,min,&reference_value)
            !=GRIB_SUCCESS) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "unable to find nearest_smaller_value of %g for %s",min,self->reference_value);
        return GRIB_INTERNAL_ERROR;
    }

    /*  the scale factor in Grib 1 is adjusted in gribex, for "normalization purpose" ... ?*/
    binary_scale_factor    = grib_get_binary_scale_fact(max,reference_value,bits_per_value,&err);

    divisor         = grib_power(-binary_scale_factor,2);


    for(i=0;i< n_vals;i++)
        sec_val[i] = (unsigned long)((((val[i]*d)-reference_value)*divisor)+0.5);

    /*  reverse the rows*/
    if(boustrophedonic)
        reverse_rows(sec_val,n_vals,nap,bitmap,bitmap_len);


    if(snd_ordr_wdiff)
        if((err =  spatial_difference(a->context,sec_val, n_vals, n_sp_diff, &bias))){
            grib_context_free(a->context,sec_val);
            return err;
        }



    /*   for(i=0;i < 10;i++)
      printf("packing value [%d] %g %ld     %ld bias %ld <<\n", i, val[i],sec_val[i],binary_scale_factor,bias );
     */
    nv        = n_vals-n_sp_diff;
    group_val = sec_val+n_sp_diff;

#if KEEP_OLD == 1

#else
    sd = grib_get_second_order_groups(a->context, group_val, nv);
#endif

    bitp   = 0;
    p1     = sd->size_of_group_array;

    bitp                         =   (width_spd_sp_desc*(n_sp_diff+1)) ;
    octet_start_group            =   (bitp+7)/8;

    bitp                         =   (p1*sd->nbits_per_widths);
    octet_start_group           +=   (bitp+7)/8;

    bitp                         =   (octet_start_group*8) + (sd->nbits_per_group_size*p1);
    n1                           =   (bitp+7)/8;

    bitp                         =   n1*8 + bits_per_value*p1;
    n2                           =   (bitp+7)/8;

    used_bits = n2*8;

    buff_len     = (n2+sd->packed_byte_count);

    if((a->offset+buff_len)%2) buff_len++;

    buf = NULL;
    buf = (unsigned char*)grib_context_malloc_clear(a->context,buff_len);

    buf_width_of_group    =  buf;
    buf_size_of_groups    =  buf+octet_start_group;
    buf_refs    =  buf+n1;
    buf_vals    =  buf+n2;

    pointer_of_group_size    = 0;
    pointer_of_group_width   = 0;
    refsp  = 0;
    bitp   = 0;

    for(i=0;i < n_sp_diff;i++)
        grib_encode_unsigned_longb(buf_width_of_group,sec_val[i],  &pointer_of_group_width,  width_spd_sp_desc);

    grib_encode_signed_longb(buf_width_of_group, bias, &pointer_of_group_width,  width_spd_sp_desc);

    if(pointer_of_group_width%8)
        pointer_of_group_width = 8+(pointer_of_group_width-(pointer_of_group_width%8));

    vcount    = n_sp_diff;

    for(i = 0;i<p1;i++){
        grib_encode_unsigned_longb(buf_width_of_group,sd->array_of_group_width[i],&pointer_of_group_width,sd->nbits_per_widths);
        grib_encode_unsigned_longb(buf_size_of_groups,sd->array_of_group_size[i], &pointer_of_group_size ,sd->nbits_per_group_size);
        grib_encode_unsigned_longb(buf_refs,  sd->array_of_group_refs[i],  &refsp  , bits_per_value);
        used_bits += sd->array_of_group_size[i]*sd->array_of_group_width[i];
        for(j=0; j < sd->array_of_group_size[i];j++){
            grib_encode_unsigned_longb(buf_vals, sec_val[vcount+j]-sd->array_of_group_refs[i] ,&bitp , sd->array_of_group_width[i]);

            /*    if(vcount+j < 10)
             printf(">>packing value [%ld] %g %ld     %ld nb %ld <<\n", vcount+j, val[vcount+j],sec_val[vcount+j],binary_scale_factor,sd->array_of_group_refs[i] );*/
        }
        vcount+= sd->array_of_group_size[i];
    }
    n_unused_bits = (buff_len*8)-used_bits;

    grib_buffer_replace(a, buf,buff_len,1,1);

    if((buff_len + (offsetdata-offsetsection)) %2) {
        buff_len ++;
        grib_update_size(a,buff_len);
    }

    octet_start_group += 1+ a->offset-offsetsection;
    if((err = grib_set_long_internal(gh,self->octet_start_group, octet_start_group)) != GRIB_SUCCESS) return err;

    n1 += 1+a->offset-offsetsection;
    if((err = grib_set_long_internal(gh,self->n1,n1)) != GRIB_SUCCESS) return err;

    n2 += 1+a->offset-offsetsection;
    if((err = grib_set_long_internal(gh,self->n2,n2)) != GRIB_SUCCESS) return err;

    extraValues=0;
    while (p1 > 65535) {
        p1-=65536;
        extraValues++;
    }

    if((err = grib_set_long_internal(gh,self->p1,p1)) != GRIB_SUCCESS) return err;

    if((err = grib_set_long_internal(gh,self->extraValues,extraValues)) != GRIB_SUCCESS) return err;

    if((err = grib_set_long_internal(gh,self->p2,n_vals - n_sp_diff)) != GRIB_SUCCESS) return err;

    if((err = grib_set_double_internal(gh,self->reference_value,    reference_value)) != GRIB_SUCCESS)return err;

    {
        /* Make sure we can decode it again */
        double ref = 1e-100;
        grib_get_double_internal(gh,self->reference_value,&ref);
        Assert(ref == reference_value);
    }

    if((err = grib_set_long_internal(gh,self->binary_scale_factor,         binary_scale_factor)) != GRIB_SUCCESS)  return err;

    if((err = grib_set_long_internal(gh,self->decimal_scale_factor, decimal_scale_factor))  != GRIB_SUCCESS)  return err;
    if((err = grib_set_long_internal(gh,self->width_widths,  sd->nbits_per_widths)) != GRIB_SUCCESS) return err;
    if((err = grib_set_long_internal(gh,self->width_lengths, sd->nbits_per_group_size)) != GRIB_SUCCESS) return err;


    err = grib_set_long_internal(gh,self->half_byte, n_unused_bits);
    if(err != GRIB_SUCCESS) return err;

#if KEEP_OLD == 1

#else

    grib_free_second_order_groups(a->context,sd);
#endif
    ;

    grib_context_free(a->context,buf);
    grib_context_free(a->context,sec_val);
    if (bitmap!=NULL) grib_context_free(a->context,bitmap);

    return GRIB_SUCCESS;
}

static int value_count(grib_accessor* a,long* count)
{
    int err=0;
    grib_accessor_data_2order_packing* self =  (grib_accessor_data_2order_packing*)a;
    long  two_ordr_spd = 0;
    long  plus1_spd    = 0;
    grib_handle* gh = grib_handle_of_accessor(a);
    unsigned char* buf_size_of_groups = (unsigned char*)gh->buffer->data;
    long octet_start_group = 0;
    long offsetsection = 0;
    long nbits_per_lengths = 0;
    long pointer_of_group_size = 0;
    long p1 = 0;
    long extraValues=0;

    size_t i = 0;

    if((err=grib_get_long_internal(gh,self->two_ordr_spd, &two_ordr_spd)) != GRIB_SUCCESS)
        return err;
    if( (err=grib_get_long_internal(gh,self->plus1_spd, &plus1_spd)) != GRIB_SUCCESS)
        return err;
    if( (err=grib_get_long_internal(gh,self->width_lengths, &nbits_per_lengths)) != GRIB_SUCCESS)
        return err;
    if( (err=grib_get_long_internal(gh,self->offsetsection, &offsetsection)) != GRIB_SUCCESS)
        return err;
    if( (err=grib_get_long_internal(gh,self->octet_start_group, &octet_start_group)) != GRIB_SUCCESS)
        return err;
    if( (err=grib_get_long_internal(gh,self->p1, &p1)) != GRIB_SUCCESS)
        return err;
    if( (err=grib_get_long_internal(gh,self->extraValues, &extraValues)) != GRIB_SUCCESS)
        return err;

    p1+=extraValues*65536;

    buf_size_of_groups +=  offsetsection+(octet_start_group-1);
    *count = two_ordr_spd*2+plus1_spd;

    for(i=0;i < p1;i++)
        *count +=  grib_decode_unsigned_long(buf_size_of_groups,  &pointer_of_group_size, nbits_per_lengths);

    return err;
}
