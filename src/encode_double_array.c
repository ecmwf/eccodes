/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


#ifdef _GET_IBM_COUNTER
#include <libhpc.h>
#endif


static
void encode_double_array_common(int numBits, long packStart, long datasize, GRIBPACK *lGrib,
        const double *data, double zref, double factor, long *gz)
{
    long i, z = *gz;
    unsigned int ival;
    int cbits, jbits;
    unsigned int c;
    static unsigned int mask[] = {0,1,3,7,15,31,63,127,255};

    /* code from gribw routine flist2bitstream */

    cbits = 8;
    c = 0;
    for ( i = packStart; i < datasize; i++ )
    {
        /* note float -> unsigned int .. truncate */
        ival = (unsigned int) ((data[i] - zref) * factor + 0.5);
        /*
	if ( ival > max_nbpv_pow2 ) ival = max_nbpv_pow2;
	if ( ival < 0 ) ival = 0;
         */
        jbits = numBits;
        while ( cbits <= jbits )
        {
            if ( cbits == 8 )
            {
                jbits -= 8;
                lGrib[z++] = (ival >> jbits) & 0xFF;
            }
            else
            {
                jbits -= cbits;
                lGrib[z++] = (c << cbits) + ((ival >> jbits) & mask[cbits]);
                cbits = 8;
                c = 0;
            }
        }
        /* now jbits < cbits */
        if ( jbits )
        {
            c = (c << jbits) + (ival & mask[jbits]);
            cbits -= jbits;
        }
    }
    if ( cbits != 8 ) lGrib[z++] = c << cbits;

    *gz = z;
}

static
void encode_double_array_byte(int numBits, long packStart, long datasize, 
        GRIBPACK *restrict lGrib,
        const double *restrict data,
        double zref, double factor, long *restrict gz)
{
    long i, z = *gz;
    unsigned long ival;
    double tmp;

    data += packStart;
    datasize -= packStart;

    if      ( numBits ==  8 )
    {
#ifdef _GET_IBM_COUNTER 
        hpmStart(2, "pack 8 bit base");
#endif
#if   defined (CRAY)
#pragma _CRI ivdep
#elif defined (SX)
#pragma vdir nodep
#elif defined (__uxp__)
#pragma loop novrec
#endif
        for ( i = 0; i < datasize; i++ )
        {
            tmp = ((data[i] - zref) * factor + 0.5);
            ival = (unsigned long) tmp;
            lGrib[z  ] = ival;
            z++;
        }
#ifdef _GET_IBM_COUNTER 
        hpmStop(2);
#endif
    }
    else if ( numBits == 16 )
    {
#ifdef _GET_IBM_COUNTER 
        hpmStart(3, "pack 16 bit base");
#endif
#if   defined (CRAY)
#pragma _CRI ivdep
#elif defined (SX)
#pragma vdir nodep
#elif defined (__uxp__)
#pragma loop novrec
#endif
        for ( i = 0; i < datasize; i++ )
        {
            tmp = ((data[i] - zref) * factor + 0.5);
            ival = (unsigned long) tmp;
            lGrib[z  ] = ival >>  8;
            lGrib[z+1] = ival;
            z += 2;
        }
#ifdef _GET_IBM_COUNTER 
        hpmStop(3);
#endif
    }
    else if ( numBits == 24 )
    {
#ifdef _GET_IBM_COUNTER 
        hpmStart(4, "pack 24 bit base");
#endif
#if   defined (CRAY)
#pragma _CRI ivdep
#elif defined (SX)
#pragma vdir nodep
#elif defined (__uxp__)
#pragma loop novrec
#endif
        for ( i = 0; i < datasize; i++ )
        {
            tmp = ((data[i] - zref) * factor + 0.5);
            ival = (unsigned long) tmp;
            lGrib[z  ] =  ival >> 16;
            lGrib[z+1] =  ival >>  8;
            lGrib[z+2] =  ival;
            z += 3;
        }
#ifdef _GET_IBM_COUNTER 
        hpmStop(4);
#endif
    }
    else if ( numBits == 32 )
    {
#ifdef _GET_IBM_COUNTER 
        hpmStart(5, "pack 32 bit base");
#endif
#if   defined (CRAY)
#pragma _CRI ivdep
#elif defined (SX)
#pragma vdir nodep
#elif defined (__uxp__)
#pragma loop novrec
#endif
        for ( i = 0; i < datasize; i++ )
        {
            tmp = ((data[i] - zref) * factor + 0.5);
            ival = (unsigned long) tmp;
            lGrib[z  ] =  ival >> 24;
            lGrib[z+1] =  ival >> 16;
            lGrib[z+2] =  ival >>  8;
            lGrib[z+3] =  ival;
            z += 4;
        }
#ifdef _GET_IBM_COUNTER 
        hpmStop(5);
#endif
    }
    else if ( numBits > 0 && numBits <= 32 )
    {
        encode_double_array_common(numBits, 0, datasize, lGrib,
                data, zref, factor, &z);
    }
    else if ( numBits == 0 )
    {
    }
    else
    {
        Error("Unimplemented packing factor %d!", numBits);
    }

    *gz = z;
}

static
void encode_double_array_unrolled(int numBits, long packStart, long datasize, 
        GRIBPACK *restrict lGrib,
        const double *restrict data,
        double zref, double factor, long *restrict gz)
{
    U_BYTEORDER;
    long i, j, z = *gz;
#ifdef _ARCH_PWR6
#define __UNROLL_DEPTH_2 8
#else
#define __UNROLL_DEPTH_2 8
#endif

    data += packStart;
    datasize -= packStart;

    {
        long residual =  datasize % __UNROLL_DEPTH_2;
        long ofs = datasize - residual;
        double dval[__UNROLL_DEPTH_2];
        unsigned long ival;

        /* reducing FP operations to single FMA is slowing down on pwr6 ... */

        if      ( numBits ==  8 )
        {
            unsigned char *cgrib = (unsigned char *) (lGrib + z);
#ifdef _GET_IBM_COUNTER 
            hpmStart(2, "pack 8 bit unrolled");
#endif
            for ( i = 0; i < datasize - residual; i += __UNROLL_DEPTH_2 )
            {
                for (j = 0; j < __UNROLL_DEPTH_2; j++)
                {
                    dval[j] = ((data[i+j] - zref) * factor + 0.5);
                }
                for (j = 0; j < __UNROLL_DEPTH_2; j++)
                {
                    *cgrib =  (unsigned long) dval[j];
                    cgrib++;
                    z++;
                }
            }
            for (j = 0; j < residual; j++)
            {
                dval[j] = ((data[ofs+j] - zref) * factor + 0.5);
            }
            for (j = 0; j < residual; j++)
            {
                *cgrib = (unsigned long) dval[j];
                cgrib++;
                z++;
            }
#ifdef _GET_IBM_COUNTER 
            hpmStop(2);
#endif
        }
        else if ( numBits == 16 )
        {
            unsigned short *sgrib = (unsigned short *) (lGrib + z);
#ifdef _GET_IBM_COUNTER 
            hpmStart(3, "pack 16 bit unrolled");
#endif
            for ( i = 0; i < datasize - residual; i += __UNROLL_DEPTH_2 )
            {
                for (j = 0; j < __UNROLL_DEPTH_2; j++)
                {
                    dval[j] = ((data[i+j] - zref) * factor + 0.5);
                }
                if ( IS_BIGENDIAN() )
                {
                    for (j = 0; j < __UNROLL_DEPTH_2; j++)
                    {
                        *sgrib = (unsigned long) dval[j];
                        sgrib++;
                        z += 2;
                    }
                }
                else
                {
                    for (j = 0; j < __UNROLL_DEPTH_2; j++)
                    {
                        ival = (unsigned long) dval[j];
                        lGrib[z  ] = ival >>  8;
                        lGrib[z+1] = ival;
                        z += 2;
                    }
                }
            }
            for (j = 0; j < residual; j++)
            {
                dval[j] = ((data[ofs+j] - zref) * factor + 0.5);
            }
            if ( IS_BIGENDIAN() )
            {
                for (j = 0; j < residual; j++)
                {
                    *sgrib = (unsigned long) dval[j];
                    sgrib++;
                    z += 2;
                }
            }
            else
            {
                for (j = 0; j < residual; j++)
                {
                    ival = (unsigned long) dval[j];
                    lGrib[z  ] = ival >>  8;
                    lGrib[z+1] = ival;
                    z += 2;
                }
            }
#ifdef _GET_IBM_COUNTER 
            hpmStop(3);
#endif
        }
        else if ( numBits == 24 )
        {
#ifdef _GET_IBM_COUNTER 
            hpmStart(4, "pack 24 bit unrolled");
#endif
            for ( i = 0; i < datasize - residual; i += __UNROLL_DEPTH_2 )
            {
                for (j = 0; j < __UNROLL_DEPTH_2; j++)
                {
                    dval[j] = ((data[i+j] - zref) * factor + 0.5);
                }
                for (j = 0; j < __UNROLL_DEPTH_2; j++)
                {
                    ival = (unsigned long) dval[j];
                    lGrib[z  ] =  ival >> 16;
                    lGrib[z+1] =  ival >>  8;
                    lGrib[z+2] =  ival;
                    z += 3;
                }
            }
            for (j = 0; j < residual; j++)
            {
                dval[j] = ((data[ofs+j] - zref) * factor + 0.5);
            }
            for (j = 0; j < residual; j++)
            {
                ival = (unsigned long) dval[j];
                lGrib[z  ] =  ival >> 16;
                lGrib[z+1] =  ival >>  8;
                lGrib[z+2] =  ival;
                z += 3;
            }
#ifdef _GET_IBM_COUNTER 
            hpmStop(4);
#endif
        }
        else if ( numBits == 32 )
        {
#ifdef _GET_IBM_COUNTER 
            hpmStart(5, "pack 32 bit unrolled");
#endif
            unsigned int *igrib = (unsigned int *) (lGrib + z);
            for ( i = 0; i < datasize - residual; i += __UNROLL_DEPTH_2 )
            {
                for (j = 0; j < __UNROLL_DEPTH_2; j++)
                {
                    dval[j] = ((data[i+j] - zref) * factor + 0.5);
                }
                if ( IS_BIGENDIAN() )
                {
                    for (j = 0; j < __UNROLL_DEPTH_2; j++)
                    {
                        *igrib = (unsigned long) dval[j];
                        igrib++;
                        z += 4;
                    }
                }
                else
                {
                    for (j = 0; j < __UNROLL_DEPTH_2; j++)
                    {
                        ival = (unsigned long) dval[j];
                        lGrib[z  ] =  ival >> 24;
                        lGrib[z+1] =  ival >> 16;
                        lGrib[z+2] =  ival >>  8;
                        lGrib[z+3] =  ival;
                        z += 4;
                    }
                }
            }
            for (j = 0; j < residual; j++)
            {
                dval[j] = ((data[ofs+j] - zref) * factor + 0.5);
            }
            if ( IS_BIGENDIAN() )
            {
                for (j = 0; j < residual; j++)
                {
                    *igrib = (unsigned long) dval[j];
                    igrib++;
                    z += 4;
                }
            }
            else
            {
                for (j = 0; j < residual; j++)
                {
                    ival = (unsigned long) dval[j];
                    lGrib[z  ] =  ival >> 24;
                    lGrib[z+1] =  ival >> 16;
                    lGrib[z+2] =  ival >>  8;
                    lGrib[z+3] =  ival;
                    z += 4;
                }
            }
#ifdef _GET_IBM_COUNTER 
            hpmStop(5);
#endif
        }
        else if ( numBits > 0 && numBits <= 32 )
        {
            encode_double_array_common(numBits, 0, datasize, lGrib,
                    data, zref, factor, &z);
        }
        else if ( numBits == 0 )
        {
        }
        else
        {
            Error("Unimplemented packing factor %d!", numBits);
        }
    }

    *gz = z;
#undef __UNROLL_DEPTH_2
}
