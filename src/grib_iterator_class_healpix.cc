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
#include <cmath>
#include <vector>
#include <algorithm>

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
    c->previous    =    (*(c->super))->previous;
    c->reset    =    (*(c->super))->reset;
    c->has_next    =    (*(c->super))->has_next;
}
/* END_CLASS_IMP */

#define ITER "HEALPix Geoiterator"
#define RAD2DEG 57.29577951308232087684 // 180 over pi

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
    size_t ny, nx;
    ny = nx = 4*N - 1;
    std::vector<double> latitudes(ny);

    for (long r = 1; r < N; r++) {
        latitudes[r - 1]         = 90.0 - RAD2DEG * std::acos(1.0 - r * r / (3.0 * N * N));
        latitudes[4 * N - 1 - r] = -latitudes[r - 1];
    }
    // Polar caps
    for (long r = 1; r < N; r++) {
        latitudes[r - 1]         = 90.0 - RAD2DEG * std::acos(1.0 - r * r / (3.0 * N * N));
        latitudes[4 * N - 1 - r] = -latitudes[r - 1];
    }
    // Equatorial belt
    for (long r = N; r < 2 * N; r++) {
        latitudes[r - 1]         = 90.0 - RAD2DEG * std::acos((4.0 * N - 2.0 * r) / (3.0 * N));
        latitudes[4 * N - 1 - r] = -latitudes[r - 1];
    }

    // Equator
    latitudes[2 * N - 1] = 0.0;

    size_t k = 0;
    for (size_t i = 0; i < ny; i++) {
        // Compute the longitudes at a given latitude
        std::vector<double> longitudes = HEALPix_longitudes(N, i);
        for (size_t j = 0; j < longitudes.size(); j++) {
            self->lons[k] = longitudes[j];
            self->lats[k] = latitudes[i];
            ++k;
        }
    }

    return GRIB_SUCCESS;
}

static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args)
{
    int err = 0;
    grib_iterator_healpix* self = (grib_iterator_healpix*)iter;

    const char* snside = grib_arguments_get_name(h, args, self->carg++);
    const char* sorder = grib_arguments_get_name(h, args, self->carg++);

    long N = 0;
    if ((err = grib_get_long_internal(h, snside, &N)) != GRIB_SUCCESS) return err;
    if (N <= 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Key %s must be greater than zero", ITER, snside);
        return GRIB_WRONG_GRID;
    }

    char ordering[32] = {0,};
    size_t slen = sizeof(ordering);
    if ((err = grib_get_string_internal(h, sorder, ordering, &slen)) != GRIB_SUCCESS)
        return err;

    if (!STR_EQUAL(ordering, "ring")) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Only ring ordering is supported", ITER);
        return GRIB_WRONG_GRID;
    }

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
    if (self->lats == NULL) return GRIB_OUT_OF_MEMORY;
    self->lons = (double*)grib_context_malloc(h->context, iter->nv * sizeof(double));
    if (self->lons == NULL) return GRIB_OUT_OF_MEMORY;

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
    grib_iterator_healpix* self = (grib_iterator_healpix*)iter;

    if ((long)iter->e >= (long)(iter->nv - 1))
        return 0;
    iter->e++;

    *lat = self->lats[iter->e];
    *lon = self->lons[iter->e];
    if (val && iter->data) {
        *val = iter->data[iter->e];
    }
    return 1;
}

static int destroy(grib_iterator* i)
{
    grib_iterator_healpix* self = (grib_iterator_healpix*)i;
    const grib_context* c       = i->h->context;

    grib_context_free(c, self->lats);
    grib_context_free(c, self->lons);
    return GRIB_SUCCESS;
}
