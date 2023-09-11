#include "ProjStringData.h"
#include "AccessorFactory.h"
#include "AccessorStore.h"
#include "Accessor.h"
#include "AccessorUtils/AccessorProxy.h"
#include "AccessorUtils/GribUtils.h"
#include "grib_api_internal.h"
#include <map>
#include <functional>
#include <string_view>

namespace eccodes::accessor {

// ProjString Helpers [BEGIN] ---------------------------------------------------------------------

// Function pointer than takes a handle and returns the proj string
//typedef GribStatus (*proj_func)(/* grib_handle*, */ char*);
//struct ProjMapping
//{
//    const char* gridType; // key gridType
//    proj_func func;       // function to compute proj string
//};

// This should only be called for GRID POINT data (not spherical harmonics etc)
GribStatus getMajorMinorAxes(/* grib_handle* h, */ double& major, double& minor)
{
    if (gribIsEarthOblate()) {
        minor = toDouble(AccessorName("earthMinorAxisInMetres"));
        major = toDouble(AccessorName("earthMajorAxisInMetres"));
    }
    else {
        double radius = 0;
        radius = toDouble(AccessorName("radius"));
        major = minor = radius;
    }
    return GribStatus::SUCCESS;
}

// Caller must have allocated enough space in the 'result' argument
GribStatus getEarthShape(/* grib_handle* h, */ std::string& result)
{
    GribStatus status{GribStatus::SUCCESS};
    double major = 0, minor = 0;
    if ((status = getMajorMinorAxes(major, minor)) != GribStatus::SUCCESS)
        return status;
    if (major == minor)
        //snprintf(result, 128, "+R=%lf", major); // spherical
        result = "+R=" + std::to_string(major);
    else
        //snprintf(result, 128, "+a=%lf +b=%lf", major, minor); // oblate
        result = "+a=" + std::to_string(major) + " +b=" + std::to_string(minor);
    return status;
}

GribStatus projSpaceView(/* grib_handle* h, */ std::string& result)
{
    return GribStatus::NOT_IMPLEMENTED;
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

GribStatus projAlbers(/* grib_handle* h, */ std::string& result)
{
    return GribStatus::NOT_IMPLEMENTED;
}
GribStatus projTransverseMercator(/* grib_handle* h, */ std::string& result)
{
    return GribStatus::NOT_IMPLEMENTED;
}
GribStatus projEquatorialAzimuthalEquidistant(/* grib_handle* h, */ std::string& result)
{
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus projLambertConformal(/* grib_handle* h, */ std::string& result)
{
    GribStatus status{GribStatus::SUCCESS};
    std::string shape;
    double LoVInDegrees = 0, LaDInDegrees = 0, Latin1InDegrees = 0, Latin2InDegrees = 0;

    if ((status = getEarthShape(shape)) != GribStatus::SUCCESS)
        return status;

    Latin1InDegrees = toDouble(AccessorName("Latin1InDegrees"));
    Latin2InDegrees = toDouble(AccessorName("Latin2InDegrees"));
    LoVInDegrees = toDouble(AccessorName("LoVInDegrees"));
    LaDInDegrees = toDouble(AccessorName("LaDInDegrees"));

//     snprintf(result, 1024, "+proj=lcc +lon_0=%lf +lat_0=%lf +lat_1=%lf +lat_2=%lf %s",
//            LoVInDegrees, LaDInDegrees, Latin1InDegrees, Latin2InDegrees, shape);

    result = "+proj=lcc +lon_0=" + std::to_string(LoVInDegrees) 
           + " +lat_0=" + std::to_string(LaDInDegrees)
           + " +lat_1=" + std::to_string(Latin1InDegrees)
           + " +lat_2=" + std::to_string(Latin2InDegrees)
           + " " + shape;

    return status;
}

GribStatus projLambertAzimuthalEqualArea(/* grib_handle* h, */ std::string& result)
{
    GribStatus status{GribStatus::SUCCESS};
    std::string shape;
    double standardParallel = 0, centralLongitude = 0;

    if ((status = getEarthShape(shape)) != GribStatus::SUCCESS)
        return status;
    standardParallel = toDouble(AccessorName("standardParallelInDegrees"));
    centralLongitude = toDouble(AccessorName("centralLongitudeInDegrees"));

//    snprintf(result, 1024, "+proj=laea +lon_0=%lf +lat_0=%lf %s",
//            centralLongitude, standardParallel, shape);

    result = "+proj=laea +lon_0=" + std::to_string(centralLongitude) 
           + " +lat_0=" + std::to_string(standardParallel)
           + " " + shape;

    return status;
}

GribStatus projPolarStereographic(/* grib_handle* h, */ std::string& result)
{
    GribStatus status{GribStatus::SUCCESS};
    double centralLongitude = 0, centralLatitude = 0;
    int has_northPole         = 0;
    long projectionCentreFlag = 0;
    std::string shape;

    if ((status = getEarthShape(shape)) != GribStatus::SUCCESS)
        return status;

    centralLongitude = toDouble(AccessorName("orientationOfTheGridInDegrees"));
    centralLatitude = toDouble(AccessorName("LaDInDegrees"));
    projectionCentreFlag = toLong(AccessorName("projectionCentreFlag"));

    has_northPole = ((projectionCentreFlag & 128) == 0);
//    snprintf(result, 1024, "+proj=stere +lat_ts=%lf +lat_0=%s +lon_0=%lf +k_0=1 +x_0=0 +y_0=0 %s",
//            centralLatitude, has_northPole ? "90" : "-90", centralLongitude, shape);

    result = "+proj=stere +lat_ts=" + std::to_string(centralLatitude) 
           + " +lat_0=" + (has_northPole ? "90" : "-90")
           + "  +lon_0=" + std::to_string(centralLongitude)
           + " +k_0=1 +x_0=0 +y_0=0 " + shape;

    return status;
}

// ECC-1552: This is for regular_ll, regular_gg, reduced_ll, reduced_gg
//           These are not 'projected' grids!
GribStatus projUnprojected(/* grib_handle* h, */ std::string& result)
{
    GribStatus status{GribStatus::SUCCESS};
    //char shape[64] = {0,};
    //if ((err = get_earth_shape(h, shape)) != GRIB_SUCCESS) return err;
    //snprintf(result, 1024, "+proj=longlat %s", shape);
    //snprintf(result, 1024, "+proj=longlat +datum=WGS84 +no_defs +type=crs");
    result = "+proj=longlat +datum=WGS84 +no_defs +type=crs";

    return status;
}

GribStatus projMercator(/* grib_handle* h, */ std::string& result)
{
    GribStatus status{GribStatus::SUCCESS};
    double LaDInDegrees = 0;
    std::string shape;

    LaDInDegrees = toDouble(AccessorName("LaDInDegrees"));

    if ((status = getEarthShape(shape)) != GribStatus::SUCCESS)
        return status;
//    snprintf(result, 1024, "+proj=merc +lat_ts=%lf +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 %s",
//            LaDInDegrees, shape);

    result = "+proj=merc +lat_ts=" + std::to_string(LaDInDegrees) 
           + " +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 " + shape;

    return status;
}

using namespace std::string_view_literals;
static std::map<std::string_view, std::function<GribStatus(std::string&)>> projMappings{
    { "regular_ll"sv, projUnprojected },
    { "regular_gg"sv, projUnprojected },
    { "reduced_ll"sv, projUnprojected },
    { "reduced_gg"sv, projUnprojected },
    { "mercator"sv, projMercator },
    { "lambert"sv, projLambertConformal },
    { "polar_stereographic"sv, projPolarStereographic },
    { "lambert_azimuthal_equal_area"sv, projLambertAzimuthalEqualArea },
    { "svpace_view"sv, projSpaceView },
    { "albers"sv, projAlbers },
    { "transverse_mercator"sv, projTransverseMercator },
    { "equatorial_azimuthal_equidistant"sv, projEquatorialAzimuthalEquidistant },
};

// ProjString Helpers [END] -----------------------------------------------------------------------

ProjStringData::ProjStringData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // For now we just "know" which argument to extract
    int argIndex{0};
    gridType_ = AccessorName(std::get<std::string>(initData[0].second));
    endpoint_ = std::get<long>(initData[1].second);

    // These are private base members - consider a redesign...
    // length_   = 0;
    // a->flags |= GRIB_ACCESSOR_FLAG_READ_ONLY;
    // a->flags |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

GribType ProjStringData::nativeType() const
{
    return GribType::STRING;
}

constexpr static int endpointSource{0};
constexpr static int endpointTarget{1};
GribStatus ProjStringData::unpack(std::string &value) const
{
    GribStatus status{GribStatus::SUCCESS};

    Assert(endpoint_ == endpointSource || endpoint_ == endpointTarget);

    std::string gridType = toString(gridType_);

    if(auto it = projMappings.find(gridType); it != std::end(projMappings))
    {
        if (endpoint_ == endpointSource) {
            //snprintf(v, 64, "EPSG:4326");
            value = "EPSG:4326";
        }
        else {
            // Invoke the appropriate function to get the target proj string
            if(status = it->second(value); status != GribStatus::SUCCESS) return status;
        }
    }
    else{
        value = {};
        return GribStatus::NOT_FOUND;
    }

    if(value.empty()) { Assert(false); }

    return status;
}

namespace {
AccessorBuilder<ProjStringData> ProjStringDataBuilder(AccessorType("proj_string"));
}

}

