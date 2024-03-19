
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
#include "grib_accessor_class_bufr_extract_area_subsets.h"

grib_accessor_class_bufr_extract_area_subsets_t _grib_accessor_class_bufr_extract_area_subsets{"bufr_extract_area_subsets"};
grib_accessor_class* grib_accessor_class_bufr_extract_area_subsets = &_grib_accessor_class_bufr_extract_area_subsets;


void grib_accessor_class_bufr_extract_area_subsets_t::init(grib_accessor* a, const long len, grib_arguments* arg){
    grib_accessor_class_gen_t::init(a, len, arg);
    grib_accessor_bufr_extract_area_subsets_t* self = (grib_accessor_bufr_extract_area_subsets_t*)a;
    grib_handle* h = grib_handle_of_accessor(a);
    int n = 0;

    a->length                          = 0;
    self->doExtractSubsets             = grib_arguments_get_name(h, arg, n++);
    self->numberOfSubsets              = grib_arguments_get_name(h, arg, n++);
    self->extractSubsetList            = grib_arguments_get_name(h, arg, n++);
    self->extractAreaWestLongitude     = grib_arguments_get_name(h, arg, n++);
    self->extractAreaEastLongitude     = grib_arguments_get_name(h, arg, n++);
    self->extractAreaNorthLatitude     = grib_arguments_get_name(h, arg, n++);
    self->extractAreaSouthLatitude     = grib_arguments_get_name(h, arg, n++);
    self->extractAreaLongitudeRank     = grib_arguments_get_name(h, arg, n++);
    self->extractAreaLatitudeRank      = grib_arguments_get_name(h, arg, n++);
    self->extractedAreaNumberOfSubsets = grib_arguments_get_name(h, arg, n++);

    a->flags |= GRIB_ACCESSOR_FLAG_FUNCTION;
}

int grib_accessor_class_bufr_extract_area_subsets_t::get_native_type(grib_accessor* a){
    return GRIB_TYPE_LONG;
}

/* Copy first element of array into all others */
void fill_in(double a[], long length){
    long i;
    for (i = 1; i < length; ++i)
        a[i] = a[0];
}

int select_area(grib_accessor* a){
    grib_accessor_bufr_extract_area_subsets_t* self = (grib_accessor_bufr_extract_area_subsets_t*)a;

    int ret = 0;
    long compressed = 0;
    grib_handle* h = grib_handle_of_accessor(a);
    grib_context* c = h->context;

    double* lat = NULL;
    double* lon = NULL;
    size_t n;
    double lonWest, lonEast, latNorth, latSouth;
    long numberOfSubsets, i, latRank, lonRank;
    grib_iarray* subsets = NULL;
    size_t nsubsets  = 0;
    char latstr[32]  = {0,};
    char lonstr[32] = {0,};

    ret = grib_get_long(h, "compressedData", &compressed);
    if (ret) return ret;

    ret = grib_get_long(h, self->numberOfSubsets, &numberOfSubsets);
    if (ret) return ret;

    subsets = grib_iarray_new(c, numberOfSubsets, 10);

    ret = grib_set_long(h, "unpack", 1);
    if (ret) return ret;

    if (compressed) {
        ret = grib_get_long(h, self->extractAreaLongitudeRank, &lonRank);
        if (ret) return ret;
        snprintf(lonstr, sizeof(lonstr), "#%ld#longitude", lonRank);
        ret = grib_get_long(h, self->extractAreaLatitudeRank, &latRank);
        if (ret) return ret;
        snprintf(latstr, sizeof(latstr), "#%ld#latitude", latRank);
    }

    /* Latitudes */
    n   = numberOfSubsets;
    lat = (double*)grib_context_malloc_clear(c, sizeof(double) * numberOfSubsets);
    if (compressed) {
        ret = grib_get_double_array(h, latstr, lat, &n);
        if (ret) return ret;
        if (!(n == 1 || n == numberOfSubsets)) {
            /* n can be 1 if all latitudes are the same */
            return GRIB_INTERNAL_ERROR;
        }
        if (n == 1) {
            fill_in(lat, numberOfSubsets);
        }
    }
    else {
        size_t values_len = 0;
        for (i = 0; i < numberOfSubsets; ++i) {
            snprintf(latstr, sizeof(latstr), "#%ld#latitude", i + 1);
            ret = grib_get_size(h, latstr, &values_len);
            if (ret) return ret;
            if (values_len > 1) return GRIB_NOT_IMPLEMENTED;
            ret = grib_get_double(h, latstr, &(lat[i]));
            if (ret) return ret;
        }
    }

    /* Longitudes */
    n   = numberOfSubsets;
    lon = (double*)grib_context_malloc_clear(c, sizeof(double) * numberOfSubsets);
    if (compressed) {
        ret = grib_get_double_array(h, lonstr, lon, &n);
        if (ret) return ret;
        if (!(n == 1 || n == numberOfSubsets)) {
            /* n can be 1 if all longitudes are the same */
            return GRIB_INTERNAL_ERROR;
        }
        if (n == 1) {
            fill_in(lon, numberOfSubsets);
        }
    }
    else {
        size_t values_len = 0;
        for (i = 0; i < numberOfSubsets; ++i) {
            snprintf(lonstr, sizeof(lonstr), "#%ld#longitude", i + 1);
            ret = grib_get_size(h, lonstr, &values_len);
            if (ret) return ret;
            if (values_len > 1) return GRIB_NOT_IMPLEMENTED;
            ret = grib_get_double(h, lonstr, &(lon[i]));
            if (ret) return ret;
        }
    }

    ret = grib_get_double(h, self->extractAreaWestLongitude, &lonWest);
    if (ret) return ret;
    ret = grib_get_double(h, self->extractAreaEastLongitude, &lonEast);
    if (ret) return ret;
    ret = grib_get_double(h, self->extractAreaNorthLatitude, &latNorth);
    if (ret) return ret;
    ret = grib_get_double(h, self->extractAreaSouthLatitude, &latSouth);
    if (ret) return ret;

    for (i = 0; i < numberOfSubsets; i++) {
        /* printf("++++++ lat: %g <= %g <= %g lon: %g <= %g <= %g \n",latSouth,lat[i],latNorth,lonWest,lon[i],lonEast); */
        if (lat[i] >= latSouth && lat[i] <= latNorth && lon[i] >= lonWest && lon[i] <= lonEast) {
            grib_iarray_push(subsets, i + 1);
            /* printf("++++++++ %ld\n",i+1); */
        }
    }

    nsubsets = grib_iarray_used_size(subsets);
    ret      = grib_set_long(h, self->extractedAreaNumberOfSubsets, nsubsets);
    if (ret) return ret;

    if (nsubsets != 0) {
        long* subsets_ar = grib_iarray_get_array(subsets);
        ret        = grib_set_long_array(h, self->extractSubsetList, subsets_ar, nsubsets);
        grib_context_free(c, subsets_ar);
        if (ret) return ret;

        ret = grib_set_long(h, self->doExtractSubsets, 1);
        if (ret) return ret;
    }

    grib_context_free(c, lat);
    grib_context_free(c, lon);
    grib_iarray_delete(subsets);
    subsets = NULL;

    return ret;
}

int grib_accessor_class_bufr_extract_area_subsets_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    /*grib_accessor_bufr_extract_area_subsets_t *self =(grib_accessor_bufr_extract_area_subsets_t*)a;*/

    if (*len == 0)
        return GRIB_SUCCESS;
    return select_area(a);
}
