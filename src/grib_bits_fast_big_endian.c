/*
 * (C) Copyright 2005- ECMWF.
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


typedef struct bits_all_one_t
{
    int inited;
    int size;
    long v[128];
} bits_all_one_t;

static bits_all_one_t bits_all_one = { 0, 0, {0,} };

static void init_bits_all_one()
{
    int size            = sizeof(long) * 8;
    long* v             = 0;
    unsigned long cmask = -1;
    bits_all_one.size   = size;
    bits_all_one.inited = 1;
    v                   = bits_all_one.v + size;
    /*
     * The result of a shift operation is undefined if the RHS is negative or
     * greater than or equal to the number of bits in the (promoted) shift-expression
     */
    /* *v= cmask << size; */
    *v = -1;
    while (size > 0)
        *(--v) = ~(cmask << --size);
}

int grib_is_all_bits_one(long val, long nbits)
{
    if (!bits_all_one.inited)
        init_bits_all_one();
    return bits_all_one.v[nbits] == val;
}

int grib_encode_string(unsigned char* bitStream, long* bitOffset, size_t numberOfCharacters, const char* string)
{
    size_t i;
    int err         = 0;
    long byteOffset = *bitOffset / 8;
    int remainder   = *bitOffset % 8;
    unsigned char* p;
    int remainderComplement = 8 - remainder;
    const char* s           = string;

    Assert(numberOfCharacters < 512 && (numberOfCharacters == 0 || string));

    /* if (remainder) byteOffset++; */

    if (numberOfCharacters > 0) {
        unsigned char* restrict p = bitStream + byteOffset;
        if (remainder == 0) {
            memcpy(p, string, numberOfCharacters);
        }
        else {
            unsigned char mask  = (unsigned char)(0xFF00 >> remainder);
            unsigned char accum = p[0];
            for (size_t i = 0; i < numberOfCharacters; ++i) {
                accum |= (s[i] >> remainder) & ~mask;
                p[i]  = accum;
                accum = (s[i] << remainderComplement) & mask;
            }
            p[numberOfCharacters] = accum;
        }
    }
    *bitOffset += numberOfCharacters * 8;
    return err;
}

char* grib_decode_string(const unsigned char* bitStream, long* bitOffset, size_t numberOfCharacters, char* string)
{
    size_t i;
    long byteOffset = *bitOffset / 8;
    int remainder   = *bitOffset % 8;
    unsigned char c;
    unsigned char* p;
    char* s                 = string;
    unsigned char mask[]    = { 0, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    int remainderComplement = 8 - remainder;

    if (numberOfCharacters == 0)
        return string;

    p = (unsigned char*)bitStream + byteOffset;

    if (remainder == 0) {
        memcpy(string, bitStream + byteOffset, numberOfCharacters);
        *bitOffset += numberOfCharacters * 8;
        return string;
    }

    for (i = 0; i < numberOfCharacters; i++) {
        c = (*p) << remainder;
        p++;
        *s = (c | ((*p) & mask[remainder]) >> remainderComplement);
        s++;
    }
    *bitOffset += numberOfCharacters * 8;
    return string;
}

unsigned long grib_decode_unsigned_long(const unsigned char* p, long* bitp, long nbits)
{
    long countOfLeftmostBits = 0, leftmostBits = 0;
    long startBit      = *bitp;
    long remainingBits = nbits;
    long* pp           = (long*)p;
    unsigned long val  = 0;

    if (startBit >= max_nbits) {
        pp += startBit / max_nbits;
        startBit %= max_nbits;
    }

    countOfLeftmostBits = startBit + remainingBits;
    if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits = (VALUE(*pp, startBit, countOfLeftmostBits)) << remainingBits;
        startBit     = 0;
        pp++;
    }
    else
        leftmostBits = 0;

    val = leftmostBits + (VALUE(*pp, startBit, remainingBits));

    *bitp += nbits;

    return val;
}

size_t grib_decode_size_t(const unsigned char* p, long* bitp, long nbits)
{
    long countOfLeftmostBits = 0, leftmostBits = 0;
    long startBit      = *bitp;
    long remainingBits = nbits;
    long* pp           = (long*)p;
    size_t val  = 0;

    if (startBit >= max_nbits_size_t) {
        pp += startBit / max_nbits_size_t;
        startBit %= max_nbits_size_t;
    }

    countOfLeftmostBits = startBit + remainingBits;
    if (countOfLeftmostBits > max_nbits_size_t) {
        countOfLeftmostBits = max_nbits_size_t - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits = (VALUE_SIZE_T(*pp, startBit, countOfLeftmostBits)) << remainingBits;
        startBit     = 0;
        pp++;
    }
    else
        leftmostBits = 0;

    val = leftmostBits + (VALUE_SIZE_T(*pp, startBit, remainingBits));

    *bitp += nbits;

    return val;
}

int grib_encode_unsigned_long(unsigned char* p, unsigned long val, long* bitp, long nbits)
{
    long* destination        = (long*)p;
    long countOfLeftmostBits = 0, nextWord = 0, startBit = 0, remainingBits = 0, rightmostBits = 0;

    startBit      = *bitp;
    remainingBits = nbits;

    if (startBit >= max_nbits) {
        nextWord = startBit / max_nbits;
        startBit %= max_nbits;
    }
    else
        nextWord = 0;

    countOfLeftmostBits = startBit + remainingBits;
    if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit            = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        destination[nextWord] =
            ((destination[nextWord] >> countOfLeftmostBits) << countOfLeftmostBits) + (VALUE(val, startBit, countOfLeftmostBits));
        startBit = 0;
        nextWord++;
    }

    rightmostBits = VALUE(val, max_nbits - remainingBits, remainingBits);
    destination[nextWord] =
        (destination[nextWord] & ~MASKVALUE(startBit, remainingBits)) + (rightmostBits << max_nbits - (remainingBits + startBit));

    *bitp += nbits;
    return GRIB_SUCCESS;
}

int grib_encode_unsigned_longb(unsigned char* p, unsigned long val, long* bitp, long nbits)
{
    return grib_encode_unsigned_long(p, val, bitp, nbits);
}

int grib_encode_size_t(unsigned char* p, size_t val, long* bitp, long nbits)
{
    long* destination        = (long*)p;
    long countOfLeftmostBits = 0, nextWord = 0, startBit = 0, remainingBits = 0, rightmostBits = 0;

    startBit      = *bitp;
    remainingBits = nbits;

    if (startBit >= max_nbits_size_t) {
        nextWord = startBit / max_nbits_size_t;
        startBit %= max_nbits_size_t;
    }
    else
        nextWord = 0;

    countOfLeftmostBits = startBit + remainingBits;
    if (countOfLeftmostBits > max_nbits_size_t) {
        countOfLeftmostBits = max_nbits_size_t - startBit;
        startBit            = max_nbits_size_t - remainingBits;
        remainingBits -= countOfLeftmostBits;
        destination[nextWord] =
            ((destination[nextWord] >> countOfLeftmostBits) << countOfLeftmostBits) + (VALUE_SIZE_T(val, startBit, countOfLeftmostBits));
        startBit = 0;
        nextWord++;
    }

    rightmostBits = VALUE_SIZE_T(val, max_nbits_size_t - remainingBits, remainingBits);
    destination[nextWord] =
        (destination[nextWord] & ~MASKVALUE_SIZE_T(startBit, remainingBits)) + (rightmostBits << max_nbits_size_t - (remainingBits + startBit));

    *bitp += nbits;
    return GRIB_SUCCESS;
}

int grib_encode_size_tb(unsigned char* p, size_t val, long* bitp, long nbits)
{
    return grib_encode_size_t(p, val, bitp, nbits);
}


#if VECTOR
#include "grib_bits_fast_big_endian_vector.c" /* Experimental */
#elif OMP
#include "grib_bits_fast_big_endian_omp.c"
#else
#include "grib_bits_fast_big_endian_simple.c"
#endif
