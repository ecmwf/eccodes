/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eccodes/geo/GribToSpec.h"

// #include <algorithm>
#include <cstring>
// #include <ios>
// #include <sstream>
#include <string>
// #include <utility>
#include <vector>
#include <string>
#include <vector>

#include "eckit/types/Fraction.h"
#include "eckit/geo/Spec.h"
#include "eckit/exception/Exceptions.h"


namespace eccodes::geo
{


struct grib_info
{
    grib_info();
    grib_info(const grib_info&) = delete;
    grib_info(grib_info&&)      = delete;

    ~grib_info() = default;

    void operator=(grib_info&&)      = delete;
    void operator=(const grib_info&) = delete;

    void extra_set(const char* key, long);
    void extra_set(const char* key, double);
    void extra_set(const char* key, const char*);

    codes_util_grid_spec grid;
    codes_util_packing_spec packing;

private:
    std::vector<std::string> strings_;
    const size_t extra_settings_size_;
};


grib_info::grib_info() :
    grid{}, packing{}, extra_settings_size_(sizeof(packing.extra_settings) / sizeof(packing.extra_settings[0]))
{
    // NOTE low-level initialisation only necessary for C interface
    std::memset(&grid, 0, sizeof(grid));
    std::memset(&packing, 0, sizeof(packing));

    strings_.reserve(extra_settings_size_);
}


void grib_info::extra_set(const char* key, long value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set      = packing.extra_settings[j];
    set.name       = key;
    set.type       = CODES_TYPE_LONG;
    set.long_value = value;
}


void grib_info::extra_set(const char* key, double value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set        = packing.extra_settings[j];
    set.name         = key;
    set.type         = CODES_TYPE_DOUBLE;
    set.double_value = value;
}


void grib_info::extra_set(const char* key, const char* value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set = packing.extra_settings[j];
    set.name  = key;
    set.type  = CODES_TYPE_STRING;

    strings_.emplace_back(value);
    set.string_value = strings_.back().c_str();
}


void Rotation_fillGrib(grib_info& info)
{
#if 0
    // Warning: scanning mode not considered

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;

    info.grid.latitudeOfSouthernPoleInDegrees  = rotation_.south_pole().lat;
    info.grid.longitudeOfSouthernPoleInDegrees = rotation_.south_pole().lon;

    // This is missing from the grib_spec
    // Remove that when supported
    if (!eckit::types::is_approximately_equal<double>(rotation_.angle(), 0.)) {
        info.extra_set("angleOfRotationInDegrees", rotation_.angle());
    }
#endif
}


void LatLon_fillGrib(grib_info& info)
{
#if 0
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    info.grid.Ni = long(ni_);
    info.grid.Nj = long(nj_);

    increments_.fillGrib(info);
    bbox_.fillGrib(info);
#endif
}


void RegularLL_fillGrib(grib_info& info)
{
#if 0
    LatLon_fillGrib(info);

    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_LL;
#endif
}


void RotatedLL_fillGrib(grib_info& info)
{
#if 0
    LatLon_fillGrib(info);

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;
    rotation_.fillGrib(info);
#endif
}


void Reduced_fillGrib(grib_info& info)
{
#if 0
    // See copy_spec_from_ksec.c in libemos for info

    const auto& pl = pls();

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj        = long(Nj_);
    info.grid.N         = long(N_);
    info.grid.pl        = &pl[k_];
    info.grid.pl_size   = long(Nj_);

    for (size_t i = k_; i < k_ + Nj_; i++) {
        ASSERT(pl[i] > 0);
    }

    bbox_.fillGrib(info);
#endif
}


void RotatedOctahedral_fillGrib(grib_info& info)
{
#if 0
    Octahedral_fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
#endif
}


void RotatedClassic_fillGrib(grib_info& info)
{
#if 0
    Classic_fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
#endif
}


void RotatedFromPL_fillGrib(grib_info& info)
{
#if 0
    FromPL_fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG;
#endif
}


void Regular_fillGrib(grib_info& info)
{
#if 0
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_GG;

    info.grid.N                            = long(N_);
    info.grid.iDirectionIncrementInDegrees = getSmallestIncrement();
    info.grid.Ni                           = long(Ni_);
    info.grid.Nj                           = long(Nj_);

    bbox_.fillGrib(info);
#endif
}


void RotatedGG_fillGrib(grib_info& info)
{
#if 0
    Regular_fillGrib(info);
    rotation_.fillGrib(info);
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_GG;
#endif
}


void HEALPix_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type                          = GRIB_UTIL_GRID_SPEC_HEALPIX;
    info.grid.N                                  = static_cast<long>(grid_->Nside());
    info.grid.longitudeOfFirstGridPointInDegrees = 45.;

    info.extra_set("orderingConvention", grid_->order().c_str());
#endif
}


void RegularGrid_fillGrib(grib_info& info)
{
#if 0
    // shape of the reference system
    shape_.fillGrib(info, grid_.projection().spec());

    // scanningMode
    info.grid.iScansNegatively = x_.front() < x_.back() ? 0L : 1L;
    info.grid.jScansPositively = y_.front() < y_.back() ? 1L : 0L;
#endif
}


void Lambert_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;

    Point2 first     = { firstPointBottomLeft() ? x().min() : x().front(), firstPointBottomLeft() ? y().min() : y().front() };
    Point2 firstLL   = grid().projection().lonlat(first);
    Point2 reference = grid().projection().lonlat({ 0., 0. });

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL[LLCOORDS::LON], 0) : firstLL[LLCOORDS::LON];

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.latitudeOfSouthernPoleInDegrees  = latitudeOfSouthernPoleInDegrees_;
    info.grid.longitudeOfSouthernPoleInDegrees = longitudeOfSouthernPoleInDegrees_;
    info.grid.uvRelativeToGrid                 = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("Latin1InDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("Latin2InDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("LoVInDegrees", writeLonPositive_ ? util::normalise_longitude(reference[LLCOORDS::LON], 0)
                                                     : reference[LLCOORDS::LON]);

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", reference[LLCOORDS::LAT]);
    }

    // some extra keys are edition-specific, so parent call is here
    RegularGrid_fillGrib(info);
#endif
}


void LambertAzimuthalEqualArea_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    Point2 reference = grid().projection().lonlat({ 0., 0. });
    Point2 firstLL   = grid().projection().lonlat({ x().front(), y().front() });

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LON];

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("standardParallelInDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("centralLongitudeInDegrees", reference[LLCOORDS::LON]);

    // some extra keys are edition-specific, so parent call is here
    RegularGrid_fillGrib(info);
#endif
}


void PolarStereographic_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;

    Point2 first   = { x().front(), y().front() };
    Point2 firstLL = grid().projection().lonlat(first);

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL[LLCOORDS::LON], 0) : firstLL[LLCOORDS::LON];

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.uvRelativeToGrid = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("orientationOfTheGridInDegrees", util::normalise_longitude(orientationOfTheGridInDegrees_, 0));

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", LaDInDegrees_);
    }

    // some extra keys are edition-specific, so parent call is here
    RegularGrid_fillGrib(info);
#endif
}


void UnstructuredGrid_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;
#endif
}


void FESOM_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    info.extra_set("unstructuredGridType", grid_->name().c_str());
    info.extra_set("unstructuredGridSubtype", grid_->arrangement().c_str());
    info.extra_set("uuidOfHGrid", grid_->uid().c_str());
#endif
}


void ICON_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    info.extra_set("unstructuredGridType", grid_->name().c_str());
    info.extra_set("unstructuredGridSubtype", grid_->arrangement().c_str());
    info.extra_set("uuidOfHGrid", grid_->uid().c_str());
#endif
#endif
}


void ORCA_fillGrib(grib_info& info)
{
#if 0
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    info.extra_set("unstructuredGridType", grid_->name().c_str());
    info.extra_set("unstructuredGridSubtype", grid_->arrangement().c_str());
    info.extra_set("uuidOfHGrid", grid_->uid().c_str());
#endif
}


void SphericalHarmonics_fillGrib(grib_info& info)
{
#if 0
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type  = CODES_UTIL_GRID_SPEC_SH;
    info.grid.truncation = static_cast<long>(truncation_);

    // MIR-131: repacking must happen, so computeLaplacianOperator is set
    info.packing.packing_type             = CODES_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX;
    info.packing.computeLaplacianOperator = 1;
    info.packing.truncateLaplacian        = 1;
    // info.packing.laplacianOperator = 0;
#endif
}


int grib_from_spec(codes_handle* h, const Spec&)
{
    // Protect ecCodes and set error callback handling (throws)
    // util::lock_guard<util::recursive_mutex> lock(local_mutex);
    codes_set_codes_assertion_failed_proc(&codes_assertion);

    grib_info info;

    // Ask representation to update info
    repres::RepresentationHandle repres(field.representation());
    repres->fillGrib(info);

    // Basic angle (after representation), support only for gridType=regular_ll
    std::string basicAngle = "decimal";
    param.get("basic-angle", basicAngle);

    if (basicAngle == "as-input") {
        std::vector<long> fraction(2);
        CHECK_CALL(codes_get_long(h, "basicAngleOfTheInitialProductionDomain", &fraction[0]));
        CHECK_CALL(codes_get_long(h, "subdivisionsOfBasicAngle", &fraction[1]));

        grib::BasicAngle basic(fraction[0], fraction[1]);
        basic.fillGrib(info);
    }
    else if (basicAngle == "fraction") {
        grib::BasicAngle basic(info);
        basic.fillGrib(info);
    }
    else {
        // codes_grib_util_set_spec does not need anything here (GRIB standard)
        ASSERT(basicAngle == "decimal");
    }

    // Extra settings (paramId comes from here)
    for (const auto& k : field.metadata(i)) {
        info.extra_set(k.first.c_str(), k.second);
    }


    const auto& values = field.values(i);
    int flags          = 0;
    int err            = 0;

    try {
        auto* hh = codes_grib_util_set_spec(h, &info.grid, &info.packing, flags, values.data(), values.size(), &err);

        CHECK_CALL(err);  // err == CODES_WRONG_GRID

        codes_handle_delete(h);
        h = hh;
    }
    catch (...) {
        codes_handle_delete(h);
        throw;
    }

    return CODES_SUCCESS;
}


}  // namespace eccodes::geo
