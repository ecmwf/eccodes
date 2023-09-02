#include "ProjStringFuncs.h"
#include "AccessorUtils/AccessorProxy.h"
#include "grib_api_internal.h"

namespace eccodes::accessor {

#if 0

// -------------------------------------------------------------------------------------------

// Function pointer than takes a handle and returns the proj string
typedef int (*proj_func)(grib_handle*, char*);
struct proj_mapping
{
    const char* gridType; // key gridType
    proj_func func;       // function to compute proj string
};
typedef struct proj_mapping proj_mapping;

// This should only be called for GRID POINT data (not spherical harmonics etc)
GribStatus getMajorMinorAxes(grib_handle* h, double* pMajor, double* pMinor)
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

// Caller must have allocated enough space in the 'result' argument
static int get_earth_shape(grib_handle* h, char* result)
{
    int err      = 0;
    double major = 0, minor = 0;
    if ((err = get_major_minor_axes(h, &major, &minor)) != GRIB_SUCCESS)
        return err;
    if (major == minor)
        snprintf(result, 128, "+R=%lf", major); // spherical
    else
        snprintf(result, 128, "+a=%lf +b=%lf", major, minor); // oblate
    return err;
}

static int proj_space_view(grib_handle* h, char* result)
{
    return GRIB_NOT_IMPLEMENTED;
    //     int err        = 0;
    //     char shape[64] = {0,};
    //     double latOfSubSatellitePointInDegrees, lonOfSubSatellitePointInDegrees;
    //     if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
    //         return err;
    //     if ((err = grib_get_double_internal(h, "longitudeOfSubSatellitePointInDegrees", &lonOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
    //         return err;
    //     snprintf(result, 526, "+proj=geos +lon_0=%lf +h=35785831 +x_0=0 +y_0=0 %s", lonOfSubSatellitePointInDegrees, shape);
    //     return err;
    //     /* Experimental: For now do the same as gdalsrsinfo - hard coded values! */
    //     snprintf(result, 526, "+proj=geos +lon_0=0 +h=35785831 +x_0=0 +y_0=0 %s",  shape);
    //     return err;
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
    snprintf(result, 1024, "+proj=lcc +lon_0=%lf +lat_0=%lf +lat_1=%lf +lat_2=%lf %s",
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
    snprintf(result, 1024, "+proj=laea +lon_0=%lf +lat_0=%lf %s",
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
    snprintf(result, 1024, "+proj=stere +lat_ts=%lf +lat_0=%s +lon_0=%lf +k_0=1 +x_0=0 +y_0=0 %s",
            centralLatitude, has_northPole ? "90" : "-90", centralLongitude, shape);
    return err;
}

// ECC-1552: This is for regular_ll, regular_gg, reduced_ll, reduced_gg
//           These are not 'projected' grids!
static int proj_unprojected(grib_handle* h, char* result)
{
    int err = 0;
    //char shape[64] = {0,};
    //if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS) return err;
    //snprintf(result, 1024, "+proj=longlat %s", shape);
    snprintf(result, 1024, "+proj=longlat +datum=WGS84 +no_defs +type=crs");

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
    snprintf(result, 1024, "+proj=merc +lat_ts=%lf +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 %s",
            LaDInDegrees, shape);
    return err;
}

#define NUMBER(a) (sizeof(a) / sizeof(a[0]))
static proj_mapping proj_mappings[] = {
    { "regular_ll", &proj_unprojected },
    { "regular_gg", &proj_unprojected },
    { "reduced_ll", &proj_unprojected },
    { "reduced_gg", &proj_unprojected },

    { "mercator", &proj_mercator },
    { "lambert", &proj_lambert_conformal },
    { "polar_stereographic", &proj_polar_stereographic },
    { "lambert_azimuthal_equal_area", &proj_lambert_azimuthal_equal_area },
    { "space_view", &proj_space_view },
    { "albers", &proj_albers },
    { "transverse_mercator", &proj_transverse_mercator },
    { "equatorial_azimuthal_equidistant", &proj_equatorial_azimuthal_equidistant },
};

// -------------------------------------------------------------------------------------------

DataProjString::DataProjString(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // For now we just "know" which argument to extract
    int argIndex{0};
    gridType_ = AccessorName(std::get<std::string>(initData[argIndex++].second));
    endpoint_ = std::get<long>(initData[argIndex++].second);

    // These are private base members - consider a redesign...
    // length_   = 0;
    // a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    // a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

GribType DataProjString::nativeType() const
{
    return GribType::STRING;
}

constexpr static int endpointSource{0};
constexpr static int endpointTarget{1};
GribStatus DataProjString::unpack(std::vector<char> &values) const
{
//static int unpack_string(grib_accessor* a, char* v, size_t* len)

    int err = 0, found = 0;
    size_t i           = 0;
    char grid_type[64] = {0,};
//    grib_handle* h = grib_handle_of_accessor(a);
    size_t size    = sizeof(grid_type) / sizeof(*grid_type);

    Assert(endpoint_ == endpointSource || endpoint_ == endpointTarget);

    auto gridTypeAccessor = AccessorStore::instance().getAccessor(gridType_);
    auto gridTypeString = gridTypeAccessor->unpack<char>();

    for (i = 0; !found && i < NUMBER(proj_mappings); ++i) {
        proj_mapping pm = proj_mappings[i];
        if (strcmp(grid_type, pm.gridType) == 0) {
            found = 1;
            if (self->endpoint == endpointSource) {
                snprintf(v, 64, "EPSG:4326");
            }
            else {
                // Invoke the appropriate function to get the target proj string
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

#endif

}

