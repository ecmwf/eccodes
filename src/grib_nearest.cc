/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/**
* Author: Enrico Fucile
* date: 31/10/2007
*
*/

#include "grib_api_internal.h"

/* Note: The 'values' argument can be NULL in which case the data section will not be decoded
 * See ECC-499
 */
int grib_nearest_find(
    grib_nearest* nearest, const grib_handle* ch,
    double inlat, double inlon,
    unsigned long flags,
    double* outlats, double* outlons,
    double* values, double* distances, int* indexes, size_t* len)
{
    grib_handle* h        = (grib_handle*)ch;
    grib_nearest_class* c = NULL;
    if (!nearest)
        return GRIB_INVALID_ARGUMENT;
    c = nearest->cclass;
    Assert(flags <= (GRIB_NEAREST_SAME_GRID | GRIB_NEAREST_SAME_DATA | GRIB_NEAREST_SAME_POINT));

    while (c) {
        grib_nearest_class* s = c->super ? *(c->super) : NULL;
        if (c->find) {
            int ret = c->find(nearest, h, inlat, inlon, flags, outlats, outlons, values, distances, indexes, len);
            if (ret != GRIB_SUCCESS) {
                if (inlon > 0)
                    inlon -= 360;
                else
                    inlon += 360;
                ret = c->find(nearest, h, inlat, inlon, flags, outlats, outlons, values, distances, indexes, len);
            }
            return ret;
        }
        c = s;
    }
    Assert(0);
    return 0;
}

/* For this one, ALL init are called */
static int init_nearest(grib_nearest_class* c, grib_nearest* i, grib_handle* h, grib_arguments* args)
{
    if (c) {
        int ret               = GRIB_SUCCESS;
        grib_nearest_class* s = c->super ? *(c->super) : NULL;
        if (!c->inited) {
            if (c->init_class)
                c->init_class(c);
            c->inited = 1;
        }
        if (s)
            ret = init_nearest(s, i, h, args);

        if (ret != GRIB_SUCCESS)
            return ret;

        if (c->init)
            return c->init(i, h, args);
    }
    return GRIB_INTERNAL_ERROR;
}

int grib_nearest_init(grib_nearest* i, grib_handle* h, grib_arguments* args)
{
    return init_nearest(i->cclass, i, h, args);
}

/* For this one, ALL destroy are called */

int grib_nearest_delete(grib_nearest* i)
{
    grib_nearest_class* c = NULL;
    if (!i)
        return GRIB_INVALID_ARGUMENT;
    c = i->cclass;
    while (c) {
        grib_nearest_class* s = c->super ? *(c->super) : NULL;
        if (c->destroy)
            c->destroy(i);
        c = s;
    }
    return 0;
}

/* Get the radius in kilometres for nearest neighbour distance calculations */
/* For an ellipsoid, approximate the radius using the average of the semimajor and semiminor axes */
int grib_nearest_get_radius(grib_handle* h, double* radiusInKm)
{
    int err = 0;
    long lRadiusInMetres;
    double result = 0;
    const char* s_radius = "radius";
    const char* s_minor = "earthMinorAxisInMetres";
    const char* s_major = "earthMajorAxisInMetres";

    if ((err = grib_get_long(h, s_radius, &lRadiusInMetres)) == GRIB_SUCCESS) {
        if (grib_is_missing(h, s_radius, &err) || lRadiusInMetres == GRIB_MISSING_LONG) {
            grib_context_log(h->context, GRIB_LOG_DEBUG, "Key 'radius' is missing");
            return GRIB_GEOCALCULUS_PROBLEM;
        }
        result = ((double)lRadiusInMetres) / 1000.0;
    }
    else {
        double minor = 0, major = 0;
        if ((err = grib_get_double_internal(h, s_minor, &minor)) != GRIB_SUCCESS) return err;
        if ((err = grib_get_double_internal(h, s_major, &major)) != GRIB_SUCCESS) return err;
        if (grib_is_missing(h, s_minor, &err)) return GRIB_GEOCALCULUS_PROBLEM;
        if (grib_is_missing(h, s_major, &err)) return GRIB_GEOCALCULUS_PROBLEM;
        result = (major + minor) / 2.0;
        result = result / 1000.0;
    }
    *radiusInKm = result;
    return GRIB_SUCCESS;
}

/* Note: the argument 'n' is NOT the size of the 'xx' array but its LAST index i.e. size of xx - 1 */
void grib_binary_search(const double xx[], const size_t n, double x, size_t* ju, size_t* jl)
{
    size_t jm     = 0;
    int ascending = 0;
    *jl           = 0;
    *ju           = n;
    ascending     = (xx[n] >= xx[0]);
    while (*ju - *jl > 1) {
        jm = (*ju + *jl) >> 1;
        if ((x >= xx[jm]) == ascending)
            *jl = jm;
        else
            *ju = jm;
    }
}

int grib_nearest_find_multiple(
    const grib_handle* h, int is_lsm,
    const double* inlats, const double* inlons, long npoints,
    double* outlats, double* outlons,
    double* values, double* distances, int* indexes)
{
    grib_nearest* nearest = 0;
    double* pdistances    = distances;
    double* poutlats      = outlats;
    double* poutlons      = outlons;
    double* pvalues       = values;
    int* pindexes         = indexes;
    int idx = 0, ii = 0;
    double max, min;
    double qdistances[4] = {0,};
    double qoutlats[4] = {0,};
    double qoutlons[4] = {0,};
    double qvalues[4] = {0,};
    double* rvalues = NULL;
    int qindexes[4] = {0,};
    int ret    = 0;
    long i     = 0;
    size_t len = 4;
    const unsigned long flags  = GRIB_NEAREST_SAME_GRID | GRIB_NEAREST_SAME_DATA;

    if (values)
        rvalues = qvalues;

    nearest = grib_nearest_new(h, &ret);
    if (ret != GRIB_SUCCESS)
        return ret;

    if (is_lsm) {
        int noland = 1;
        /* ECC-499: In land-sea mask mode, 'values' cannot be NULL because we need to query whether >= 0.5 */
        Assert(values);
        for (i = 0; i < npoints; i++) {
            ret = grib_nearest_find(nearest, h, inlats[i], inlons[i], flags, qoutlats, qoutlons,
                                    qvalues, qdistances, qindexes, &len);
            max = qdistances[0];
            for (ii = 0; ii < 4; ii++) {
                if (max < qdistances[ii]) {
                    max = qdistances[ii];
                    idx = ii;
                }
                if (qvalues[ii] >= 0.5)
                    noland = 0;
            }
            min = max;
            for (ii = 0; ii < 4; ii++) {
                if ((min >= qdistances[ii]) && (noland || (qvalues[ii] >= 0.5))) {
                    min = qdistances[ii];
                    idx = ii;
                }
            }
            *poutlats = qoutlats[idx];
            poutlats++;
            *poutlons = qoutlons[idx];
            poutlons++;
            *pvalues = qvalues[idx];
            pvalues++;
            *pdistances = qdistances[idx];
            pdistances++;
            *pindexes = qindexes[idx];
            pindexes++;
        }
    }
    else {
        /* ECC-499: 'values' can be NULL */
        for (i = 0; i < npoints; i++) {
            ret = grib_nearest_find(nearest, h, inlats[i], inlons[i], flags, qoutlats, qoutlons,
                                    rvalues, qdistances, qindexes, &len);
            min = qdistances[0];
            for (ii = 0; ii < 4; ii++) {
                if ((min >= qdistances[ii])) {
                    min = qdistances[ii];
                    idx = ii;
                }
            }
            *poutlats = qoutlats[idx];
            poutlats++;
            *poutlons = qoutlons[idx];
            poutlons++;
            if (values) {
                *pvalues = qvalues[idx];
                pvalues++;
            }
            *pdistances = qdistances[idx];
            pdistances++;
            *pindexes = qindexes[idx];
            pindexes++;
        }
    }

    grib_nearest_delete(nearest);

    return ret;
}


/* Generic implementation of nearest for Lambert, Polar stereo, Mercator etc */
static int compare_doubles(const void* a, const void* b, int ascending)
{
    /* ascending is a boolean: 0 or 1 */
    double* arg1 = (double*)a;
    double* arg2 = (double*)b;
    if (ascending) {
        if (*arg1 < *arg2)
            return -1; /*Smaller values come before larger ones*/
    }
    else {
        if (*arg1 > *arg2)
            return -1; /*Larger values come before smaller ones*/
    }
    if (*arg1 == *arg2)
        return 0;
    else
        return 1;
}

static int compare_doubles_ascending(const void* a, const void* b)
{
    return compare_doubles(a, b, 1);
}

typedef struct PointStore
{
    double m_lat;
    double m_lon;
    double m_dist;
    double m_value;
    int m_index;
} PointStore;

/* Comparison function to sort points by distance */
static int compare_points(const void* a, const void* b)
{
    PointStore* pA = (PointStore*)a;
    PointStore* pB = (PointStore*)b;

    if (pA->m_dist < pB->m_dist) return -1;
    if (pA->m_dist > pB->m_dist) return 1;
    return 0;
}

int grib_nearest_find_generic(
    grib_nearest* nearest, grib_handle* h,
    double inlat, double inlon, unsigned long flags,

    const char* values_keyname,
    double** out_lats,
    int* out_lats_count,
    double** out_lons,
    int* out_lons_count,
    double** out_distances,

    double* outlats, double* outlons,
    double* values, double* distances, int* indexes, size_t* len)
{
    int ret = 0;
    size_t i = 0, nvalues = 0, nneighbours = 0;
    double radiusInKm;
    grib_iterator* iter = NULL;
    double lat = 0, lon = 0;

    /* array of candidates for nearest neighbours */
    PointStore* neighbours = NULL;

    inlon = normalise_longitude_in_degrees(inlon);

    if ((ret = grib_get_size(h, values_keyname, &nvalues)) != GRIB_SUCCESS)
        return ret;
    nearest->values_count = nvalues;

    if ((ret = grib_nearest_get_radius(h, &radiusInKm)) != GRIB_SUCCESS)
        return ret;

    neighbours = (PointStore*)grib_context_malloc(nearest->context, nvalues * sizeof(PointStore));
    for (i = 0; i < nvalues; ++i) {
        neighbours[i].m_dist  = 1e10; /* set all distances to large number to begin with */
        neighbours[i].m_lat   = 0;
        neighbours[i].m_lon   = 0;
        neighbours[i].m_value = 0;
        neighbours[i].m_index = 0;
    }

    /* GRIB_NEAREST_SAME_GRID not yet implemented */
    {
        double the_value = 0;
        double min_dist  = 1e10;
        size_t the_index = 0;
        int ilat = 0, ilon = 0;
        size_t idx_upper = 0, idx_lower = 0;
        double lat1 = 0, lat2 = 0;     /* inlat will be between these */
        const double LAT_DELTA = 10.0; /* in degrees */

        *out_lons_count = nvalues; /* Maybe overestimate but safe */
        *out_lats_count = nvalues;

        if (*out_lats)
            grib_context_free(nearest->context, *out_lats);
        *out_lats = (double*)grib_context_malloc(nearest->context, nvalues * sizeof(double));
        if (!*out_lats)
            return GRIB_OUT_OF_MEMORY;

        if (*out_lons)
            grib_context_free(nearest->context, *out_lons);
        *out_lons = (double*)grib_context_malloc(nearest->context, nvalues * sizeof(double));
        if (!*out_lons)
            return GRIB_OUT_OF_MEMORY;

        iter = grib_iterator_new(h, 0, &ret);
        if (ret) {
            free(neighbours);
            return ret;
        }
        /* First pass: collect all latitudes and longitudes */
        while (grib_iterator_next(iter, &lat, &lon, &the_value)) {
            ++the_index;
            Assert(ilat < *out_lats_count);
            Assert(ilon < *out_lons_count);
            (*out_lats)[ilat++] = lat;
            (*out_lons)[ilon++] = lon;
        }

        /* See between which 2 latitudes our point lies */
        qsort(*out_lats, nvalues, sizeof(double), &compare_doubles_ascending);
        grib_binary_search(*out_lats, *out_lats_count - 1, inlat, &idx_upper, &idx_lower);
        lat2 = (*out_lats)[idx_upper];
        lat1 = (*out_lats)[idx_lower];
        Assert(lat1 <= lat2);

        /* Second pass: Iterate again and collect candidate neighbours */
        grib_iterator_reset(iter);
        the_index = 0;
        i         = 0;
        while (grib_iterator_next(iter, &lat, &lon, &the_value)) {
            if (lat > lat2 + LAT_DELTA || lat < lat1 - LAT_DELTA) {
                /* Ignore latitudes too far from our point */
            }
            else {
                double dist = geographic_distance_spherical(radiusInKm, inlon, inlat, lon, lat);
                if (dist < min_dist)
                    min_dist = dist;
                /*printf("Candidate: lat=%.5f lon=%.5f dist=%f Idx=%ld Val=%f\n",lat,lon,dist,the_index,the_value);*/
                /* store this candidate point */
                neighbours[i].m_dist  = dist;
                neighbours[i].m_index = (int)the_index;
                neighbours[i].m_lat   = lat;
                neighbours[i].m_lon   = lon;
                neighbours[i].m_value = the_value;
                i++;
            }
            ++the_index;
        }
        nneighbours = i;
        /* Sort the candidate neighbours in ascending order of distance */
        /* The first 4 entries will now be the closest 4 neighbours */
        qsort(neighbours, nneighbours, sizeof(PointStore), &compare_points);

        grib_iterator_delete(iter);
    }
    nearest->h = h;

    /* Sanity check for sorting */
#ifdef DEBUG
    for (i = 0; i < nneighbours - 1; ++i) {
        Assert(neighbours[i].m_dist <= neighbours[i + 1].m_dist);
    }
#endif

    /* GRIB_NEAREST_SAME_XXX not yet implemented */
    if (!*out_distances) {
        *out_distances = (double*)grib_context_malloc(nearest->context, 4 * sizeof(double));
    }
    (*out_distances)[0] = neighbours[0].m_dist;
    (*out_distances)[1] = neighbours[1].m_dist;
    (*out_distances)[2] = neighbours[2].m_dist;
    (*out_distances)[3] = neighbours[3].m_dist;

    for (i = 0; i < 4; ++i) {
        distances[i] = neighbours[i].m_dist;
        outlats[i]   = neighbours[i].m_lat;
        outlons[i]   = neighbours[i].m_lon;
        indexes[i]   = neighbours[i].m_index;
        if (values) {
            values[i] = neighbours[i].m_value;
        }
        /*printf("(%f,%f)  i=%d  d=%f  v=%f\n",outlats[i],outlons[i],indexes[i],distances[i],values[i]);*/
    }

    free(neighbours);
    return GRIB_SUCCESS;
}
