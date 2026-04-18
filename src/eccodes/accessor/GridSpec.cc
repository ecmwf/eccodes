/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


#include "GridSpec.h"

#include "eccodes_config.h"
#include "eccodes/geo/eckit.h"

#if defined(HAVE_ECKIT_GEO)
    #include <cstring>
    #include <memory>

    #include "eckit/geo/Grid.h"
    #include "eckit/geo/Exceptions.h"

    #include "geo/GribFromSpec.h"
    #include "geo/GribToSpec.h"
#endif


eccodes::accessor::GridSpec _grib_accessor_grid_spec;
eccodes::Accessor* grib_accessor_grid_spec = &_grib_accessor_grid_spec;

namespace eccodes::accessor
{

void GridSpec::init(const long len, grib_arguments* arg)
{
    length_ = 0;
    flags_ |= GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC;
}

long GridSpec::get_native_type()
{
    return GRIB_TYPE_STRING;
}


int GridSpec::pack_string(const char* v, size_t* len)
{
#if defined(HAVE_GEOGRAPHY) && defined(HAVE_ECKIT_GEO)

    auto* h = get_enclosing_handle();
    ECCODES_ASSERT(h);

    if (!eccodes::geo::eckit_geo_use_for_gridspec(h)) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "GridSpec::pack_string not available");
        return GRIB_NOT_IMPLEMENTED;
    }

    ECCODES_ASSERT(len != nullptr && 0 < *len);
    ECCODES_ASSERT(v && v[*len] == '\0');

    std::string spec_str(v);
    ECCODES_ASSERT(spec_str.length() == *len);

    try {
        eccodes::geo::eckit_main_init();

        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec_str));
        ASSERT(grid);

        auto* result = eccodes::geo::GribFromSpec::set(h, grid->spec());
        if (!result) return GRIB_GEOCALCULUS_PROBLEM;
    }
    catch (eckit::geo::Exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "GridSpec: geo::Exception thrown (%s)", e.what());
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    catch (std::exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "GridSpec: Exception thrown (%s)", e.what());
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    return GRIB_SUCCESS;
#else
    // print_warning_feature_not_implemented();
    return GRIB_NOT_IMPLEMENTED;
#endif
}

int GridSpec::unpack_string(char* v, size_t* len)
{
#if defined(HAVE_GEOGRAPHY) && defined(HAVE_ECKIT_GEO)

    auto* h = get_enclosing_handle();
    ECCODES_ASSERT(h);

    if (!eccodes::geo::eckit_geo_use_for_gridspec(h)) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "GridSpec::unpack_string not available");
        return GRIB_NOT_IMPLEMENTED;
    }
    ECCODES_ASSERT(0 < *len);
    ECCODES_ASSERT(v);

    std::string spec_str;

    try {
        eccodes::geo::eckit_main_init();

        std::unique_ptr<const Spec> spec(new eccodes::geo::GribToSpec(h));
        ASSERT(spec);

        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(*spec));
        ASSERT(grid);

        spec_str = grid->spec_str();
    }
    catch (eckit::geo::Exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "GridSpec: geo::Exception thrown (%s)", e.what());
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    catch (std::exception& e) {
        grib_context_log(context_, GRIB_LOG_ERROR, "GridSpec: Exception thrown (%s)", e.what());
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    // guarantee null-termination
    auto length = spec_str.length();
    if (*len < length + 1) {
        grib_context_log(context_, GRIB_LOG_ERROR,
                         "%s: Buffer too small for %s. It is %zu bytes long (len=%zu)",
                         class_name_, name_, length, *len);
        return GRIB_BUFFER_TOO_SMALL;
    }

    std::strncpy(v, spec_str.c_str(), *len);
    ECCODES_ASSERT(v[length] == '\0');

    *len = length;

    return GRIB_SUCCESS;
#else
    // print_warning_feature_not_implemented();
    return GRIB_NOT_IMPLEMENTED;
#endif
}

}  // namespace eccodes::accessor
