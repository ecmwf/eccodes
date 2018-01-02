/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

int grib_decode_long_array(const unsigned char* p, long *bitp, long bitsPerValue,
        size_t n_vals,long* val) {
    long i=0;
    unsigned long lvalue = 0;

    if(bitsPerValue%8)
    {
        int j=0;
        for(i=0;i < n_vals;i++) {
            lvalue=0;
            for(j=0; j< bitsPerValue;j++){
                lvalue <<= 1;
                if(grib_get_bit( p, *bitp)) lvalue += 1;
                *bitp += 1;
            }
            val[i] = lvalue;
        }
    }  else  {
        int bc;
        int l = bitsPerValue/8;
        size_t o = *bitp/8;

        for(i=0;i < n_vals;i++)
        {
            lvalue  = 0;
            lvalue  <<= 8;
            lvalue |= p[o++] ;

            for ( bc=1; bc<l; bc++ )
            {
                lvalue <<= 8;
                lvalue |= p[o++] ;
            }
            val[i] = lvalue;
        }
        *bitp+=bitsPerValue*n_vals;
    }

    return 0;
}

int grib_decode_double_array(const unsigned char* p, long *bitp, long bitsPerValue,
        double reference_value,double s,double d,
        size_t n_vals,double* val) {
    long i=0;
    unsigned long lvalue = 0;
    double x;

    if(bitsPerValue%8)
    {
        int j=0;
        for(i=0;i < n_vals;i++) {
            lvalue=0;
            for(j=0; j< bitsPerValue;j++){
                lvalue <<= 1;
                if(grib_get_bit( p, *bitp)) lvalue += 1;
                *bitp += 1;
            }
            x=((lvalue*s)+reference_value)*d;
            val[i] = (double)x;
        }
    }  else  {
        int bc;
        int l = bitsPerValue/8;
        size_t o = 0;

        for(i=0;i < n_vals;i++)
        {
            lvalue  = 0;
            lvalue  <<= 8;
            lvalue |= p[o++] ;

            for ( bc=1; bc<l; bc++ )
            {
                lvalue <<= 8;
                lvalue |= p[o++] ;
            }
            x=((lvalue*s)+reference_value)*d;
            val[i] = (double)x;
        }
    }

    return 0;
}

/* code from grib_bits_fast_big_endian_simple.c */
int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* d,size_t size,double* val) {
    long i=0;
    long countOfLeftmostBits=0,leftmostBits=0;
    long startBit;
    long remainingBits = nbits;
    long *pp=(long*)p;
    int inited=0;
    unsigned long uval=0;

    startBit=*bitp;
    remainingBits = nbits;
    if (startBit >= max_nbits) {
        pp+=startBit/max_nbits;
        startBit %= max_nbits;
    }

    if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {
        for (i=0;i<size;i++) {
            if (startBit == max_nbits) {
                startBit = 0;
                pp++;
            }

            val[i]=VALUE(*pp,startBit,remainingBits);
            val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
            startBit+=remainingBits;
            remainingBits=nbits;
        }

    } else {

        for (i=0;i<size;i++) {
            countOfLeftmostBits = startBit + remainingBits;
            if (countOfLeftmostBits > max_nbits) {
                countOfLeftmostBits = max_nbits - startBit;
                remainingBits -= countOfLeftmostBits;
                leftmostBits=(VALUE(*pp,startBit,countOfLeftmostBits)) << remainingBits;
                startBit = 0;
                pp++;
            } else
                leftmostBits = 0;

            val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
            val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
            startBit+=remainingBits;
            remainingBits=nbits;
        }
    }

    *bitp+=size*nbits;

    return GRIB_SUCCESS;
}

int grib_encode_long_array(size_t n_vals,const long* val,long bits_per_value,unsigned char* p,long *off)
{
    size_t i=0;
    unsigned long unsigned_val=0;
    unsigned char *encoded=p;
    if(bits_per_value%8){
        for(i=0;i< n_vals;i++){
            unsigned_val=val[i];
            grib_encode_unsigned_longb(encoded, unsigned_val, off , bits_per_value);
        }
    } else{
        for(i=0;i< n_vals;i++){
            int blen=0;
            blen = bits_per_value;
            unsigned_val = val[i];
            while(blen >= 8)
            {
                blen   -= 8;
                *encoded = (unsigned_val >> blen);
                encoded++;
                *off+=8;
            }
        }
    }
    return GRIB_SUCCESS;
}

#define  U_BYTEORDER     static union {unsigned long l; unsigned char c[sizeof(long)];} u_byteorder = {1}
#define  IS_BIGENDIAN()  (u_byteorder.c[sizeof(long) - 1])
#define  GRIBPACK      unsigned char
#define  Error(x, y)   fprintf(stderr, x, y)
#define  restrict

#include "encode_double_array.c"

#undef  U_BYTEORDER
#undef  IS_BIGENDIAN
#undef  GRIBPACK
#undef  Error
#undef  restrict

int grib_encode_double_array(size_t n_vals,const double* val,long bits_per_value,double reference_value,double d,double divisor,unsigned char* p,long *off)
{
    if ( fabs(d - 1) > 0 ) /* d != 1 */
    {
        size_t i=0;
        unsigned long unsigned_val=0;
        unsigned char *encoded=p;
        double x;

        if(bits_per_value%8){
            for(i=0;i< n_vals;i++){
                x=(((val[i]*d)-reference_value)*divisor)+0.5;
                unsigned_val = (unsigned long)x;
                grib_encode_unsigned_longb(encoded, unsigned_val, off , bits_per_value);
            }
        } else{
            for(i=0;i< n_vals;i++){
                int blen=0;
                blen = bits_per_value;
                x = ((((val[i]*d)-reference_value)*divisor)+0.5);
                unsigned_val = (unsigned long)x;
                while(blen >= 8)
                {
                    blen   -= 8;
                    *encoded = (unsigned_val >> blen);
                    encoded++;
                    *off+=8;
                }
            }
        }
    }
    else
    {
#if  defined  (_ARCH_PWR6)
        encode_double_array_unrolled(bits_per_value, 0, n_vals, p, val, reference_value, divisor, off);
#else
        encode_double_array_byte    (bits_per_value, 0, n_vals, p, val, reference_value, divisor, off);
#endif
    }

    return GRIB_SUCCESS;
}

/* code from grib_bits_fast_big_endian_simple.c */
int grib_encode_double_array_complex(size_t n_vals,double* val,long nbits,double reference_value,
        double* scal,double d,double divisor,unsigned char* p,long *bitp) {
    long* destination = (long*)p;
    double* v=val;
    long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0;
    unsigned long uval=0;
    size_t i=0;

    startBit=*bitp;
    remainingBits = nbits;

    if (startBit >= max_nbits) {
        destination += startBit / max_nbits;
        startBit %= max_nbits;
    }

    if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

        for(i=0;i< n_vals;i++) {
            uval  = (unsigned long)(((((*v)*d*scal[i/2])-reference_value)*divisor)+0.5);
            if (startBit == max_nbits) {
                startBit = 0;
                destination++;
            }

            rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
            *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
                + (rightmostBits << max_nbits-(remainingBits+startBit));
            startBit+=remainingBits;
            remainingBits=nbits;

            v++;
        }

    } else {

        for(i=0;i< n_vals;i++) {
            countOfLeftmostBits = startBit + remainingBits;
            uval  = (unsigned long)(((((*v)*d*scal[i/2])-reference_value)*divisor)+0.5);
            if (countOfLeftmostBits > max_nbits) {
                countOfLeftmostBits = max_nbits - startBit;
                startBit = max_nbits - remainingBits;
                remainingBits -= countOfLeftmostBits;
                *destination = (((*destination) >> countOfLeftmostBits) << countOfLeftmostBits)
                  + (VALUE(uval,startBit,countOfLeftmostBits));
                startBit = 0;
                destination++;
            }

            rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
            *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
                + (rightmostBits << max_nbits-(remainingBits+startBit));
            startBit+=remainingBits;
            remainingBits=nbits;

            v++;
        }
    }

    *bitp+=n_vals*nbits;

    return 0;
}
