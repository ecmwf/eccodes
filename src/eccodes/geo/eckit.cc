/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eccodes/geo/eckit.h"

#include <set>
#include <string>

#include "eccodes_config.h"
#include "eccodes/grib_api_internal.h"


#if defined(HAVE_ECKIT_GEO)
    #include "eckit/runtime/Main.h"
#endif


namespace eccodes::geo {


void eckit_main_init()
{
#if defined(HAVE_ECKIT_GEO)
    struct EckitMainInit {
        EckitMainInit() {
            if (!eckit::Main::ready()) {
                static char* argv[]{ const_cast<char*>("eccodes::geo::eckit_main_init") };
                eckit::Main::initialise(1, argv);
            }
        }
    };
    static const EckitMainInit init;
#endif
}

inline std::string get_string(const grib_handle* h, const char* name)
{
    char str[128] = {
        0,
    };
    size_t len = sizeof(str);
    if (grib_get_string(h, name, str, &len) == GRIB_SUCCESS) {
        return { str };
    }
    return {};
}


inline EckitGeoLevel eckit_geo_level(const grib_handle* h)
{
#if defined(HAVE_ECKIT_GEO)
    return h == nullptr || h->context == nullptr ? EckitGeoLevel::DISABLED : static_cast<EckitGeoLevel>(h->context->eckit_geo);
#else
    return EckitGeoLevel::DISABLED;
#endif
}


bool eckit_geo_use_for_iterator(const grib_handle* h)
{
    const auto lvl = eckit_geo_level(h);
    if (lvl == EckitGeoLevel::DISABLED || lvl == EckitGeoLevel::ENABLED) {
        return lvl == ENABLED;
    }

    // EckitGeoLevel::RESTRICTED

    // (see tests, including the differences to eccodes)
    static const std::set<std::string> GRID_TYPES{ "albers",
                                                   "healpix",
                                                   "lambert_azimuthal_equal_area",
                                                   "lambert_lam",
                                                   "lambert",
                                                   "mercator",
                                                   "polar_stereographic",
                                                   "reduced_gg",
                                                   "reduced_ll",
                                                   "reduced_rotated_gg",
                                                   "regular_gg",
                                                   "regular_ll",
                                                   "rotated_gg",
                                                   "rotated_ll",
                                                   "space_view",
                                                   "unstructured_grid", };
    return GRID_TYPES.count(get_string(h, "gridType")) != 0;
}


bool eckit_geo_use_for_projstring(const grib_handle* h)
{
    return eckit_geo_level(h) > EckitGeoLevel::DISABLED;
}


bool eckit_geo_use_grib_fixes(const grib_handle* h)
{
    return eckit_geo_level(h) > EckitGeoLevel::DISABLED;
}


}  // namespace eccodes::geo
