/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Enrico Fucile  - 19.06.2007                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

#if OMP_PACKING
#include "omp.h"
#endif

#define mask1(i)    (1u << i)
#define test(n,i)    !!((n) & mask1(i))

long GRIB_MASK = -1;       /* Mask of sword bits */

# define VALUE(p,q,b) \
 (((b)==max_nbits ? GRIB_MASK : ~(GRIB_MASK<<(b))) & ((p)>>(max_nbits-((q)+(b)))))

# define MASKVALUE(q,b) \
 ((b)==max_nbits ? GRIB_MASK : (~(GRIB_MASK<<(b))<<(max_nbits-((q)+(b)))))


static unsigned long dmasks[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, };

static int max_nbits = sizeof(unsigned long)*8;

unsigned long grib_decode_unsigned_byte_long(const unsigned char* p, long o, int l)
{
    long accum = 0;
    int i = 0;
    unsigned char b = p[o++];

    Assert(l <= max_nbits);

    accum  <<= 8;
    accum |= b ;

    for ( i=1; i<l; i++ )
    {
        b = p[o++];
        accum <<= 8;
        accum |= b ;
    }
    return accum;
}

long grib_decode_signed_long(const unsigned char* p, long o, int l)
{
    long accum = 0;
    int i = 0;
    unsigned char b = p[o++];
    int sign = grib_get_bit(&b, 0);

    Assert(l <= max_nbits);

    b &= 0x7f;
    accum  <<= 8;
    accum |= b ;

    for ( i=1; i<l; i++ )
    {
        b = p[o++];
        accum <<= 8;
        accum |= b ;
    }
    if (sign == 0)
        return accum;
    else
        return -accum;
}

int grib_encode_signed_long(unsigned char* p, long val , long o, int l)
{
    unsigned short accum = 0;
    int i = 0;
    int off = o;
    int sign = (val<0);

    Assert(l <= max_nbits);

    if (sign) val *= -1;

    for (  i=0; i<l; i++ )
    {
        accum    =  val >> (l*8-(8*(i+1)));
        p[o++] =  accum ;
    }

    if (sign) p[off] |= 128;

    return GRIB_SUCCESS;
}

static void grib_set_bit_on( unsigned char* p, long* bitp){

    p +=  *bitp/8;
    *p |=  (1u << (7-((*bitp)%8)));
    (*bitp)++;
}

void grib_set_bits_on( unsigned char* p, long* bitp,long nbits){
    int i;
    for (i=0;i<nbits;i++) {
      grib_set_bit_on(p,bitp);
    }
}

static void grib_set_bit_off( unsigned char* p, long* bitp){

    p +=  *bitp/8;
    *p &= ~(1u << (7-((*bitp)%8)));
    (*bitp)++;
}

int grib_get_bit(const unsigned char* p, long bitp){
    p += (bitp >> 3);
    return (*p&(1<<(7-(bitp%8))));
}

void grib_set_bit( unsigned char* p, long bitp, int val){

    if(val == 0) grib_set_bit_off(p,&bitp);
    else grib_set_bit_on(p,&bitp);
}

long grib_decode_signed_longb(const unsigned char* p, long *bitp, long nbits)
{
    int sign = grib_get_bit(p, *bitp);
    long val = 0;

    Assert(nbits <= max_nbits);

    *bitp += 1;

    val = grib_decode_unsigned_long( p, bitp,  nbits-1);

    if (sign)val = -val;

    return  val;
}

int grib_encode_signed_longb(unsigned char* p,  long val ,long *bitp, long nb)
{
    short  sign = val < 0;

    Assert(nb <= max_nbits);

    if(sign) val = -val;

    if(sign)
        grib_set_bit_on (p, bitp);
    else
        grib_set_bit_off(p, bitp);

    return grib_encode_unsigned_longb(p, val ,bitp, nb-1);
}

#if GRIB_IBMPOWER67_OPT

#include "grib_bits_ibmpow.c"

#else
#if FAST_BIG_ENDIAN

#include "grib_bits_fast_big_endian.c"

#else

#include "grib_bits_any_endian.c"

#endif

#endif
