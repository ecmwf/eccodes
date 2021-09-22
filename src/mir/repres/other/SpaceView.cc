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


#include "mir/repres/other/SpaceView.h"

#include <ostream>
#include <sstream>
#include <utility>

#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Angles.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace other {


static RepresentationBuilder<SpaceView> __builder("space_view");


#if 0
Point2 SpaceView::Projection::xy(const Point2& /*p*/) const {
    NOTIMP;
}


Point2 SpaceView::Projection::lonlat(const Point2& /*p*/) const {
    NOTIMP;
}


void SpaceView::Projection::hash(eckit::Hash& h) const {
    h << "SpaceView::Projection";
}
#endif


SpaceView::SpaceView(const param::MIRParametrisation& param) {
    param.get("earthMajorAxis", earthMajorAxis_ = util::Earth::radius());
    param.get("earthMinorAxis", earthMinorAxis_ = util::Earth::radius());

#if 1
    long nx;
    long ny;
    ASSERT(param.get("numberOfPointsAlongXAxis", nx));
    ASSERT(param.get("numberOfPointsAlongYAxis", ny));

    // Sub-satellite point
    double Lap;
    double Lop;
    ASSERT(param.get("latitudeOfSubSatellitePointInDegrees", Lap));
    ASSERT(param.get("longitudeOfSubSatellitePointInDegrees", Lop));

    // Apparent diameter of Earth in grid lengths, in X/Y-directions
    size_t dx;
    size_t dy;
    ASSERT(param.get("dx", dx));
    ASSERT(param.get("dy", dy));

    // X/Y-coordinate of sub-satellite point (in units of 10^-3 grid length expressed as an integer)
    size_t Xp;
    size_t Yp;
    ASSERT(param.get("XpInGridLengths", Xp));
    ASSERT(param.get("YpInGridLengths", Yp));

    // Angle between the increasing Y-axis and the meridian of the sub-satellite point in the direction of increasing
    // latitude
    double orientationOfTheGridInDegrees;
    ASSERT(param.get("orientationOfTheGridInDegrees", orientationOfTheGridInDegrees));

    // Altitude of the camera from the Earth's centre in units of the Earth's (equatorial) radius
    double NrInRadiusOfEarth;
    ASSERT(param.get("NrInRadiusOfEarth", NrInRadiusOfEarth));

    // X/Y-coordinate of origin of sector image
    size_t xCoordinateOfOriginOfSectorImage;
    size_t yCoordinateOfOriginOfSectorImage;
    ASSERT(param.get("xCoordinateOfOriginOfSectorImage", xCoordinateOfOriginOfSectorImage));
    ASSERT(param.get("yCoordinateOfOriginOfSectorImage", yCoordinateOfOriginOfSectorImage));

    double h = NrInRadiusOfEarth - earthMajorAxis_;
    ASSERT(h >= 0);

    Projection::Spec spec("type", "proj");

    std::ostringstream str;
    str << " +proj=geos";
    str << " +h=" << h;
    str << " +lon_0=" << Lop;
    str << " +sweep=y";
    str << " +a=" << earthMajorAxis_;
    str << " +b=" << earthMinorAxis_;
    str << " +type=crs";

    spec.set("proj", str.str());
    projection_ = {spec};


    x_    = {-double(nx)/2,double(nx)/2, nx};
    y_    = {-double(ny)/2,double(ny)/2, ny};
    grid_ = {x_, y_, projection_};

    atlas::RectangularDomain range({x_.min(), x_.max()}, {y_.min(), y_.max()}, "meters");
    auto bbox = projection_.lonlatBoundingBox(range);
    Log::info() << bbox << std::endl;
    ASSERT(bbox);

    bbox_ = {bbox.north(), bbox.west(), bbox.south(), bbox.east()};

#else
    // TODO projection_
    double rpol = earthMinorAxis_;
    double req  = earthMajorAxis_;

    size_t Ni;
    size_t Nj;
    ASSERT(param.get("numberOfPointsAlongXAxis", Ni) && Ni > 0);
    ASSERT(param.get("numberOfPointsAlongYAxis", Nj) && Nj > 0);

    // --

    double dx;
    double dy;
    double offx;
    double offy;
    long xp;
    long yp;
    double slat;
    double slon;
    ASSERT(param.get("dx", dx));
    ASSERT(param.get("dy", dy));
    ASSERT(param.get("xCoordinateOfOriginOfSectorImage", offx));  // Xo
    ASSERT(param.get("yCoordinateOfOriginOfSectorImage", offy));  // Yo
    ASSERT(param.get("XpInGridLengths", xp));
    ASSERT(param.get("YpInGridLengths", yp));
    ASSERT(param.get("latitudeOfSubSatellitePointInDegrees", slat));
    ASSERT(param.get("longitudeOfSubSatellitePointInDegrees", slon));

    // --

    double sub_lon = 0;

    double x = 0;
    double y = 0;

    // --

    double f1 = 42164. * std::cos(x) * std::cos(y);
    double f2 = std::cos(y) * std::cos(y) + 1.006803 * std::sin(y) * std::sin(y);

    double sd = std::sqrt(f1 * f1 - f2 * 1737121856.);
    double sn = (f1 - sd) / f2;

    double s1  = 42164. - sn * std::cos(x) * std::cos(y);
    double s2  = sn * std::sin(x) * std::cos(y);
    double s3  = -sn * std::sin(y);
    double sxy = std::sqrt(s1 * s1 + s2 * s2);

    // --

    double lon = std::atan2(s2, s1) + sub_lon;
    double lat = std::atan2(1.006803 * s3, sxy);
#endif
}


SpaceView::~SpaceView() = default;


bool SpaceView::sameAs(const Representation& other) const {
    auto name = [](const SpaceView& repres) {
        std::ostringstream str;
        repres.makeName(str);
        return str.str();
    };

    auto o = dynamic_cast<const SpaceView*>(&other);
    return (o != nullptr) && name(*this) == name(*o);
}


void SpaceView::fill(grib_info& /*info*/) const {
    NOTIMP;
}


void SpaceView::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
}


void SpaceView::reorder(long scanningMode, MIRValuesVector& values) const {
    grib_reorder(values, scanningMode, y_.size(), x_.size());
}


void SpaceView::validate(const MIRValuesVector& values) const {
    auto count = numberOfPoints();

    Log::debug() << "SpaceView::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << " (" << domain() << ")." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("SpaceView", values.size(), count);
}


void SpaceView::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << earthMajorAxis_;
    h << earthMinorAxis_;
    h << grid_;
    h << projection_;

    auto type = grid_.projection().spec().getString("type");
    out << "SpaceView-" << (type.empty() ? "" : type + "-") << h.digest();
}


void SpaceView::print(std::ostream& out) const {
    out << "SpaceView[]";
}


Iterator* SpaceView::iterator() const {
    class SpaceViewIterator : public Iterator {
        Projection projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;
        PointLonLat pLonLat_;

        size_t ni_;
        size_t nj_;
        size_t i_;
        size_t j_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "SpaceViewIterator[";
            Iterator::print(out);
            out << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            if (j_ < nj_ && i_ < ni_) {
                pLonLat_ = projection_.lonlat({x_[i_], y_[j_]});
                _lat     = lat(pLonLat_.lat());
                _lon     = lon(pLonLat_.lon());

                if (++i_ == ni_) {
                    i_ = 0;
                    j_++;
                }

                count_++;
                return true;
            }
            return false;
        }

    public:
        SpaceViewIterator(Projection projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(std::move(projection)), x_(x), y_(y), ni_(x.size()), nj_(y.size()), i_(0), j_(0), count_(0) {}
        SpaceViewIterator(const SpaceViewIterator&) = delete;
        SpaceViewIterator& operator=(const SpaceViewIterator&) = delete;
    };

    return new SpaceViewIterator(projection_, x_, y_);
}


size_t SpaceView::numberOfPoints() const {
    return x_.size() * y_.size();
}


util::Domain SpaceView::domain() const {
    return {bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east()};
}


const util::BoundingBox& SpaceView::boundingBox() const {
    return bbox_;
}


bool SpaceView::getLongestElementDiagonal(double&) const {
    return false;
}


void SpaceView::estimate(api::MIREstimation&) const {
    NOTIMP;
}


}  // namespace other
}  // namespace repres
}  // namespace mir
