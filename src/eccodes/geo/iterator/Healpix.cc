/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = iterator
   SUPER      = grib_iterator_class_gen
   IMPLEMENTS = destroy
   IMPLEMENTS = init;next
   MEMBERS     =   double *lats
   MEMBERS     =   double *lons
   MEMBERS     =   long Nsides
   MEMBERS     =   bool nested
   END_CLASS_DEF
*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "iterator.class" and rerun ./make_class.pl

*/


static void init_class              (grib_iterator_class*);

static int init               (grib_iterator* i,grib_handle*,grib_arguments*);
static int next               (grib_iterator* i, double *lat, double *lon, double *val);
static int destroy            (grib_iterator* i);


typedef struct grib_iterator_healpix{
  grib_iterator it;
    /* Members defined in gen */
    int carg;
    const char* missingValue;
    /* Members defined in healpix */
    double *lats;
    double *lons;
    long Nsides;
    bool nested;
} grib_iterator_healpix;

extern grib_iterator_class* grib_iterator_class_gen;

static grib_iterator_class _grib_iterator_class_healpix = {
    &grib_iterator_class_gen,                    /* super                     */
    "healpix",                    /* name                      */
    sizeof(grib_iterator_healpix),/* size of instance          */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                     /* constructor               */
    &destroy,                  /* destructor                */
    &next,                     /* Next Value                */
    0,                 /*  Previous Value           */
    0,                    /* Reset the counter         */
    0,                 /* has next values           */
};

grib_iterator_class* grib_iterator_class_healpix = &_grib_iterator_class_healpix;


static void init_class(grib_iterator_class* c)
{
    c->previous = (*(c->super))->previous;
    c->reset    = (*(c->super))->reset;
    c->has_next = (*(c->super))->has_next;
}
/* END_CLASS_IMP */

#define ITER "HEALPix Geoiterator"
constexpr double RAD2DEG = 57.29577951308232087684;  // 180 over pi

namespace
{

struct CodecFijNest
{
    static constexpr uint64_t __masks[] = { 0x00000000ffffffff, 0x0000ffff0000ffff, 0x00ff00ff00ff00ff,
                                            0x0f0f0f0f0f0f0f0f, 0x3333333333333333, 0x5555555555555555 };

    inline static int nest_encode_bits(int n)
    {
        auto b = static_cast<uint64_t>(n) & __masks[0];
        b      = (b ^ (b << 16)) & __masks[1];
        b      = (b ^ (b << 8)) & __masks[2];
        b      = (b ^ (b << 4)) & __masks[3];
        b      = (b ^ (b << 2)) & __masks[4];
        b      = (b ^ (b << 1)) & __masks[5];
        return static_cast<int>(b);
    }

    // inline static int nest_decode_bits(int n)
    // {
    //     auto b = static_cast<uint64_t>(n) & __masks[5];
    //     b      = (b ^ (b >> 1)) & __masks[4];
    //     b      = (b ^ (b >> 2)) & __masks[3];
    //     b      = (b ^ (b >> 4)) & __masks[2];
    //     b      = (b ^ (b >> 8)) & __masks[1];
    //     b      = (b ^ (b >> 16)) & __masks[0];
    //     return static_cast<int>(b);
    // }

    // static std::tuple<int, int, int> nest_to_fij(int n, int k)
    // {
    //     Assert(0 <= n);
    //     auto f = n >> (2 * k);    // f = n / (Nside * Nside)
    //     n &= (1 << (2 * k)) - 1;  // n = n % (Nside * Nside)
    //     auto i = nest_decode_bits(n);
    //     auto j = nest_decode_bits(n >> 1);
    //     return { f, i, j };
    // }

    static int fij_to_nest(int f, int i, int j, int k)
    {
        return (f << (2 * k)) + nest_encode_bits(i) + (nest_encode_bits(j) << 1);
    }
};

inline int int_sqrt(int n)
{
    return static_cast<int>(std::sqrt(static_cast<double>(n) + 0.5));
}

// for division result within [0; 3]
inline int div_03(int a, int b)
{
    int t = (a >= (b << 1)) ? 1 : 0;
    a -= t * (b << 1);
    return (t << 1) + (a >= b ? 1 : 0);
}

inline bool is_power_of_2(int n)
{
    return std::bitset<sizeof(int) * 8>(n).count() == 1;
}

inline int pll(int f)
{
    constexpr int __pll[] = { 1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7 };
    return __pll[f];
}

size_t HEALPix_nj(size_t N, size_t i)
{
    Assert(0 < N);
    size_t ni = 4 * N - 1;
    Assert(i < ni);
    return i < N ? 4 * (i + 1) : i < 3 * N ? 4 * N
                                           : HEALPix_nj(N, ni - 1 - i);
}

// Thanks to Willem Deconinck and Pedro Maciel
//
// y[] = { y0, y1, y2, y3, ... };   // the latitude values
// PL[] = { 4, ...  } ; // the number of values on each latitude
// xstart[] = { 45, ... }; // the value of first longitude on each latitude
// assume that you have 360 degrees to cover on each latitude
//
//    std::vector<double> xstart(4 * N - 1);
//    std::vector<int> pl(4 * N - 1);
//    // Polar caps
//    for (int r = 1; r < N; r++) {
//        xstart[r-1] = 45./r;
//        pl[r-1] = 4*r;
//        xstart[4*N-r-1] = xstart[r-1];
//        pl[4*N-r-1] = pl[r-1];
//    }
//    // Equatorial belt
//    const double start = 45. / N;
//    for (int r = N; r < 2 * N; r++) {
//        xstart[r-1] = start * (2. - (r - N + 1) % 2);
//        pl[r-1] = 4*N;
//        xstart[4*N-r-1] = xstart[r-1];
//        pl[4*N-r-1] = pl[r-1];
//    }
//    // Equator
//    xstart[2*N-1] = start * (1 - (N % 2 ? 1 : 0));
//    pl[2*N-1] = 4*N;
//
static std::vector<double> HEALPix_longitudes(size_t N, size_t i)
{
    const auto Nj    = HEALPix_nj(N, i);
    const auto step  = 360. / static_cast<double>(Nj);
    const auto start = i < N || 3 * N - 1 < i || static_cast<bool>((i + N) % 2) ? step / 2. : 0.;

    std::vector<double> longitudes(Nj);
    std::generate_n(longitudes.begin(), Nj, [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return longitudes;
}

static int iterate_healpix(grib_iterator_healpix* self, long N)
{
    size_t Ny = 4 * static_cast<size_t>(N) - 1;
    auto Nd   = static_cast<double>(N);

    std::vector<double> latitudes(Ny);

    for (long r = 1; r < N; r++) {
        auto rd                  = static_cast<double>(r);
        latitudes[r - 1]         = 90.0 - RAD2DEG * std::acos(1.0 - rd * rd / (3.0 * Nd * Nd));
        latitudes[4 * N - 1 - r] = -latitudes[r - 1];
    }

    // Polar caps
    for (long r = 1; r < N; r++) {
        auto rd                  = static_cast<double>(r);
        latitudes[r - 1]         = 90.0 - RAD2DEG * std::acos(1.0 - rd * rd / (3.0 * Nd * Nd));
        latitudes[4 * N - 1 - r] = -latitudes[r - 1];
    }

    // Equatorial belt
    for (long r = N; r < 2 * N; r++) {
        auto rd                  = static_cast<double>(r);
        latitudes[r - 1]         = 90.0 - RAD2DEG * std::acos((4.0 * Nd - 2.0 * rd) / (3.0 * Nd));
        latitudes[4 * N - 1 - r] = -latitudes[r - 1];
    }

    // Equator
    latitudes[2 * N - 1] = 0.0;

    if (self->nested) {
        if (!is_power_of_2(N)) {
            grib_context* c = grib_context_get_default();
            grib_context_log(c, GRIB_LOG_ERROR, "%s: For nested ordering, Nside must be a power of 2", ITER);
            return GRIB_WRONG_GRID;
        }

        const auto Nside = static_cast<int>(N);
        const auto k     = static_cast<int>(std::log2(Nside));
        const int Npix   = 12 * Nside * Nside;
        const int Ncap   = (Nside * (Nside - 1)) << 1;

        auto to_nest = [&](int f,      //!< base pixel index
                           int ring,   //!< 1-based ring number
                           int Nring,  //!< number of pixels in ring
                           int phi,    //!< index in longitude
                           int shift   //!< if ring's first pixel is not at phi=0
                           ) -> int {
            int r = ((2 + (f >> 2)) << k) - ring - 1;
            int p = 2 * phi - pll(f) * Nring - shift - 1;
            if (p >= 2 * Nside) {
                p -= 8 * Nside;
            }

            int i = std::max(0, (r + p)) >> 1;
            int j = std::max(0, (r - p)) >> 1;

            Assert(f < 12 && i < Nside && j < Nside);
            return CodecFijNest::fij_to_nest(f, i, j, k);
        };

        std::vector<size_t> ring_to_nest(Npix);
        for (int r = 0; r < Npix; ++r) {
            if (r < Ncap) {
                // North polar cap
                int Nring = (1 + int_sqrt(2 * r + 1)) >> 1;
                int phi   = 1 + r - 2 * Nring * (Nring - 1);
                int f     = div_03(phi - 1, Nring);

                ring_to_nest[r] = to_nest(f, Nring, Nring, phi, 0);
                continue;
            }

            if (Npix - Ncap <= r) {
                // South polar cap
                int Nring = (1 + int_sqrt(2 * Npix - 2 * r - 1)) >> 1;
                int phi   = 1 + r + 2 * Nring * (Nring - 1) + 4 * Nring - Npix;
                int ring  = 4 * Nside - Nring;  // (from South pole)
                int f     = div_03(phi - 1, Nring) + 8;

                ring_to_nest[r] = to_nest(f, ring, Nring, phi, 0);
                continue;
            }

            // Equatorial belt
            int ip  = r - Ncap;
            int tmp = ip >> (k + 2);

            int phi  = ip - tmp * 4 * Nside + 1;
            int ring = tmp + Nside;

            int ifm = 1 + ((phi - 1 - ((1 + tmp) >> 1)) >> k);
            int ifp = 1 + ((phi - 1 - ((1 - tmp + 2 * Nside) >> 1)) >> k);
            int f   = (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8));

            ring_to_nest[r] = to_nest(f, ring, Nside, phi, ring & 1);
        }

        for (size_t i = 0, j=0; i < Ny; i++) {
            // Compute the longitudes at a given latitude
            for (double longitude : HEALPix_longitudes(N, i)) {
                Assert( ring_to_nest.at(j) < Npix );
                self->lons[ring_to_nest.at(j)] = longitude;
                self->lats[ring_to_nest.at(j)] = latitudes[i];
                ++j;
            }
        }
    } else {
        for (size_t i = 0, j = 0; i < Ny; i++) {
            // Compute the longitudes at a given latitude
            for (double longitude : HEALPix_longitudes(N, i)) {
                self->lons[j] = longitude;
                self->lats[j] = latitudes[i];
                ++j;
            }
        }
    }

    return GRIB_SUCCESS;
}

}  // anonymous namespace

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int err    = 0;
    grib_iterator_healpix* self = (grib_iterator_healpix*)iter;

    const char* snside = grib_arguments_get_name(h, args, self->carg++);
    const char* sorder = grib_arguments_get_name(h, args, self->carg++);

    long N = 0;
    if ((err = grib_get_long_internal(h, snside, &N)) != GRIB_SUCCESS) {
        return err;
    }
    if (N <= 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Key %s must be greater than zero", ITER, snside);
        return GRIB_WRONG_GRID;
    }

    char ordering[32] = {0,};
    size_t slen = sizeof(ordering);
    if ((err = grib_get_string_internal(h, sorder, ordering, &slen)) != GRIB_SUCCESS) {
        return err;
    }

    self->nested = STR_EQUAL(ordering, "nested");
    if (!STR_EQUAL(ordering, "ring") && !self->nested) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Only orderingConvention=(ring|nested) are supported", ITER);
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    // if (self->nested && N == 1) {
    //     grib_context_log(h->context, GRIB_LOG_ERROR, "%s: For orderingConvention=nested, N must be greater than 1", ITER);
    //     return GRIB_GEOCALCULUS_PROBLEM;
    // }

    if (grib_is_earth_oblate(h)) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Only spherical earth is supported", ITER);
        return GRIB_WRONG_GRID;
    }

    if (iter->nv != 12 * N * N) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Wrong number of points (%zu!=12x%ldx%ld)",
                         ITER, iter->nv, N, N);
        return GRIB_WRONG_GRID;
    }

    self->lats = (double*)grib_context_malloc(h->context, iter->nv * sizeof(double));
    if (self->lats == nullptr) {
        return GRIB_OUT_OF_MEMORY;
    }

    self->lons = (double*)grib_context_malloc(h->context, iter->nv * sizeof(double));
    if (self->lons == nullptr) {
        return GRIB_OUT_OF_MEMORY;
    }

    try {
        err = iterate_healpix(self, N);
    }
    catch (...) {
        return GRIB_INTERNAL_ERROR;
    }

    iter->e = -1;

    return err;
}

static int next(grib_iterator* iter, double* lat, double* lon, double* val)
{
    auto* self = (grib_iterator_healpix*)iter;

    if (iter->e >= static_cast<long>(iter->nv - 1)) {
        return 0;
    }

    iter->e++;

    *lat = self->lats[iter->e];
    *lon = self->lons[iter->e];
    if (val != nullptr && iter->data != nullptr) {
        *val = iter->data[iter->e];
    }
    return 1;
}

static int destroy(grib_iterator* iter)
{
    auto* self    = (grib_iterator_healpix*)iter;
    const auto* c = iter->h->context;

    grib_context_free(c, self->lats);
    grib_context_free(c, self->lons);
    return GRIB_SUCCESS;
}
