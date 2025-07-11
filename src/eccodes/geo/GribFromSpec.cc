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
#include <iosfwd>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/grid/HEALPix.h"
#include "eckit/geo/grid/ORCA.h"
#include "eckit/geo/grid/ReducedGaussian.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/grid/RegularGaussian.h"
#include "eckit/geo/grid/SphericalHarmonics.h"
#include "eckit/geo/grid/unstructured/FESOM.h"
#include "eckit/geo/grid/unstructured/ICON.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eccodes::geo
{


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


namespace
{


eckit::geo::util::recursive_mutex MUTEX;


class lock_type
{
    eckit::geo::util::lock_guard<eckit::geo::util::recursive_mutex> lock_guard_{ MUTEX };
};


using ::eckit::geo::PointLonLat;
using ::eckit::geo::PointXY;


static inline double normalise_longitude(const double& lon, const double& minimum)
{
    return PointLonLat::normalise_angle_to_minimum(lon, minimum);
}


struct BoundingBox
{
    BoundingBox(double _north, double _west, double _south, double _east) :
        north(_north), west(_west), south(_south), east(_east) {}

    explicit BoundingBox(const ::eckit::geo::area::BoundingBox& bbox) :
        BoundingBox(bbox.north, bbox.west, bbox.south, bbox.east) {}

    void fillGrib(grib_info& info) const
    {
        // Warning: scanning mode not considered
        info.grid.latitudeOfFirstGridPointInDegrees  = north;
        info.grid.longitudeOfFirstGridPointInDegrees = west;
        info.grid.latitudeOfLastGridPointInDegrees   = south;
        info.grid.longitudeOfLastGridPointInDegrees  = east;

        info.extra_set("expandBoundingBox", 1L);
    }

    const double north;
    const double west;
    const double south;
    const double east;
};


struct Fraction
{
    using value_type = long;

    explicit Fraction(double d) :
        Fraction(eckit::Fraction(d)) {}

    explicit Fraction(const eckit::Fraction& frac) :
        num(frac.numerator()), den(frac.denominator())
    {
        constexpr auto min = std::numeric_limits<value_type>::lowest();
        constexpr auto max = std::numeric_limits<value_type>::max();
        ASSERT(frac.denominator() != 0);
        ASSERT(min <= frac.denominator() && frac.denominator() <= max);
        ASSERT(min <= frac.numerator() && frac.numerator() <= max);
    }

    Fraction(value_type _numerator, value_type _denominator) :
        num(_numerator), den(_denominator == 0 || _numerator == 0 ? 1 : _denominator) {}


    const value_type num;
    const value_type den;
};


constexpr Fraction::value_type lcm(Fraction::value_type a)
{
    return a;
}


template <typename... Values>
constexpr Fraction::value_type lcm(Fraction::value_type a, Values... bc)
{
    return (std::lcm(a, lcm(bc...)));
}


constexpr Fraction::value_type gcd(Fraction::value_type a)
{
    return a;
}


template <typename... Values>
constexpr Fraction::value_type gcd(Fraction::value_type a, Values... bc)
{
    return (std::gcd(a, gcd(bc...)));
}


struct BasicAngle : Fraction
{
    using Fraction::Fraction;

    BasicAngle(Fraction a, Fraction b, Fraction c, Fraction d, Fraction e, Fraction f) :
        Fraction(gcd(a.num, b.num, c.num, d.num, e.num, f.num), lcm(a.den, b.den, c.den, d.den, e.den, f.den))
    {
        ASSERT(den != 0);
    }

    explicit BasicAngle(const grib_info& info) :
        BasicAngle(Fraction(info.grid.latitudeOfFirstGridPointInDegrees),
                   Fraction(normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.)),
                   Fraction(info.grid.latitudeOfLastGridPointInDegrees),
                   Fraction(normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.)),
                   Fraction(info.grid.iDirectionIncrementInDegrees), Fraction(info.grid.jDirectionIncrementInDegrees)) {}

    void fillGrib(grib_info& info) const
    {
        // FIXME limited functionality
        ASSERT(info.grid.grid_type == CODES_UTIL_GRID_SPEC_REGULAR_LL);

        auto fill = [this, &info](const char* key, double value) {
            Fraction f(value);
            ASSERT(f.den != 0);
            if (f.num != 0) {
                info.extra_set(key, numerator(f));
            }
        };

        info.extra_set("basicAngleOfTheInitialProductionDomain", num);
        info.extra_set("subdivisionsOfBasicAngle", den);

        fill("latitudeOfFirstGridPoint", info.grid.latitudeOfFirstGridPointInDegrees);
        fill("longitudeOfFirstGridPoint", normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.));
        fill("latitudeOfLastGridPoint", info.grid.latitudeOfLastGridPointInDegrees);
        fill("longitudeOfLastGridPoint", normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.));
        fill("iDirectionIncrement", info.grid.iDirectionIncrementInDegrees);
        fill("jDirectionIncrement", info.grid.jDirectionIncrementInDegrees);
    }

    value_type numerator(const Fraction& f) const
    {
        Fraction x(f.num * den, f.den * num);
        const auto div = gcd(x.num, x.den);

        ASSERT(x.den == div);  // if BasicAngle is adequate
        return x.num / div;
    }

    static void list(std::ostream& out)
    {
        out << "as-input, decimal, fraction";
    }
};


struct Rotation
{
    Rotation(double south_pole_lat, double south_pole_lon, double south_pole_angle) :
        south_pole_lat_(south_pole_lat), south_pole_lon_(south_pole_lon), south_pole_angle_(south_pole_angle) {}

    explicit Rotation(const ::eckit::geo::projection::Rotation& r) :
        Rotation(r.south_pole().lat,
                 r.south_pole().lon,
                 r.angle()) {}

    void fillGrib(grib_info& info) const
    {
        // Warning: scanning mode not considered

        info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;

        info.grid.latitudeOfSouthernPoleInDegrees  = south_pole_lat_;
        info.grid.longitudeOfSouthernPoleInDegrees = south_pole_lon_;

        if (!eckit::types::is_approximately_equal<double>(south_pole_angle_, 0.)) {
            info.extra_set("angleOfRotationInDegrees", south_pole_angle_);
        }
    }

private:
    const double south_pole_lat_;
    const double south_pole_lon_;
    const double south_pole_angle_;
};


class ScanningMode
{
public:
    explicit ScanningMode(const ::eckit::geo::grid::Regular& grid) :
        iScansNegatively_(grid.x().a() < grid.x().b() ? 0L : 1L),
        jScansPositively_(grid.y().a() < grid.y().b() ? 1L : 0L) {}

    void fillGrib(grib_info& info) const
    {
        info.grid.iScansNegatively = iScansNegatively_;
        info.grid.jScansPositively = jScansPositively_;
    }

private:
    long iScansNegatively_;
    long jScansPositively_;
};


class Shape
{
public:
    explicit Shape(const ::eckit::geo::Figure& figure) :
        figure_(figure) {}

    void fillGrib(grib_info& info) const
    {
        static const std::string EDITION{ "edition" };
        static const std::string SHAPE{ "shapeOfTheEarth" };
        static const auto* A = "earthMajorAxis";
        static const auto* B = "earthMinorAxis";

        // GRIB edition=2 only, check if shape is already set/provided
        auto edition  = info.packing.editionNumber;
        auto code     = 6L;
        bool provided = false;

        for (long j = 0; j < info.packing.extra_settings_count; ++j) {
            const auto& set = info.packing.extra_settings[j];

            if (set.name == EDITION && set.type == CODES_TYPE_LONG) {
                edition = set.long_value;
            }
            else if (set.name == SHAPE && set.type == CODES_TYPE_LONG) {
                code     = set.long_value;
                provided = true;
            }
        }

        if (edition != 2) {
            return;
        }

        if (!provided) {
            code = figure_.spherical() ? 1L : 7L;
            info.extra_set(SHAPE.c_str(), code);
        }

        switch (code) {
            case 1:
                info.extra_set("radius", figure_.R());
                break;
            case 3:
                info.extra_set(A, figure_.a() / 1000.);
                info.extra_set(B, figure_.b() / 1000.);
            case 6:
                break;
            case 7:
                info.extra_set(A, figure_.a());
                info.extra_set(B, figure_.b());
                break;
            default:
                throw ::eckit::geo::exception::FigureError("Shape: unsupported " + SHAPE + ": " + std::to_string(code), Here());
        }
    }

private:
    // -- Members

    const ::eckit::geo::Figure& figure_;
};


void set_grid_type_regular_ll(grib_info& info, const Grid& grid)
{
    const auto rotated  = grid.projection().type() == "rotation";
    info.grid.grid_type = rotated ? CODES_UTIL_GRID_SPEC_ROTATED_LL : CODES_UTIL_GRID_SPEC_REGULAR_LL;

    const auto& g =
        dynamic_cast<const ::eckit::geo::grid::Regular&>(grid);

    info.grid.iDirectionIncrementInDegrees = g.dx();  // west-east
    info.grid.jDirectionIncrementInDegrees = g.dy();  // south-north

    info.grid.Ni = static_cast<long>(g.nx());
    info.grid.Nj = static_cast<long>(g.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);

    if (rotated) {
        Rotation rotation(dynamic_cast<const ::eckit::geo::projection::Rotation&>(grid.projection()));
        rotation.fillGrib(info);
    }
}


void set_grid_type_regular_gg(grib_info& info, const Grid& grid)
{
    const auto rotated  = grid.projection().type() == "rotation";
    info.grid.grid_type = rotated ? CODES_UTIL_GRID_SPEC_ROTATED_GG : CODES_UTIL_GRID_SPEC_REGULAR_GG;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::RegularGaussian&>(grid);

    info.grid.N                            = static_cast<long>(r.N());
    info.grid.iDirectionIncrementInDegrees = 90. / r.N();

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);
}


void set_grid_type_reduced_gg(grib_info& info, const Grid& grid)
{
    const auto rotated  = grid.projection().type() == "rotation";
    info.grid.grid_type = rotated ? CODES_UTIL_GRID_SPEC_REDUCED_ROTATED_GG : CODES_UTIL_GRID_SPEC_REDUCED_GG;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::ReducedGaussian&>(grid);

    info.grid.pl      = r.pl().data();
    info.grid.pl_size = static_cast<long>(r.pl().size());
    info.grid.N       = static_cast<long>(r.N());

    info.grid.Nj = static_cast<long>(r.ny());

    BoundingBox bbox(dynamic_cast<const ::eckit::geo::area::BoundingBox&>(grid.area()));
    bbox.fillGrib(info);
}


void set_grid_type_unstructured(grib_info& info, const Grid& grid)
{
    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    auto properties = [&info](const auto& grid) {
        info.extra_set("unstructuredGridType", grid.name().c_str());
        info.extra_set("unstructuredGridSubtype", grid.arrangement().c_str());
        info.extra_set("uuidOfHGrid", grid.uid().c_str());
    };


    if (const auto type = grid.type();
        type == "fesom") {
        properties(dynamic_cast<const ::eckit::geo::grid::unstructured::FESOM&>(grid));
    }
    else if (type == "icon") {
        properties(dynamic_cast<const ::eckit::geo::grid::unstructured::ICON&>(grid));
    }
    else if (type == "orca") {
        properties(dynamic_cast<const ::eckit::geo::grid::ORCA&>(grid));
    }
}


void set_grid_type_healpix(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;

    const auto& r = dynamic_cast<const ::eckit::geo::grid::HEALPix&>(grid);

    info.grid.N                                  = static_cast<long>(r.Nside());
    info.grid.longitudeOfFirstGridPointInDegrees = 45.;

    info.extra_set("orderingConvention", r.order().c_str());
}


void set_grid_type_spherical_harmonics(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_SH;

    const auto& r = dynamic_cast<const ::eckit::geo::grid::SphericalHarmonics&>(grid);

    info.grid.truncation = static_cast<long>(r.truncation());

    // MIR-131: repacking must happen, so computeLaplacianOperator is set
    info.packing.packing_type             = CODES_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX;
    info.packing.computeLaplacianOperator = 1;
    info.packing.truncateLaplacian        = 1;
    // info.packing.laplacianOperator = 0;
}


void set_grid_type_lambert_azimuthal_equal_area(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;

    const auto& r =
        dynamic_cast<const ::eckit::geo::grid::Regular&>(grid);

    info.packing.editionNumber = 2;

    auto reference = std::get<PointLonLat>(grid.projection().fwd(PointXY{ 0., 0. }));
#if 0
    auto firstLL   = std::get<PointLonLat>(grid.projection().fwd({ x().front(), y().front() }));

    info.grid.Ni = static_cast<long>(r.nx());
    info.grid.Nj = static_cast<long>(r.ny());

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL.lat;
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL.lon;
#endif

    info.extra_set("DxInMetres", r.dx());
    info.extra_set("DyInMetres", r.dy());
    info.extra_set("standardParallelInDegrees", reference.lat);
    info.extra_set("centralLongitudeInDegrees", reference.lon);

    // some extra keys are edition-specific
    RegularGrid_fillGrib(info);
}


void set_grid_type_grid_type_lambert(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;


#if 0
    auto reference = std::get<PointLonLat>(grid.projection().fwd({ 0., 0. }));

    PointXY first     = { firstPointBottomLeft() ? x().min() : x().front(), firstPointBottomLeft() ? y().min() : y().front() };
    auto firstLL   = std::get<PointLonLat>(grid.projection().fwd(first));

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL.lat;
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL.lon, 0) : firstLL.lon;

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.latitudeOfSouthernPoleInDegrees  = latitudeOfSouthernPoleInDegrees_;
    info.grid.longitudeOfSouthernPoleInDegrees = longitudeOfSouthernPoleInDegrees_;
    info.grid.uvRelativeToGrid                 = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("Latin1InDegrees", reference.lat);
    info.extra_set("Latin2InDegrees", reference.lat);
    info.extra_set("LoVInDegrees", writeLonPositive_ ? util::normalise_longitude(reference.lon, 0)
                                                     : reference.lon);

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", reference.lat);
    }
#endif

    // some extra keys are edition-specific
    RegularGrid_fillGrib(info);
}


void set_grid_type_polar_stereographic(grib_info& info, const Grid& grid)
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;

#if 0
    Point2 first   = { x().front(), y().front() };
    Point2 firstLL = grid().projection().lonlat(first);

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL.lat;
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL.lon, 0) : firstLL.lon;

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.uvRelativeToGrid = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("orientationOfTheGridInDegrees", util::normalise_longitude(orientationOfTheGridInDegrees_, 0));

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", LaDInDegrees_);
    }
#endif

    // some extra keys are edition-specific
    RegularGrid_fillGrib(info);
}


void set_rotation(grib_info& info, const Grid& grid)
{
    Rotation rotation(dynamic_cast<const ::eckit::geo::projection::Rotation&>(grid.projection()));
    rotation.fillGrib(info);
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

    grib_info info;

    if (const auto g = grid->type(); g == "regular-ll") {
        set_grid_type_regular_ll(info, *grid);
    }
    else if (g == "regular-xy") {
        const auto p = grid->projection().type();
        if (p == "laea") {
            set_grid_type_lambert_azimuthal_equal_area(info, *grid);
        }
        else if (p == "lcc") {
            set_grid_type_grid_type_lambert(info, *grid);
        }
        else if (p == "polar-stereographic") {
            set_grid_type_polar_stereographic(info, *grid);
        }
        else {
            throw ::eckit::SeriousBug("GribFromSpec: unknown projection type: '" + p + "'");
        }
    }
    else if (g == "reduced-ll") {
        NOTIMP;
    }
    else if (g == "regular-gg") {
        set_grid_type_regular_gg(info, *grid);
    }
    else if (g == "reduced-gg") {
        set_grid_type_reduced_gg(info, *grid);
    }
    else if (g == "healpix") {
        set_grid_type_healpix(info, *grid);
    }
    else if (g == "unstructured" || g == "fesom" || g == "icon" || g == "orca") {
        set_grid_type_unstructured(info, *grid);
    }
    else if (g == "sh") {
        set_grid_type_spherical_harmonics(info, *grid);
    }
    else {
        throw ::eckit::SeriousBug("GribFromSpec: unknown grid type: '" + g + "'");
    }


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


    // Extra settings incl.:
    // - paramId
    // - uvRelativeToGrid (in codes_util_grid_spec as well)
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
