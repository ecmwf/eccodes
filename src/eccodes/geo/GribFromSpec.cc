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

#include <utility>
#include <cstring>
#include <memory>
#include <vector>
#include <string>
#include <vector>

#include "eckit/geo/Spec.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/Projection.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/geo/area/BoundingBox.h"

#include "eccodes/geo/BoundingBox.h"
#include "eccodes/geo/BasicAngle.h"
#include "eccodes/geo/Rotation.h"


namespace eccodes::geo
{


struct Representation
{
    explicit Representation(const Spec&) {}

    virtual void fill(grib_info&) const = 0;
};


struct SphericalHarmonics : Representation
{
    explicit SphericalHarmonics(const Spec& spec) :
        Representation(spec), spec_(spec) {}

private:
    const Spec& spec_;
};


struct Gridded : Representation
{
    enum ProjectionType
    {
        UNROTATED = 0,
        ROTATED,
        LAMBERT_AZIMUTHAL_EQUAL_AREA,
        LAMBERT_CONFORMAL_CONIC,
        POLAR_STEREOGRAPHIC,
        MERCATOR,
        TRANSVERSE_MERCATOR,
    };

    enum LatitudeType
    {
        LAT_REGULAR = 0,
        LAT_GAUSSIAN,
        LAT_UNSTRUCTURED,
    };

    enum LongitudeType
    {
        LON_REGULAR = 0,
        LON_REDUCED,
        LON_UNSTRUCTURED,
    };

    explicit Gridded(const Spec& spec) :
        Representation(spec),
        grid_(::eckit::geo::GridFactory::build(spec)),
        projection_(grid_->projection())
    {
        /*
         * TODO:
         * - none => longlat
         * - rotation => ob_tran
         * - polar-stereographic => stere
         * - merc => mercator
         * - tmerc => transverse_mercator
         */
        const auto p    = projection().type();
        projectionType_ = p == "none" ? ProjectionType::UNROTATED : p == "rotation"          ? ProjectionType::ROTATED
                                                                : p == "laea"                ? ProjectionType::LAMBERT_AZIMUTHAL_EQUAL_AREA
                                                                : p == "lcc"                 ? ProjectionType::LAMBERT_CONFORMAL_CONIC
                                                                : p == "polar-stereographic" ? ProjectionType::POLAR_STEREOGRAPHIC
                                                                : p == "merc"                ? ProjectionType::MERCATOR
                                                                : p == "tmerc"               ? ProjectionType::TRANSVERSE_MERCATOR
                                                                                             : throw ::eckit::SeriousBug("GribFromSpec: unknown projection type: " + p);

        const auto g = grid().type();
        if (g == "regular-ll") {
            latitudeType_  = LAT_REGULAR;
            longitudeType_ = LON_REGULAR;
        }
        else if (g == "reduced-ll") {
            latitudeType_  = LAT_REGULAR;
            longitudeType_ = LON_REDUCED;
        }
        else if (g == "regular-gg") {
            latitudeType_  = LAT_GAUSSIAN;
            longitudeType_ = LON_REGULAR;
        }
        else if (g == "reduced-gg") {
            latitudeType_  = LAT_GAUSSIAN;
            longitudeType_ = LON_REGULAR;
        }
        else if (g == "healpix") {
            latitudeType_  = LAT_REGULAR;
            longitudeType_ = LON_REDUCED;
        }
        else if (g == "unstructured" || g == "fesom" || g == "icon" || g == "orca") {
            latitudeType_  = LAT_UNSTRUCTURED;
            longitudeType_ = LON_UNSTRUCTURED;
        }
        else {
            throw ::eckit::SeriousBug("GribFromSpec: unknown projection type: " + g);
        }
    };

    const Grid&
    grid() const
    {
        return *grid_;
    }

    const Projection& projection() const
    {
        return projection_;
    }

    ProjectionType projectionType() const
    {
        return projectionType_;
    }

    LatitudeType latitudeType() const
    {
        return latitudeType_;
    }

    LongitudeType longitudeType() const
    {
        return longitudeType_;
    }

protected:
    const Grid* grid_ptr() const
    {
        return grid_.get();
    }

private:
    std::unique_ptr<const Grid> grid_;

    const Projection& projection_;

    ProjectionType projectionType_ = ProjectionType::UNROTATED;
    LatitudeType latitudeType_     = LatitudeType::LAT_REGULAR;
    LongitudeType longitudeType_   = LongitudeType::LON_REGULAR;
};


struct Regular : Gridded
{
    explicit Regular(const Spec& spec) :
        Gridded(spec), regular_grid_(dynamic_cast<const ::eckit::geo::grid::Regular*>(grid_ptr()))
    {
        ASSERT(regular_grid_);

        ASSERT(latitudeType() == LatitudeType::LAT_REGULAR);
        ASSERT(longitudeType() == LongitudeType::LON_REGULAR);
    }

    void fill(grib_info& info) const override
    {
        info.grid.iDirectionIncrementInDegrees = regular_grid_->dx();  // west-east
        info.grid.jDirectionIncrementInDegrees = regular_grid_->dy();  // south-north

        info.grid.Ni = static_cast<long>(regular_grid_->nx());
        info.grid.Nj = static_cast<long>(regular_grid_->ny());

        BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid().area()));
        bbox.fillGrib(info);

        // See copy_spec_from_ksec.c in libemos for info
        if (projectionType() == ProjectionType::UNROTATED) {
            info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_LL;
        }
        else if (projectionType() == ProjectionType::ROTATED) {
            info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;

            Rotation rotation(dynamic_cast<const ::eckit::geo::projection::Rotation&>(projection()));
            rotation.fillGrib(info);
        }

        NOTIMP;
    }

private:
    std::unique_ptr<const ::eckit::geo::grid::Regular> regular_grid_;
};


struct Reduced : Gridded
{
    explicit Reduced(const Spec& spec) :
        Gridded(spec)
    {
        ASSERT(longitudeType() == LongitudeType::LON_REDUCED);
        ASSERT(projectionType() == Gridded::ProjectionType::ROTATED || projectionType() == Gridded::ProjectionType::UNROTATED);
    }

    void fill(grib_info&) const override
    {
        NOTIMP;
    }
};


struct Unstructured : Gridded
{
    explicit Unstructured(const Spec& spec) :
        Gridded(spec)
    {
        ASSERT(latitudeType() == LatitudeType::LAT_UNSTRUCTURED);
        ASSERT(longitudeType() == LongitudeType::LON_UNSTRUCTURED);
        ASSERT(projectionType() == Gridded::ProjectionType::UNROTATED);
    }

    void fill(grib_info&) const override
    {
        NOTIMP;
    }
};


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


}  // namespace


GribFromSpec::GribFromSpec(BasicAngleFormat basicAngleFormat) :
    basicAngleFormat_(basicAngleFormat)
{
}


int GribFromSpec::set(codes_handle* h, const Spec&, const std::map<std::string, long>& extra)
{
    // Protect ecCodes and set error callback handling (throws)
    lock_type lock;
    codes_set_codes_assertion_failed_proc(&codes_assertion);

    grib_info info;

    // Ask representation to update info
#if 0
    repres::RepresentationHandle repres(field.representation());
    repres->fillGrib(info);
#endif

    // Basic angle (after representation), support only for gridType=regular_ll

    if (basicAngleFormat_ == BasicAngleFormat::AS_INPUT) {
        std::vector<long> fraction(2);
        CHECK_CALL(codes_get_long(h, "basicAngleOfTheInitialProductionDomain", fraction.data()));
        CHECK_CALL(codes_get_long(h, "subdivisionsOfBasicAngle", &fraction[1]));

        BasicAngle basic(fraction[0], fraction[1]);
        basic.fillGrib(info);
    }
    else if (basicAngleFormat_ == BasicAngleFormat::FRACTION) {
        BasicAngle basic(info);
        basic.fillGrib(info);
    }
    else {
        // codes_grib_util_set_spec does not need anything here (GRIB standard)
        ASSERT(basicAngleFormat_ == BasicAngleFormat::DECIMAL);
    }

    // Extra settings (paramId comes from here)
    for (const auto& k : extra) {
        info.extra_set(k.first.c_str(), k.second);
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
