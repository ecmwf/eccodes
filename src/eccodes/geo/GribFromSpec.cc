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


#include "eccodes/geo/GribFromSpec.h"

#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/grid/RegularGaussian.h"
#include "eckit/geo/util/mutex.h"

#include "eccodes/geo/BasicAngle.h"
#include "eccodes/geo/BoundingBox.h"
#include "eccodes/geo/Rotation.h"
#include "eccodes/geo/Shape.h"


namespace eccodes::geo
{


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


namespace
{


eckit::geo::util::recursive_mutex MUTEX;


class lock_type
{
    eckit::geo::util::lock_guard<eckit::geo::util::recursive_mutex> lock_guard_{ MUTEX };
};


int grid_type_regular_ll(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_LL;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::Regular&>(grid);

    info.grid.iDirectionIncrementInDegrees = r.dx();  // west-east
    info.grid.jDirectionIncrementInDegrees = r.dy();  // south-north

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);

    return CODES_SUCCESS;
}


int grid_type_rotated_ll(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::Regular&>(grid);

    info.grid.iDirectionIncrementInDegrees = r.dx();  // west-east
    info.grid.jDirectionIncrementInDegrees = r.dy();  // south-north

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);

    Rotation rotation(dynamic_cast<const ::eckit::geo::projection::Rotation&>(grid.projection()));
    rotation.fillGrib(info);

    return CODES_SUCCESS;
}


int grid_type_lambert_azimuthal_equal_area(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;

    using ::eckit::geo::PointLonLat;
    using ::eckit::geo::PointXY;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::Regular&>(grid);

    info.packing.editionNumber = 2;

    auto reference = std::get<PointLonLat>(grid.projection().fwd(PointXY{ 0., 0. }));
    auto firstLL   = std::get<PointLonLat>(grid.projection().fwd({ x().front(), y().front() }));

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL.lat;
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL.lon;

    info.extra_set("DxInMetres", r.dx());
    info.extra_set("DyInMetres", r.dy());
    info.extra_set("standardParallelInDegrees", reference.lat);
    info.extra_set("centralLongitudeInDegrees", reference.lon);

    // some extra keys are edition-specific, so parent call is here
    RegularGrid_fillGrib(info);

    return CODES_SUCCESS;
}


int grid_type_grid_type_lambert(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;

    NOTIMP;

    return CODES_SUCCESS;
}


int grid_type_polar_stereographic(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;

    NOTIMP;

    return CODES_SUCCESS;
}


int grid_type_regular_gg(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_GG;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::RegularGaussian&>(grid);

    info.grid.N                            = static_cast<long>(r.N());
    info.grid.iDirectionIncrementInDegrees = 90. / r.N();

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);

    return CODES_SUCCESS;
}


int grid_type_rotated_gg(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_GG;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::RegularGaussian&>(grid);

    info.grid.N                            = static_cast<long>(r.N());
    info.grid.iDirectionIncrementInDegrees = 90. / r.N();

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);

    Rotation rotation(dynamic_cast<const ::eckit::geo::projection::Rotation&>(grid.projection()));
    rotation.fillGrib(info);

    return CODES_SUCCESS;
}


}  // namespace


int GribFromSpec::set(codes_handle* h, const Spec& spec, const std::map<std::string, long>& extra)
{
    // Protect ecCodes and set error callback handling (throws)
    lock_type lock;
    codes_set_codes_assertion_failed_proc(&codes_assertion);


    // Check grid properties
    //
    // TODO:
    // - none => longlat
    // - rotation => ob_tran
    // - polar-stereographic => stere
    std::unique_ptr<const Grid> grid(::eckit::geo::GridFactory::build(spec));
    ASSERT(grid);

    struct key_type
    {
        ProjectionType proj;
        LatitudeType lat;
        LongitudeType lon;
    } key;

    if (const auto p = grid->projection().type(); p == "none") {
        key.proj = ProjectionType::UNROTATED;
    }
    else if (p == "rotation") {
        key.proj = ProjectionType::ROTATED;
    }
    else if (p == "laea") {
        key.proj = ProjectionType::LAMBERT_AZIMUTHAL_EQUAL_AREA;
    }
    else if (p == "lcc") {
        key.proj = ProjectionType::LAMBERT_CONFORMAL_CONIC;
    }
    else if (p == "polar-stereographic") {
        key.proj = ProjectionType::POLAR_STEREOGRAPHIC;
    }
    else {
        throw ::eckit::SeriousBug("GribFromSpec: unknown projection type: " + p);
    }

    if (const auto g = grid->type(); g == "regular-ll") {
        key.lat = LatitudeType::LAT_REGULAR;
        key.lon = LongitudeType::LON_REGULAR;
    }
    else if (g == "reduced-ll") {
        key.lat = LatitudeType::LAT_REGULAR;
        key.lon = LongitudeType::LON_REDUCED;
    }
    else if (g == "regular-gg") {
        key.lat = LatitudeType::LAT_GAUSSIAN;
        key.lon = LongitudeType::LON_REGULAR;
    }
    else if (g == "reduced-gg") {
        key.lat = LatitudeType::LAT_GAUSSIAN;
        key.lon = LongitudeType::LON_REGULAR;
    }
    else if (g == "healpix") {
        key.lat = LatitudeType::LAT_REGULAR;
        key.lon = LongitudeType::LON_REDUCED;
    }
    else if (g == "unstructured" || g == "fesom" || g == "icon" || g == "orca") {
        key.lat = LatitudeType::LAT_UNSTRUCTURED;
        key.lon = LongitudeType::LON_UNSTRUCTURED;
    }
    else {
        throw ::eckit::SeriousBug("GribFromSpec: unknown projection type: " + g);
    }


    grib_info info;

    // Map from Spec properties to grib_info method
    static const std::map<key_type, std::function<int(grib_info&, const Grid&)>> GRIB_INFO_METHODS{
        { { UNROTATED, LAT_REGULAR, LON_REGULAR }, grid_type_regular_ll },
        { { ROTATED, LAT_REGULAR, LON_REGULAR }, grid_type_rotated_ll },

        { { LAMBERT_AZIMUTHAL_EQUAL_AREA }, grid_type_lambert_azimuthal_equal_area },
        { { LAMBERT_CONFORMAL_CONIC }, grid_type_grid_type_lambert },
        { { POLAR_STEREOGRAPHIC }, grid_type_polar_stereographic },

        { { UNROTATED, LAT_GAUSSIAN, LON_REGULAR }, grid_type_regular_gg },
        { { ROTATED, LAT_GAUSSIAN, LON_REGULAR }, grid_type_rotated_gg },
    };

    CHECK_CALL(GRIB_INFO_METHODS.at(key)(info, *grid));


    // Basic angle (after representation), support only for gridType=regular_ll

    if (basicAngleFormat_ == BasicAngleType::AS_INPUT) {
        std::vector<long> fraction(2);
        CHECK_CALL(codes_get_long(h, "basicAngleOfTheInitialProductionDomain", fraction.data()));
        CHECK_CALL(codes_get_long(h, "subdivisionsOfBasicAngle", &fraction[1]));

        BasicAngle basic(fraction[0], fraction[1]);
        basic.fillGrib(info);
    }
    else if (basicAngleFormat_ == BasicAngleType::FRACTION) {
        BasicAngle basic(info);
        basic.fillGrib(info);
    }
    else {
        // codes_grib_util_set_spec does not need anything here (GRIB standard)
        ASSERT(basicAngleFormat_ == BasicAngleType::DECIMAL);
    }

    // Extra settings (paramId comes from here)
    for (const auto& [key, value] : extra) {
        info.extra_set(key.c_str(), value);
    }


    try {
        int flags = 0;
        int err   = 0;

        auto* hh = codes_grib_util_set_spec(h, &info.grid, &info.packing, flags, nullptr, 0, &err);

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
