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
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/Plural.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"
#include "atlas/util/Earth.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/unsupported/LambertAzimuthalEqualArea.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"

namespace mir {
namespace repres {

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(
    const param::MIRParametrisation& parametrisation) :
    Dx_(0.),
    Dy_(0.),
    nx_(0),
    ny_(0) {

    // set projection
    double standardParallel;
    double centralLongitude;
    double radius;
    parametrisation.get("standardParallelInDegrees", standardParallel);
    parametrisation.get("centralLongitudeInDegrees", centralLongitude);
    parametrisation.get("radius", radius);
    ASSERT(radius > 0.);

    auto projection = atlas::Projection(atlas::Projection::Spec()
                                            .set("type", "lambert_azimuthal_equal_area")
                                            .set("standard_parallel", standardParallel)
                                            .set("central_longitude", centralLongitude)
                                            .set("radius", radius));

    // increments [m]
    parametrisation.get("xDirectionGridLengthInMetres", Dx_);
    parametrisation.get("yDirectionGridLengthInMetres", Dy_);

    ASSERT(Dx_ > 0.);
    ASSERT(Dy_ > 0.);
    Dy_ = -Dy_;  // for "canonical" scanningMode (iScansPositively, jScansNegatively)

    ASSERT(parametrisation.get("numberOfPointsAlongXAxis", nx_));
    ASSERT(parametrisation.get("numberOfPointsAlongYAxis", ny_));
    ASSERT(nx_ > 0);
    ASSERT(ny_ > 0);

    // first and last points [m]
    double value[2];
    parametrisation.get("longitudeOfFirstGridPointInDegrees", value[0]);
    parametrisation.get("latitudeOfFirstGridPointInDegrees", value[1]);

    firstXY_ = projection.xy({value[0], value[1]});
    lastXY_ = Point2::add(firstXY_, Point2((nx_ - 1) * Dx_, (ny_ - 1) * Dy_));

    atlas::RectangularDomain xy({firstXY_[0], lastXY_[0]}, {firstXY_[1], lastXY_[1]}, "meters");
    atlas::RectangularDomain bbox = projection.boundingBox(xy);
    ASSERT(bbox);

    bbox_ = {bbox.ymax(), bbox.xmin(), bbox.ymin(), bbox.xmax()};

    grid_ = atlas::StructuredGrid(atlas::grid::LinearSpacing(firstXY_[0], lastXY_[0], long(nx_)),
                                  atlas::grid::LinearSpacing(firstXY_[1], lastXY_[1], long(ny_)), projection);
}

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(double standardParallel,
                                                     double centralLongitude, double firstLatitude,
                                                     double firstLongitude, double Dx, double Dy,
                                                     size_t nx, size_t ny) :
    Dx_(Dx),
    Dy_(Dy),
    nx_(nx),
    ny_(ny) {

    auto projection = atlas::Projection(atlas::Projection::Spec()
                                            .set("type", "lambert_azimuthal_equal_area")
                                            .set("standard_parallel", standardParallel)
                                            .set("central_longitude", centralLongitude)
                                            .set("radius", atlas::util::Earth::radius()));

    ASSERT(Dx_ > 0.);
    ASSERT(Dy_ > 0.);
    Dy_ = -Dy_;  // for "canonical" scanningMode (iScansPositively, jScansNegatively)

    ASSERT(nx_ > 0);
    ASSERT(ny_ > 0);

    firstXY_ = projection.xy({firstLongitude, firstLatitude});
    lastXY_ = Point2::add(firstXY_, Point2((nx_ - 1) * Dx_, (ny_ - 1) * Dy_));

    atlas::RectangularDomain xy({firstXY_[0], lastXY_[0]}, {firstXY_[1], lastXY_[1]}, "meters");
    atlas::RectangularDomain bbox = projection.boundingBox(xy);
    ASSERT(bbox);

    bbox_ = {bbox.ymax(), bbox.xmin(), bbox.ymin(), bbox.xmax()};

    grid_ = atlas::StructuredGrid(atlas::grid::LinearSpacing(firstXY_[0], lastXY_[0], long(nx_)),
                                  atlas::grid::LinearSpacing(firstXY_[1], lastXY_[1], long(ny_)), projection);
}

LambertAzimuthalEqualArea::~LambertAzimuthalEqualArea() = default;

void LambertAzimuthalEqualArea::print(std::ostream& out) const {
    out << "LambertAzimuthalEqualArea[" << bbox_ << ",projection=" << grid_.projection().spec() << ",Dx=" << Dx_
        << ",Dy=" << Dy_ << ",nx=" << nx_ << ",ny=" << ny_ << "]";
}

size_t LambertAzimuthalEqualArea::numberOfPoints() const {
    return nx_ * ny_;
}

atlas::Grid LambertAzimuthalEqualArea::atlasGrid() const {
    return grid_;
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

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    ASSERT(Dx_ > 0.);
    ASSERT(Dy_ < 0.);

    Point2 reference = grid_.projection().lonlat({0., 0.});
    Point2 firstLL = grid_.projection().lonlat(firstXY_);

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[1];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[0];

    using key_t = std::pair<const char*, long>;

    for (auto& key : {
             key_t{"xDirectionGridLengthInMillimetres", std::lround(Dx_ * 1.e3)},
             key_t{"yDirectionGridLengthInMillimetres", std::lround(-Dy_ * 1.e3)},
             key_t{"numberOfPointsAlongXAxis", nx_},
             key_t{"numberOfPointsAlongYAxis", ny_},
             key_t{"standardParallelInMicrodegrees", std::lround(reference[1] * 1.e6)},
             key_t{"centralLongitudeInMicrodegrees", std::lround(reference[0] * 1.e6)},
         }) {
        auto& set = info.packing.extra_settings[info.packing.extra_settings_count++];

        set.name       = key.first;
        set.long_value = key.second;
        set.type       = GRIB_TYPE_LONG;
    }
}

void LambertAzimuthalEqualArea::reorder(long scanningMode, mir::data::MIRValuesVector& values) const {
    GribReorder::reorder(values, scanningMode, nx_, ny_);
}

void LambertAzimuthalEqualArea::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();
    eckit::Log::debug<LibMir>() << "LatLon::validate checked " << eckit::Plural(values.size(), "value")
                                << ", within domain: " << eckit::BigNum(count) << "." << std::endl;
    ASSERT(values.size() == count);
}

Iterator* LambertAzimuthalEqualArea::iterator() const {

    class LAEAIterator : public Iterator {
        atlas::Projection projection_;

        double Dx_;
        double Dy_;
        size_t nx_;
        size_t ny_;
        size_t i_;
        size_t j_;
        size_t count_;

        double x_;
        double y_;
        double x0_;

        void print(std::ostream& out) const {
            out << "LAEAIterator[";
            Iterator::print(out);
            out << ",Dx=" << Dx_ << ",Dy=" << Dy_ << ",nx=" << nx_ << ",ny=" << ny_ << ",i=" << i_ << ",j=" << j_
                << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) {

            if (j_ < ny_ && i_ < nx_) {

                auto ll = projection_.lonlat({x_, y_});
                _lat = lat(ll.lat());
                _lon = lon(ll.lon());

                x_ += Dx_;
                if (++i_ == nx_) {
                    i_ = 0;
                    x_ = x0_;
                    j_++;
                    y_ += Dy_;
                }

                count_++;
                return true;
            }
            return false;
        }

    public:
        LAEAIterator(const atlas::Projection& projection, const Point2& firstXY, size_t nx,
                     size_t ny, double Dx, double Dy) :
            projection_(projection),
            Dx_(Dx),
            Dy_(Dy),
            nx_(nx),
            ny_(ny),
            i_(0),
            j_(0),
            count_(0) {
            ASSERT(Dx_ > 0);
            ASSERT(Dy_ < 0);

            x_ = firstXY[0];
            y_ = firstXY[1];
            x0_ = x_;
        }
        LAEAIterator(const LAEAIterator&) = delete;
        LAEAIterator& operator=(const LAEAIterator&) = delete;
    };

    return new LAEAIterator(grid_.projection(), firstXY_, nx_, ny_, Dx_, Dy_);
}

void LambertAzimuthalEqualArea::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << grid_.projection().spec();
    h << bbox_;

    out << "LAEA-" << nx_ << "x" << ny_ << "-" << h.digest();
}

bool LambertAzimuthalEqualArea::sameAs(const Representation& other) const {
    auto spec = [](const LambertAzimuthalEqualArea& repres) {
        std::stringstream str;
        repres.makeName(str);
        return str.str();
    };

    auto o = dynamic_cast<const LambertAzimuthalEqualArea*>(&other);
    return o &&
           eckit::types::is_approximately_equal(Dx_, o->Dx_) &&
           eckit::types::is_approximately_equal(Dy_, o->Dy_) &&
           spec(*this) == spec(*o);
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
