/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


#ifdef __PGI
#undef __AVX__
#undef __SSE2__
#endif


#ifdef _GET_IBM_COUNTER
#include <libhpc.h>
#endif

#ifdef __AVX__
#include <float.h>
#include <stdint.h>
#include <inttypes.h>
#include <immintrin.h>
#ifdef _GET_X86_COUNTER
#include <x86intrin.h>
#endif
#else
#ifdef __SSE2__
#include <float.h>
#include <stdint.h>
#include <inttypes.h>
#include <emmintrin.h>
#ifdef _GET_X86_COUNTER
#include <x86intrin.h>
#endif
#endif
#endif


#ifdef __AVX__
static
void avx_minmax_val(const double *restrict buf, long nframes, double *min, double *max)
{
    __m256d current_max, current_min, work;

    /* load max and min values into all four slots of the XMM registers */
    current_min = _mm256_set1_pd(*min);
    current_max = _mm256_set1_pd(*max);

    /* Work input until "buf" reaches 32 byte alignment */
    while ( ((unsigned long)buf) % 32 != 0 && nframes > 0) {

        /* Load the next double into the work buffer */
        work = _mm256_set1_pd(*buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf++;
        nframes--;
    }

    while (nframes >= 16) {
        /* use 64 byte prefetch? */
        __builtin_prefetch(buf+64,0,0); /* for GCC 4.3.2+ */

        work = _mm256_load_pd(buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf += 4;
        __builtin_prefetch(buf+64,0,0); /* for GCC 4.3.2+ */
        work = _mm256_load_pd(buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf += 4;
        __builtin_prefetch(buf+64,0,0); /* for GCC 4.3.2+ */
        work = _mm256_load_pd(buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf += 4;
        __builtin_prefetch(buf+64,0,0); /* for GCC 4.3.2+ */
        work = _mm256_load_pd(buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf += 4;
        nframes -= 16;
    }

    /* work through aligned buffers */
    while (nframes >= 4) {
        work = _mm256_load_pd(buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf += 4;
        nframes -= 4;
    }

    /* work through the remainung values */
    while ( nframes > 0) {
        work = _mm256_set1_pd(*buf);
        current_min = _mm256_min_pd(current_min, work);
        current_max = _mm256_max_pd(current_max, work);
        buf++;
        nframes--;
    }

    /* find min & max value through shuffle tricks */

    work = current_min;
    work = _mm256_shuffle_pd(work, work, _MM_SHUFFLE(2, 3, 0, 1));
    work = _mm256_min_pd (work, current_min);
    current_min = work;
    work = _mm256_shuffle_pd(work, work, _MM_SHUFFLE(1, 0, 3, 2));
    work = _mm256_min_pd (work, current_min);
    _mm256_store_pd(min, work);
    work = current_max;
    work = _mm256_shuffle_pd(work, work, _MM_SHUFFLE(2, 3, 0, 1));
    work = _mm256_max_pd (work, current_max);
    current_max = work;
    work = _mm256_shuffle_pd(work, work, _MM_SHUFFLE(1, 0, 3, 2));
    work = _mm256_max_pd (work, current_max);
    _mm256_store_pd(max, work);

    return;
}

#else

#ifdef __SSE2__
static
void sse2_minmax_val(const double *restrict buf, long nframes, double *min, double *max)
{
    __m128d current_max, current_min, work;

    /* load starting max and min values into all slots of the XMM registers */
    current_min = _mm_set1_pd(*min);
    current_max = _mm_set1_pd(*max);

    /* work on input until buf reaches 16 byte alignment */
    while ( ((unsigned long)buf) % 16 != 0 && nframes > 0) {

        /* load one double and replicate */
        work = _mm_set1_pd(*buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf++;
        nframes--;
    }

    while (nframes >= 8) {
        /* use 64 byte prefetch for double octetts */
        __builtin_prefetch(buf+64,0,0); /* for GCC 4.3.2 + */

        work = _mm_load_pd(buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf += 2;
        work = _mm_load_pd(buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf += 2;
        work = _mm_load_pd(buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf += 2;
        work = _mm_load_pd(buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf += 2;
        nframes -= 8;
    }

    /* work through smaller chunks of aligned buffers without prefetching */
    while (nframes >= 2) {
        work = _mm_load_pd(buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf += 2;
        nframes -= 2;
    }

    /* work through the remaining value */
    while ( nframes > 0) {
        /* load the last double and replicate */
        work = _mm_set1_pd(*buf);
        current_min = _mm_min_pd(current_min, work);
        current_max = _mm_max_pd(current_max, work);
        buf++;
        nframes--;
    }

    /* find final min and max value through shuffle tricks */
    work = current_min;
    work = _mm_shuffle_pd(work, work, _MM_SHUFFLE2(0, 1));
    work = _mm_min_pd (work, current_min);
    _mm_store_sd(min, work);
    work = current_max;
    work = _mm_shuffle_pd(work, work, _MM_SHUFFLE2(0, 1));
    work = _mm_max_pd (work, current_max);
    _mm_store_sd(max, work);

    return;
}
#endif

#endif

static
void minmax_val(const double *restrict data, long datasize, double *fmin, double *fmax)
{
#ifdef _GET_X86_COUNTER 
    uint64_t start_minmax, end_minmax;
#endif

    if ( datasize < 1 ) return;

#ifdef _GET_X86_COUNTER 
    start_minmax = _rdtsc();
#endif

#ifdef __AVX__

    avx_minmax_val(data, datasize, fmin, fmax);

#else


#ifdef __SSE2__

    sse2_minmax_val(data, datasize, fmin, fmax);

#else

#ifdef _ARCH_PWR6
#define __UNROLL_DEPTH_1 6

    /* to allow pipelining we have to unroll */

#ifdef _GET_IBM_COUNTER 
    hpmStart(1, "minmax fsel");
#endif
    {
        long i, j;
        long residual =  datasize % __UNROLL_DEPTH_1;
        long ofs = datasize - residual;
        double register dmin[__UNROLL_DEPTH_1];
        double register dmax[__UNROLL_DEPTH_1];

        for ( j = 0; j < __UNROLL_DEPTH_1; j++)
        {
            dmin[j] = data[0];
            dmax[j] = data[0];
        }

        for ( i = 0; i < datasize - residual; i += __UNROLL_DEPTH_1 )
        {
            for (j = 0; j < __UNROLL_DEPTH_1; j++)
            {
                dmin[j] = __fsel(dmin[j] - data[i+j], data[i+j], dmin[j]);
                dmax[j] = __fsel(data[i+j] - dmax[j], data[i+j], dmax[j]);
            }
        }

        for (j = 0; j < residual; j++)
        {
            dmin[j] = __fsel(dmin[j] - data[ofs+j], data[ofs+j], dmin[j]);
            dmax[j] = __fsel(data[ofs+j] - dmax[j], data[ofs+j], dmax[j]);
        }

        for ( j = 0; j < __UNROLL_DEPTH_1; j++)
        {
            *fmin = __fsel(*fmin - dmin[j], dmin[j], *fmin);
            *fmax = __fsel(dmax[j] - *fmax, dmax[j], *fmax);
        }
    }
#ifdef _GET_IBM_COUNTER 
    hpmStop(1);
#endif

#undef __UNROLL_DEPTH_1
#else

#ifdef _GET_IBM_COUNTER 
    hpmStart(1, "minmax base");
#endif
    {
        long i;

#if   defined (CRAY)
#pragma _CRI ivdep
#elif defined (SX)
#pragma vdir nodep
#elif defined (__uxp__)
#pragma loop novrec
#endif
        for ( i = 0; i < datasize; ++i )
        {
            if ( *fmin > data[i] ) *fmin = data[i];
            if ( *fmax < data[i] ) *fmax = data[i];
            /*
             *fmin = *fmin < data[i] ? *fmin : data[i];
             *fmax = *fmax > data[i] ? *fmax : data[i];
             */
        }
    }
#ifdef _GET_IBM_COUNTER 
    hpmStop(1);
#endif

#endif
#endif
#endif

#ifdef _GET_X86_COUNTER 
    end_minmax = _rdtsc();
#ifdef __AVX__
    printf("AVX cycles:: %" PRIu64 "\n",
            end_minmax-start_minmax);
#else
#ifdef __SSE2__
    printf("SSE2 cycles:: %" PRIu64 "\n",
            end_minmax-start_minmax);
#else
    printf("loop cycles:: %" PRIu64 "\n",
            end_minmax-start_minmax);
#endif  
#endif
#endif

    return;
}
