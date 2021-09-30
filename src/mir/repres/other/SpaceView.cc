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

#include <algorithm>
#include <cmath>
#include <functional>
#include <ostream>
#include <sstream>
#include <utility>

#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/data/MIRField.h"
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


namespace {


double geometric_maximum(double x_min, double x_eps, const std::function<double(double)>& f, double f_eps = 1.e-9,
                         size_t it_max = 1000) {
    if (!std::isfinite(f(x_min))) {
        return x_min;
    }

    size_t it = 0;
    auto x    = x_min;
    for (auto dx = x_eps, fx = f(x); f_eps < dx && it < it_max; ++it) {
        auto fx_new = f(x + dx);
        if (!std::isfinite(fx_new) || std::abs(fx_new) < std::abs(fx)) {
            dx /= 2.;
        }
        else {
            x += dx;
            fx = fx_new;
            dx *= 2.;
        }
    }

    return x;
}


template <typename EXTERNAL_T, typename INTERNAL_T = EXTERNAL_T>
EXTERNAL_T get(const param::MIRParametrisation& param, const std::string& key) {
    INTERNAL_T value;
    ASSERT(param.get(key, value));
    return static_cast<EXTERNAL_T>(value);
}


}  // namespace


SpaceView::SpaceView(const param::MIRParametrisation& param) {
    // References:
    // - LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
    // - MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057, Issue 6, 21.
    // June 2006)
    // - MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057 v7 e-signed.
    // 30 November 2015)

    auto earthIsOblate = get<bool>(param, "earthIsOblate");
    earthMajorAxis_    = get<double>(param, earthIsOblate ? "earthMajorAxis" : "radius");
    earthMinorAxis_    = earthIsOblate ? get<double>(param, "earthMinorAxis") : earthMajorAxis_;

    auto Nr = get<double>(param, "NrInRadiusOfEarth") * (get<long>(param, "edition") == 1 ? 1e-6 : 1.);
    ASSERT(Nr > 1.);
    auto height = (Nr - 1.) * earthMajorAxis_;

    auto Lap = get<double>(param, "latitudeOfSubSatellitePointInDegrees");
    auto Lop = get<double>(param, "longitudeOfSubSatellitePointInDegrees");
    ASSERT(eckit::types::is_approximately_equal(Lap, 0.));

    // ASSERT(get<size_t>(param, "orientationOfTheGridInDegrees") == 180);
    ASSERT(get<bool>(param, "iScansPositively"));
    ASSERT(!get<bool>(param, "jScansPositively"));

    std::string str;
    str += " +proj=geos";
    str += " +type=crs";
    str += " +sweep=y";
    str += " +h=" + std::to_string(height);
    str += " +a=" + std::to_string(earthMajorAxis_);
    str += " +b=" + std::to_string(earthMinorAxis_);
    str += " +lon_0=" + std::to_string(Lop);

    atlas::Projection proj(atlas::Projection::Spec("type", "proj").set("proj", str));


    // (x, y) space
    auto xspace = [Nr, height](double p, double d, size_t N) -> LinearSpacing {
        // NOTE: height factor is PROJ-specific (Geostationary Satellite View documentation)
        auto r = 2. * std::asin(1. / Nr) / d;  //< [radian]
        return {-p * r * height, (double(N) - p - 1.) * r * height, long(N), true};
    };

    x_ = xspace(get<double, long>(param, "XpInGridLengths"), get<double, long>(param, "dx"), get<size_t>(param, "Nx"));
    y_ = xspace(get<double, long>(param, "YpInGridLengths"), get<double, long>(param, "dy"), get<size_t>(param, "Ny"));


    // grid
    grid_ = {x_, y_, proj};


    // bounding box
    auto eps_xy = 1e-6 * height;
    auto eps_ll = 1e-6;

    auto max_y = geometric_maximum(0., eps_xy, [&proj](double y) { return proj.lonlat({0, y}).lat(); });
    auto n     = proj.lonlat({0, max_y}).lat() + eps_ll;
    auto s     = -n;

    auto max_x = geometric_maximum(0., eps_xy, [&proj](double x) { return proj.lonlat({x, 0}).lon(); });
    auto e     = util::normalise_longitude(proj.lonlat({max_x, 0}).lon(), Lop) + eps_ll;
    auto w     = 2. * Lop - e;

    bbox_ = {n, w, s, e};
    Log::info() << bbox_ << std::endl;
    Log::info() << std::endl;
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
    NOTIMP;
    //    grib_reorder(values, scanningMode, y_.size(), x_.size());
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

    return new SpaceViewIterator(grid_.projection(), x_, y_);
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
