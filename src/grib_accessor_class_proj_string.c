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
/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_gen
   IMPLEMENTS = get_native_type;init
   IMPLEMENTS = unpack_string
   MEMBERS = const char* grid_type
   MEMBERS = int endpoint
   END_CLASS_DEF

 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int unpack_string(grib_accessor*, char*, size_t* len);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_proj_string
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in proj_string */
    const char* grid_type;
    int endpoint;
} grib_accessor_proj_string;

extern grib_accessor_class* grib_accessor_class_gen;

static grib_accessor_class _grib_accessor_class_proj_string = {
    &grib_accessor_class_gen,                      /* super */
    "proj_string",                      /* name */
    sizeof(grib_accessor_proj_string),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* free mem */
    0,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    0,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    0,                  /* grib_pack procedures long */
    0,                /* grib_unpack procedures long */
    0,                /* grib_pack procedures double */
    0,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    &unpack_string,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_proj_string = &_grib_accessor_class_proj_string;


static void init_class(grib_accessor_class* c)
{
    c->dump    =    (*(c->super))->dump;
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->value_count    =    (*(c->super))->value_count;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_long    =    (*(c->super))->pack_long;
    c->unpack_long    =    (*(c->super))->unpack_long;
    c->pack_double    =    (*(c->super))->pack_double;
    c->unpack_double    =    (*(c->super))->unpack_double;
    c->pack_string    =    (*(c->super))->pack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long len, grib_arguments* arg)
{
    grib_accessor_proj_string* self = (grib_accessor_proj_string*)a;
    grib_handle* h                  = grib_handle_of_accessor(a);

    self->grid_type = grib_arguments_get_name(h, arg, 0);
    self->endpoint  = grib_arguments_get_long(h, arg, 1);
    a->length       = 0;
    a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

static int get_native_type(grib_accessor* a)
{
    return GRIB_TYPE_STRING;
}

/* Function pointer than takes a handle and returns the proj string */
typedef int (*proj_func)(grib_handle*, char*);
struct proj_mapping
{
    const char* gridType; /* key gridType */
    proj_func func;       /* function to compute proj string */
};
typedef struct proj_mapping proj_mapping;


/* This should only be called for GRID POINT data (not spherical harmonics etc) */
static int get_major_minor_axes(grib_handle* h, double* pMajor, double* pMinor)
{
    int err = 0;
    if (grib_is_earth_oblate(h)) {
        if ((err = grib_get_double_internal(h, "earthMinorAxisInMetres", pMinor)) != GRIB_SUCCESS) return err;
        if ((err = grib_get_double_internal(h, "earthMajorAxisInMetres", pMajor)) != GRIB_SUCCESS) return err;
    }
    else {
        double radius = 0;
        if ((err = grib_get_double_internal(h, "radius", &radius)) != GRIB_SUCCESS) return err;
        *pMajor = *pMinor = radius;
    }
    return err;
}

/* Caller must have allocated enough space in the 'result' argument */
static int get_earth_shape(grib_handle* h, char* result)
{
    int err      = 0;
    double major = 0, minor = 0;
    if ((err = get_major_minor_axes(h, &major, &minor)) != GRIB_SUCCESS)
        return err;
    if (major == minor)
        sprintf(result, "+R=%lf", major); /* spherical */
    else
        sprintf(result, "+a=%lf +b=%lf", major, minor); /*oblate*/
    return err;
}
#if 0
static int proj_regular_latlon(grib_handle* h, char* result)
{
    int err        = 0;
    char shape[64] = {0,};
    if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
        return err;
    sprintf(result, "+proj=latlong %s", shape);
    return err;
}
#endif

static int proj_space_view(grib_handle* h, char* result)
{
    return GRIB_NOT_IMPLEMENTED;
#if 0
    int err        = 0;
    char shape[64] = {0,};
    double latOfSubSatellitePointInDegrees, lonOfSubSatellitePointInDegrees;

    if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
        return err;

    if ((err = grib_get_double_internal(h, "longitudeOfSubSatellitePointInDegrees", &lonOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
        return err;

    sprintf(result, "+proj=geos +lon_0=%lf +h=35785831 +x_0=0 +y_0=0 %s", lonOfSubSatellitePointInDegrees, shape);
    return err;

    /* Experimental: For now do the same as gdalsrsinfo - hard coded values! */
    sprintf(result, "+proj=geos +lon_0=0 +h=35785831 +x_0=0 +y_0=0 %s",  shape);
    return err;
#endif
}
static int proj_albers(grib_handle* h, char* result)
{
    return GRIB_NOT_IMPLEMENTED;
}
static int proj_transverse_mercator(grib_handle* h, char* result)
{
    return GRIB_NOT_IMPLEMENTED;
}
static int proj_equatorial_azimuthal_equidistant(grib_handle* h, char* result)
{
    return GRIB_NOT_IMPLEMENTED;
}

static int proj_lambert_conformal(grib_handle* h, char* result)
{
    int err        = 0;
    char shape[64] = {0,};
    double LoVInDegrees = 0, LaDInDegrees = 0, Latin1InDegrees = 0, Latin2InDegrees = 0;

    if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "Latin1InDegrees", &Latin1InDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "Latin2InDegrees", &Latin2InDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "LoVInDegrees", &LoVInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "LaDInDegrees", &LaDInDegrees)) != GRIB_SUCCESS)
        return err;
    sprintf(result, "+proj=lcc +lon_0=%lf +lat_0=%lf +lat_1=%lf +lat_2=%lf %s",
            LoVInDegrees, LaDInDegrees, Latin1InDegrees, Latin2InDegrees, shape);
    return err;
}

static int proj_lambert_azimuthal_equal_area(grib_handle* h, char* result)
{
    int err        = 0;
    char shape[64] = {0,};
    double standardParallel = 0, centralLongitude = 0;

    if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "standardParallelInDegrees", &standardParallel)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "centralLongitudeInDegrees", &centralLongitude)) != GRIB_SUCCESS)
        return err;
    sprintf(result, "+proj=laea +lon_0=%lf +lat_0=%lf %s",
            centralLongitude, standardParallel, shape);
    return err;
}

static int proj_polar_stereographic(grib_handle* h, char* result)
{
    int err                 = 0;
    double centralLongitude = 0, centralLatitude = 0;
    int has_northPole         = 0;
    long projectionCentreFlag = 0;
    char shape[64]            = {0,};

    if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "orientationOfTheGridInDegrees", &centralLongitude)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_double_internal(h, "LaDInDegrees", &centralLatitude)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_get_long_internal(h, "projectionCentreFlag", &projectionCentreFlag)) != GRIB_SUCCESS)
        return err;
    has_northPole = ((projectionCentreFlag & 128) == 0);
    sprintf(result, "+proj=stere +lat_ts=%lf +lat_0=%s +lon_0=%lf +k_0=1 +x_0=0 +y_0=0 %s",
            centralLatitude, has_northPole ? "90" : "-90", centralLongitude, shape);
    return err;
}

static int proj_mercator(grib_handle* h, char* result)
{
    int err             = 0;
    double LaDInDegrees = 0;
    char shape[64]      = {0,};

    if ((err = grib_get_double_internal(h, "LaDInDegrees", &LaDInDegrees)) != GRIB_SUCCESS)
        return err;
    if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
        return err;
    sprintf(result, "+proj=merc +lat_ts=%lf +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 %s",
            LaDInDegrees, shape);
    return err;
}

#define NUMBER(a) (sizeof(a) / sizeof(a[0]))
static proj_mapping proj_mappings[] = {
    /*{ "regular_ll", &proj_regular_latlon },*/

    { "mercator", &proj_mercator },
    { "lambert", &proj_lambert_conformal },
    { "polar_stereographic", &proj_polar_stereographic },
    { "lambert_azimuthal_equal_area", &proj_lambert_azimuthal_equal_area },
    { "space_view", &proj_space_view },
    { "albers", &proj_albers },
    { "transverse_mercator", &proj_transverse_mercator },
    { "equatorial_azimuthal_equidistant", &proj_equatorial_azimuthal_equidistant },
};

#define ENDPOINT_SOURCE 0
#define ENDPOINT_TARGET 1
static int unpack_string(grib_accessor* a, char* v, size_t* len)
{
    grib_accessor_proj_string* self = (grib_accessor_proj_string*)a;
    int err = 0, found = 0;
    size_t i           = 0;
    char grid_type[64] = {0,};
    grib_handle* h = grib_handle_of_accessor(a);
    size_t size    = sizeof(grid_type) / sizeof(*grid_type);

    Assert(self->endpoint == ENDPOINT_SOURCE || self->endpoint == ENDPOINT_TARGET);

    err = grib_get_string(h, self->grid_type, grid_type, &size);
    if (err) return err;

    for (i = 0; !found && i < NUMBER(proj_mappings); ++i) {
        proj_mapping pm = proj_mappings[i];
        if (strcmp(grid_type, pm.gridType) == 0) {
            found = 1;
            if (self->endpoint == ENDPOINT_SOURCE) {
                sprintf(v, "EPSG:4326");
            }
            else {
                /* Invoke the appropriate function to get the target proj string */
                if ((err = pm.func(h, v)) != GRIB_SUCCESS) return err;
            }
        }
    }
    if (!found) {
        *len = 0;
        return GRIB_NOT_FOUND;
    }

    size = strlen(v);
    Assert(size > 0);
    *len = size + 1;
    return err;
}
