/*
 * Copyright 2005-2018 ECMWF.
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

#if GRIB_PTHREADS
static pthread_once_t once  = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void init() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex,&attr);
    pthread_mutexattr_destroy(&attr);
}
#elif GRIB_OMP_THREADS
static int once = 0;
static omp_nest_lock_t mutex;

static void init()
{
    GRIB_OMP_CRITICAL(lock_grib_bits_any_endian_c)
    {
        if (once == 0)
        {
            omp_init_nest_lock(&mutex);
            once = 1;
        }
    }
}
#endif

typedef struct bits_all_one_t {
    int inited;
    int size;
    long v[128];
} bits_all_one_t;

static bits_all_one_t bits_all_one={0,0,{0,}};

static void init_bits_all_one()
{
    int size=sizeof(long)*8;
    long* v=0;
    unsigned long cmask=-1;
    if (!bits_all_one.inited) {
        bits_all_one.size=size;
        bits_all_one.inited=1;
        v=bits_all_one.v+size;
        /*
         * The result of a shift operation is undefined if the RHS is negative or
         * greater than or equal to the number of bits in the (promoted) shift-expression
         */
        /* *v= cmask << size; */
        *v = -1;
        while (size>0)  *(--v)= ~(cmask << --size);
    }
}

static void init_bits_all_one_if_needed()
{
    GRIB_MUTEX_INIT_ONCE(&once,&init);
    GRIB_MUTEX_LOCK(&mutex);
    if (!bits_all_one.inited) init_bits_all_one();
    GRIB_MUTEX_UNLOCK(&mutex);
}
int grib_is_all_bits_one(long val, long nbits)
{
    /*if (!bits_all_one.inited) init_bits_all_one();*/
    init_bits_all_one_if_needed();
    return bits_all_one.v[nbits]==val;
}

int grib_encode_string(unsigned char *bitStream, long *bitOffset, size_t numberOfCharacters, const char *string)
{
    size_t i;
    int err=0;
    long byteOffset = *bitOffset / 8;
    int remainder = *bitOffset % 8;
    unsigned char c;
    unsigned char* p;
    unsigned char mask[] ={ 0, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    int remainderComplement=8-remainder;
    char str[512]={0,};
    char *s=str;

    Assert(numberOfCharacters<512);

    if (string) memcpy(s,string,strlen(string));

    /* if (remainder) byteOffset++; */

    if (numberOfCharacters==0) return err;

    p=(unsigned char*)bitStream+byteOffset;

    if ( remainder == 0 )  {
        memcpy(p,str,numberOfCharacters);
        *bitOffset+=numberOfCharacters*8;
        return err;
    }

    for (i=0;i<numberOfCharacters;i++) {
        c=((*s)>>remainder) & ~mask[remainder];
        *p |= c;
        p++;
        *p = ((*s)<<remainderComplement) & mask[remainder];
        s++;
    }
    *bitOffset+=numberOfCharacters*8;
    return err;
}

char* grib_decode_string(const unsigned char* bitStream, long *bitOffset, size_t numberOfCharacters,char* string)
{
    size_t i;
    long byteOffset = *bitOffset / 8;
    int remainder = *bitOffset % 8;
    unsigned char c;
    unsigned char* p;
    char* s=string;
    unsigned char mask[] ={ 0, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    int remainderComplement=8-remainder;

    if (numberOfCharacters==0) return string;

    p=(unsigned char*)bitStream+byteOffset;

    if ( remainder == 0 )  {
        memcpy(string,bitStream+byteOffset,numberOfCharacters);
        *bitOffset+=numberOfCharacters*8;
        return string;
    }

    for (i=0;i<numberOfCharacters;i++) {
        c=(*p)<<remainder;
        p++;
        *s = ( c | ( (*p) & mask[remainder] )>>remainderComplement );
        s++;
    }
    *bitOffset+=numberOfCharacters*8;
    return string;
}

/* A mask with x least-significant bits set, possibly 0 or >=32 */
/* -1UL is 1111111... in every bit in binary representation */
#define BIT_MASK(x) \
        (((x) == max_nbits) ? \
                (unsigned long) -1UL : (1UL << (x)) - 1)
/**
 * decode a value consisting of nbits from an octet-bitstream to long-representation
 *
 * @param p input bitstream, for technical reasons put into octets
 * @param bitp current start position in the bitstream
 * @param nbits number of bits needed to build a number (e.g. 8=byte, 16=short, 32=int, but also other sizes allowed)
 * @return value encoded as 32/64bit numbers
 */
unsigned long grib_decode_unsigned_long(const unsigned char* p, long *bitp, long nbits)
{
    unsigned long ret = 0;
    long oc = *bitp/8;
    unsigned long mask = 0;
    long pi = 0;
    int usefulBitsInByte = 0;
    long bitsToRead = 0;

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
#if 0
    long ret2 = 0;
    for(i=0; i< nbits;i++){
        ret2 <<= 1;
        if(grib_get_bit( p, *bitp)) ret2 += 1;
        *bitp += 1;
    }
    *bitp -= nbits;
#else
    mask = BIT_MASK(nbits);
    /* pi: position of bitp in p[]. >>3 == /8 */
    pi = oc;
    /* number of useful bits in current byte */
    usefulBitsInByte = 8-(*bitp & 7);
    /* read at least enough bits (byte by byte) from input */
    bitsToRead = nbits;
    while (bitsToRead > 0) {
        ret  <<= 8;
        /*   ret += p[pi];     */
        DebugAssert( (ret & p[pi]) == 0 );
        ret = ret | p[pi];
        pi++;
        bitsToRead -= usefulBitsInByte;
        usefulBitsInByte = 8;
    }
    *bitp += nbits;
    /* printf("%d %d %d\n", pi, ret, offset); */
    /* bitsToRead might now be negative (too many bits read) */
    /* remove those which are too much */
    ret >>= -1*bitsToRead;
    /* remove leading bits (from previous value) */
    ret &= mask;
    /* printf("%d %d\n", ret2, ret);*/
#endif
    return ret;
}

int grib_encode_unsigned_long(unsigned char* p, unsigned long val ,long *bitp, long nbits)
{
    long len = nbits;
    int s = *bitp % 8;
    int n = 8 - s;
    unsigned char tmp = 0; /*for temporary results*/

    if (nbits > max_nbits) {
        /* TODO: Do some real code here, to support long longs */
        int bits = nbits;
        int mod = bits % max_nbits;
        long zero = 0;

        if (mod != 0) {
            int e = grib_encode_unsigned_long(p, zero, bitp, mod);
            /* printf(" -> : encoding %ld bits=%ld %ld\n",zero,(long)mod,*bitp); */
            Assert(e == 0);
            bits -= mod;
        }

        while (bits > max_nbits) {
            int e = grib_encode_unsigned_long(p, zero, bitp, max_nbits);
            /* printf(" -> : encoding %ld bits=%ld %ld\n",zero,(long)max_nbits,*bitp); */
            Assert(e == 0);
            bits -= max_nbits;
        }

        /* printf(" -> : encoding %ld bits=%ld %ld\n",val,(long)bits,*bitp); */
        return grib_encode_unsigned_long(p, val, bitp, bits);
    }

    if (s)
        p += (*bitp >> 3); /* skip the bytes */
    else
        p += (*bitp >> 3); /* skip the bytes */

    /* head */
    if (s) {
        len -= n;
        if (len < 0) {
            tmp = ((val << -len) | ((*p) & dmasks[n]));
        } else {
            tmp = ((val >> len) | ((*p) & dmasks[n]));
        }
        *p = tmp;
        (*p)++;

        /*Beware of code like this! compiler warning: operation may be undefined
      Read GCC manual on -Wsequence-point*/
        /* *p  = ((val << -len) | ((*p)++ & dmasks[n])); */
    }

    /*  write the middle words */
    while (len >= 8) {
        len -= 8;
        *p++ = (val >> len);
    }

    /*  write the end bits */
    if (len)
        *p = (val << (8 - len));

    *bitp += nbits;
    return GRIB_SUCCESS;
}

int grib_encode_unsigned_longb(unsigned char* p, unsigned long val ,long *bitp, long nb)
{
    long  i = 0;

    if (nb > max_nbits) {
        fprintf(stderr, "Number of bits (%ld) exceeds maximum number of bits (%d)\n", nb, max_nbits);
        Assert(0);
    }
#ifdef DEBUG
    {
        unsigned long maxV = grib_power(nb,2);
        if (val > maxV) {
            fprintf(stderr, "grib_encode_unsigned_longb: Value=%lu, but number of bits=%ld!\n", val, nb);
            Assert(0);
        }
    }
#endif
    for(i=nb-1; i >= 0; i--){
        if(test(val,i))
            grib_set_bit_on (p, bitp);
        else
            grib_set_bit_off(p, bitp);
    }
    return GRIB_SUCCESS;
}

#if OMP_PACKING
 #include "grib_bits_any_endian_omp.c"
#elif VECTOR
 #include "grib_bits_any_endian_vector.c"
#else
 #include "grib_bits_any_endian_simple.c"
#endif
