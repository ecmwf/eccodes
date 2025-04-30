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

#if defined(HAVE_ECKIT_GEO)
    #include <cstring>
    #include <memory>

    #include "eckit/geo/Grid.h"
    #include "eckit/geo/Exceptions.h"

    #include "geo/GribSpec.h"
    #include "geo/EckitMainInit.h"
#endif


eccodes::AccessorBuilder<eccodes::accessor::GridSpec> _grib_accessor_grid_spec_builder{};

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

static void print_warning_feature_not_implemented()
{
    fprintf(stderr, "ECCODES WARNING :  Key gridSpec is not yet implemented. Work in progress...\n");
}

int GridSpec::pack_string(const char* sval, size_t* len)
{
    print_warning_feature_not_implemented();
    return GRIB_NOT_IMPLEMENTED;

#if defined(HAVE_GEOGRAPHY) && defined(HAVE_ECKIT_GEO)
    // TODO(mapm)
#else
    return GRIB_NOT_IMPLEMENTED;
#endif
}

int GridSpec::unpack_string(char* v, size_t* len)
{
#if defined(HAVE_GEOGRAPHY) && defined(HAVE_ECKIT_GEO)
    if (context_->eckit_geo == 0) { // check env. variable too
        return GRIB_NOT_IMPLEMENTED;
    }
    ECCODES_ASSERT(0 < *len);
    ECCODES_ASSERT(v != nullptr);

    auto* h = get_enclosing_handle();
    ECCODES_ASSERT(h != nullptr);

    std::string spec_str;

    try {
        eccodes::geo::eckit_main_init();

        std::unique_ptr<const eckit::geo::Spec> spec(new eccodes::geo::GribSpec(h));
        std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::build(*spec));

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
                         accessor_type().get().c_str(), name_, length, *len);
        return GRIB_BUFFER_TOO_SMALL;
    }

    std::strncpy(v, spec_str.c_str(), *len);
    ECCODES_ASSERT(v[length] == '\0');

    *len = length;

    return GRIB_SUCCESS;
#else
    print_warning_feature_not_implemented();
    return GRIB_NOT_IMPLEMENTED;
#endif
}

}  // namespace eccodes::accessor
