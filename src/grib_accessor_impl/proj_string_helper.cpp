#include "proj_string_helper.h"
#include "grib_api_internal.h"

#include <array>
#include <algorithm>
#include <cstring>

namespace eccodes {  

    // This should only be called for GRID POINT data (not spherical harmonics etc)
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
    #if 0
        int err        = 0;
        char shape[64] = {0,};
        double latOfSubSatellitePointInDegrees, lonOfSubSatellitePointInDegrees;

        if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS)
            return err;

        if ((err = grib_get_double_internal(h, "longitudeOfSubSatellitePointInDegrees", &lonOfSubSatellitePointInDegrees)) != GRIB_SUCCESS)
            return err;

        snprintf(result, 526, "+proj=geos +lon_0=%lf +h=35785831 +x_0=0 +y_0=0 %s", lonOfSubSatellitePointInDegrees, shape);
        return err;

        /* Experimental: For now do the same as gdalsrsinfo - hard coded values! */
        snprintf(result, 526, "+proj=geos +lon_0=0 +h=35785831 +x_0=0 +y_0=0 %s",  shape);
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

    struct proj_mapping
    {
        const char* gridType; // key gridType
        proj_func func;       // function to compute proj string
    };
    typedef struct proj_mapping proj_mapping;

    static std::array proj_mappings = {
        proj_mapping{ "regular_ll", &proj_unprojected },
        proj_mapping{ "regular_gg", &proj_unprojected },
        proj_mapping{ "reduced_ll", &proj_unprojected },
        proj_mapping{ "reduced_gg", &proj_unprojected },
        proj_mapping{ "mercator", &proj_mercator },
        proj_mapping{ "lambert", &proj_lambert_conformal },
        proj_mapping{ "polar_stereographic", &proj_polar_stereographic },
        proj_mapping{ "lambert_azimuthal_equal_area", &proj_lambert_azimuthal_equal_area },
        proj_mapping{ "space_view", &proj_space_view },
        proj_mapping{ "albers", &proj_albers },
        proj_mapping{ "transverse_mercator", &proj_transverse_mercator },
        proj_mapping{ "equatorial_azimuthal_equidistant", &proj_equatorial_azimuthal_equidistant },
    };

    int get_proj_func(const char* grid_type, proj_func &func)
    {
        if(auto it = std::find_if(std::begin(proj_mappings), std::end(proj_mappings),
                                  [grid_type](const proj_mapping& pm){ return strcmp(grid_type, pm.gridType) == 0; });
                it != proj_mappings.end()) {
            func = it->func;
            return GRIB_SUCCESS;
        }

        return GRIB_NOT_FOUND;
    }

}
