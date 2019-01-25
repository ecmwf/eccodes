/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015

#include <cmath>
#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/Plural.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/unsupported/LambertAzimuthalEqualArea.h"
#include "mir/util/Angles.h"
#include "mir/util/Domain.h"
#include "mir/util/MeshGeneratorParameters.h"

namespace mir {
namespace repres {

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const param::MIRParametrisation& parametrisation)
    : Dx_(0.), Dy_(0.), nx_(0), ny_(0) {
    eckit::Log::warning() << "WARNING: scanningMode is completelly ignored!" << std::endl;

    // reference parallel/meridian ([degree])
    double value[2];
    parametrisation.get("standardParallelInDegrees", value[0]);
    parametrisation.get("centralLongitudeInDegrees", value[1]);
    reference_ = PointLatLon(value[0], value[1]);

    // first point (in [degree] and [m])
    parametrisation.get("latitudeOfFirstGridPointInDegrees", value[0]);
    parametrisation.get("longitudeOfFirstGridPointInDegrees", value[1]);
    firstLL_ = PointLatLon(value[0], value[1]);

    radius_ = 0.;
    parametrisation.get("radius", radius_);
    ASSERT(radius_ > 0.);

    double phi1 = util::degree_to_radian(reference_.lat().value());
    double sin_phi1 = std::sin(phi1);
    double cos_phi1 = std::cos(phi1);

    double phi = util::degree_to_radian(firstLL_.lat().value());
    double sin_phi = std::sin(phi);
    double cos_phi = std::cos(phi);

    double dlambda = util::degree_to_radian((firstLL_.lon() - reference_.lon()).value());
    double cos_dlambda = std::cos(dlambda);
    double sin_dlambda = std::sin(dlambda);

    double kp = radius_ * std::sqrt(2. / (1. + sin_phi1 * sin_phi + cos_phi1 * cos_phi * cos_dlambda));

    firstXY_ = Point2(kp * cos_phi * sin_dlambda, kp * (cos_phi1 * sin_phi - sin_phi1 * cos_phi * cos_dlambda));

    // increments [m]
    parametrisation.get("xDirectionGridLengthInMetres", Dx_);
    parametrisation.get("yDirectionGridLengthInMetres", Dy_);

    Dy_ = -Dy_; // FIXME: more intelligence here please!
    ASSERT(Dx_ > 0.);
    ASSERT(Dy_ < 0.);

    ASSERT(parametrisation.get("numberOfPointsAlongXAxis", nx_));
    ASSERT(parametrisation.get("numberOfPointsAlongYAxis", ny_));
    ASSERT(nx_ > 0);
    ASSERT(ny_ > 0);

    // calculate "contained" bounding box (internal to the Lambert Az. Eq. Area limits)
    // FIXME: This isn't working well really!!!
    double lambda0 = util::degree_to_radian(reference_.lon().value());

    auto f = [=](double x, double y) -> Point2 {
        double sin_phi1 = std::sin(phi1);
        double cos_phi1 = std::cos(phi1);

        double rho = std::sqrt(x * x + y * y);
        double c = 2. * std::asin(rho / (2. * radius_));
        double cos_c = std::cos(c);
        double sin_c = std::sin(c);

        return Point2{lambda0 + std::atan2(x * sin_c, rho * cos_phi1 * cos_c - y * sin_phi1 * sin_c),
                      std::asin(cos_c * sin_phi1 + y * sin_c * cos_phi1 / rho)};
    };

    Point2 lastXY = Point2::add(firstXY_, Point2((nx_ - 1) * Dx_, (ny_ - 1) * Dy_));
    Point2 min = Point2::componentsMin(firstXY_, lastXY);
    Point2 max = Point2::componentsMax(firstXY_, lastXY);

    Point2 corners[] = {f(min[0], max[1]), f(max[0], max[1]), f(min[0], min[1]), f(max[0], min[1])};

    double w = util::radian_to_degree(std::max(corners[0][0], corners[2][0]));
    double e = util::radian_to_degree(std::min(corners[1][0], corners[3][0]));

    double n = std::min(corners[0][1], corners[1][1]);
    double s = std::max(corners[2][1], corners[3][1]);
    n = util::radian_to_degree(min[0] * max[0] < 0 ? std::min(n, f(0, max[1])[1]) : n);
    s = util::radian_to_degree(min[0] * max[0] < 0 ? std::max(s, f(0, min[1])[1]) : s);

    bbox_ = util::BoundingBox(n, w, s, e);
}

LambertAzimuthalEqualArea::~LambertAzimuthalEqualArea() = default;

void LambertAzimuthalEqualArea::print(std::ostream& out) const {
    out << "LambertAzimuthalEqualArea["
           "reference_="
        << reference_ << ",firstLL=" << firstLL_ << ",Dx=" << Dx_ << ",Dy=" << Dy_ << ",nx=" << nx_ << ",ny=" << ny_
        << "]";
}

size_t LambertAzimuthalEqualArea::numberOfPoints() const {
    return nx_ * ny_;
}

atlas::Grid LambertAzimuthalEqualArea::atlasGrid() const {
    using atlas::grid::StructuredGrid;
    using atlas::grid::LinearSpacing;

    Point2 lastXY = Point2::add(firstXY_, Point2((nx_ - 1) * Dx_, (ny_ - 1) * Dy_));
    StructuredGrid::XSpace xspace(LinearSpacing(firstXY_[0], lastXY[0], long(nx_)));
    StructuredGrid::YSpace yspace(LinearSpacing(firstXY_[1], lastXY[1], long(ny_)));

    atlas::util::Config projection;
    projection.set("type", "lambert_azimuthal_equal_area");
    projection.set("standard_parallel", reference_.lat().value());
    projection.set("central_longitude", reference_.lon().value());

    return StructuredGrid(xspace, yspace, projection);
}

bool LambertAzimuthalEqualArea::isPeriodicWestEast() const {
    return false;
}

bool LambertAzimuthalEqualArea::includesNorthPole() const {
    return false;
}

bool LambertAzimuthalEqualArea::includesSouthPole() const {
    return false;
}

void LambertAzimuthalEqualArea::fill(grib_info& info) const {
    NOTIMP;
}

void LambertAzimuthalEqualArea::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();
    eckit::Log::debug<LibMir>() << "LatLon::validate checked " << eckit::Plural(values.size(), "value")
                                << ", within domain: " << eckit::BigNum(count) << "." << std::endl;
    ASSERT(values.size() == count);
}

Iterator* LambertAzimuthalEqualArea::iterator() const {

    class LAEAIterator : public Iterator {
        double Dx_;
        double Dy_;
        size_t nx_;
        size_t ny_;
        size_t i_;
        size_t j_;
        size_t count_;

        double radius_;
        double lambda0_;
        double phi1_;

        double x_;
        double y_;
        double sin_phi1_;
        double cos_phi1_;
        double x0_;
        double y2_;

        void print(std::ostream& out) const {
            out << "LAEAIterator[";
            Iterator::print(out);
            out << ",Dx=" << Dx_ << ",Dy=" << Dy_ << ",nx=" << nx_ << ",ny=" << ny_ << ",i=" << i_ << ",j=" << j_
                << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) {

            if (j_ < ny_ && i_ < nx_) {
                double rho = std::sqrt(x_ * x_ + y2_);
                if (eckit::types::is_approximately_equal(rho, 0.)) {

                    _lat = lat(util::radian_to_degree(phi1_));
                    _lon = lon(util::radian_to_degree(lambda0_));

                } else {
                    double c = 2. * std::asin(rho / (2. * radius_));
                    double cos_c = std::cos(c);
                    double sin_c = std::sin(c);

                    _lat = lat(util::radian_to_degree(std::asin(cos_c * sin_phi1_ + y_ * sin_c * cos_phi1_ / rho)));
                    _lon = lon(util::radian_to_degree(
                        lambda0_ + std::atan2(x_ * sin_c, rho * cos_phi1_ * cos_c - y_ * sin_phi1_ * sin_c)));
                }

                x_ += Dx_;
                if (++i_ == nx_) {
                    i_ = 0;
                    x_ = x0_;
                    j_++;
                    y_ += Dy_;
                    y2_ = y_ * y_;
                }

                count_++;
                return true;
            }
            return false;
        }

    public:
        LAEAIterator(double radius, const PointLatLon& reference, const Point2& firstXY, size_t nx, size_t ny,
                     double Dx, double Dy)
            : Dx_(Dx)
            , Dy_(Dy)
            , nx_(nx)
            , ny_(ny)
            , i_(0)
            , j_(0)
            , count_(0)
            , radius_(radius)
            , lambda0_(util::degree_to_radian(reference.lon().value()))
            , phi1_(util::degree_to_radian(reference.lat().value())) {
            ASSERT(Dx_ > 0);
            ASSERT(Dy_ < 0);

            x_ = firstXY[0];
            y_ = firstXY[1];

            sin_phi1_ = std::sin(phi1_);
            cos_phi1_ = std::cos(phi1_);
            x0_ = x_;
            y2_ = y_ * y_;
        }
        LAEAIterator(const LAEAIterator&) = delete;
        LAEAIterator& operator=(const LAEAIterator&) = delete;
    };

    return new LAEAIterator(radius_, reference_, firstXY_, nx_, ny_, Dx_, Dy_);
}

void LambertAzimuthalEqualArea::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << reference_.lat() << reference_.lon() << firstLL_.lat() << firstLL_.lon() << radius_;

    out << "LAEA-" << nx_ << "x" << ny_ << "-" << h.digest();
}

bool LambertAzimuthalEqualArea::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const LambertAzimuthalEqualArea*>(&other);
    return o && nx_ == o->nx_ && ny_ == o->ny_ && eckit::types::is_approximately_equal(radius_, o->radius_) &&
           eckit::types::is_approximately_equal(Dx_, o->Dx_) && eckit::types::is_approximately_equal(Dy_, o->Dy_) &&
           reference_.lon() == o->reference_.lon() && reference_.lat() == o->reference_.lat() &&
           firstLL_.lon() == o->firstLL_.lon() && firstLL_.lat() == o->firstLL_.lat();
}

void LambertAzimuthalEqualArea::fill(util::MeshGeneratorParameters& params) const {
    params.meshGenerator_ = "structured";
}

namespace {
static RepresentationBuilder<LambertAzimuthalEqualArea>
    lambertAzimuthalEqualArea("lambert_azimuthal_equal_area"); // Name is what is returned by grib_api
}

} // namespace repres
} // namespace mir
