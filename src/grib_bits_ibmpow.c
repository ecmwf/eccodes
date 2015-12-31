/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


unsigned long grib_decode_unsigned_long(const unsigned char* p, long *bitp, long nbits)
{
    int i;
    long ret = 0;
    long o = *bitp/8;
    int l = nbits/8;

    if (nbits==0) return 0;

    if(nbits > max_nbits)
    {
        int bits = nbits;
        int mod = bits % max_nbits;

        if(mod != 0)
        {
            int e=grib_decode_unsigned_long(p,bitp,mod);
            Assert( e == 0);
            bits -= mod;
        }

        while(bits > max_nbits)
        {
            int e=grib_decode_unsigned_long(p,bitp,max_nbits);
            Assert( e == 0);
            bits -= max_nbits;
        }

        return grib_decode_unsigned_long(p,bitp,bits);
    }

    if((nbits%8 > 0)||(*bitp%8 > 0)) {
        for(i=0; i< nbits;i++){
            ret <<= 1;
            if(grib_get_bit( p, *bitp)) ret += 1;
            *bitp += 1;
        }
        return ret;
    }

    ret  <<= 8;
    ret |= p[o++] ;

    for ( i=1; i<l; i++ )
    {
        ret <<= 8;
        ret |= p[o++] ;
    }
    *bitp += nbits;

    return ret;
}

int grib_encode_unsigned_long(unsigned char* p, unsigned long val ,long *bitp, long nbits)
{
    long            len     = nbits;
    int              s      = *bitp%8;
    int              n      = 8-s;
    unsigned char tmp = 0; /*for temporary results*/

    if(nbits > max_nbits)
    {
        /* TODO: Do some real code here, to support long longs */
        int bits = nbits;
        int mod = bits % max_nbits;
        long zero = 0;

        /* printf("Warning: encoding %ld bits=%ld %ld\n",val,nbits,*bitp); */

        if(mod != 0)
        {
            int e=grib_encode_unsigned_long(p,zero,bitp,mod);
            /* printf(" -> : encoding %ld bits=%ld %ld\n",zero,(long)mod,*bitp); */
            Assert( e == 0);
            bits -= mod;
        }

        while(bits > max_nbits)
        {
            int e=grib_encode_unsigned_long(p,zero,bitp,max_nbits);
            /* printf(" -> : encoding %ld bits=%ld %ld\n",zero,(long)max_nbits,*bitp); */
            Assert(e == 0);
            bits -= max_nbits;
        }

        /* printf(" -> : encoding %ld bits=%ld %ld\n",val,(long)bits,*bitp); */
        return grib_encode_unsigned_long(p,val,bitp,bits);
    }

    if(s)
        p += (*bitp >> 3); /* skip the bytes */
    else
        p += (*bitp >> 3); /* skip the bytes */

    /* head */
    if(s) {
        len -= n;
        if (len < 0) {
            tmp = ((val << -len) | ((*p) & dmasks[n]));
        } else {
            tmp = ((val >> len) | ((*p) & dmasks[n]));
        }
        *p = tmp;
        (*p)++;
    }

    /*  write the middle words */
    while(len >= 8)
    {
        len   -= 8;
        *p++ = (val >> len);
    }

    /*  write the end bits */
    if(len) *p =  (val << (8-len));

    *bitp += nbits;
    return GRIB_SUCCESS;
}

int grib_encode_unsigned_longb(unsigned char* p, unsigned long val ,long *bitp, long nb)
{
    long  i = 0;

    Assert(nb <= max_nbits);

    for(i=nb-1; i >= 0; i--){
        if(test(val,i))
            grib_set_bit_on (p, bitp);
        else
            grib_set_bit_off(p, bitp);
    }
    return GRIB_SUCCESS;
}

#include "grib_bits_ibmpow_opt.c"

