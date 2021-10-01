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


template <typename EXTERNAL_T, typename INTERNAL_T = EXTERNAL_T>
EXTERNAL_T get(const param::MIRParametrisation& param, const std::string& key) {
    INTERNAL_T value;
    ASSERT(param.get(key, value));
    return static_cast<EXTERNAL_T>(value);
}


}  // namespace


SpaceView::SpaceView(const param::MIRParametrisation& param) : sv_(param) {}


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
        params.meshGenerator_ = "delaunay";  // because points are missing
    }
}


void SpaceView::reorder(long, MIRValuesVector&) const {
    // do not reorder, iterator is doing the right thing (or should!)
    // FIXME this function should not be overriding to do nothing
}


void SpaceView::validate(const MIRValuesVector& values) const {
    auto count = numberOfPoints();

    Log::debug() << "SpaceView::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << " (" << domain() << ")." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("SpaceView", values.size(), count);
}


void SpaceView::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << sv_;
    out << "SpaceView-" << sv_.numberOfPoints();
    sv_.bbox().makeName(out);
    out << "-" << h.digest();
}


void SpaceView::print(std::ostream& out) const {
    out << "SpaceView[projection=" << sv_.projection().spec() << ",bbox=" << sv_.bbox() << "]";
}


Iterator* SpaceView::iterator() const {
    class SpaceViewIterator : public Iterator {
        const Projection projection_;
        const LinearSpacing x_;
        const LinearSpacing y_;

        const size_t N_;
        const size_t Nx_;
        const size_t Ny_;
        size_t ix_;
        size_t iy_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "SpaceViewIterator[";
            Iterator::print(out);
            out << ",ix=" << ix_ << ",iy=" << iy_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            while (iy_ < Ny_ && ix_ < Nx_) {
                auto ll = projection_.lonlat({x_[ix_], y_[iy_]});

                if (++ix_ == Nx_) {
                    ix_ = 0;
                    iy_++;
                }

                if (std::isfinite(ll.lon()) && std::isfinite(ll.lat())) {
                    _lat = lat(ll.lat());
                    _lon = lon(ll.lon());

                    count_++;
                    return true;
                }
            }
            return false;
        }

    public:
        SpaceViewIterator(const space_view_t& sv) :
            projection_(sv.projection()),
            x_(sv.x()),
            y_(sv.y()),
            N_(sv.numberOfPoints()),
            Nx_(x_.size()),
            Ny_(y_.size()),
            ix_(0),
            iy_(0),
            count_(0) {}

        ~SpaceViewIterator() override { ASSERT(count_ == N_); }

        SpaceViewIterator(const SpaceViewIterator&) = delete;
        SpaceViewIterator& operator=(const SpaceViewIterator&) = delete;
    };

    return new SpaceViewIterator(sv_);
}


size_t SpaceView::numberOfPoints() const {
    return sv_.numberOfPoints();
}


util::Domain SpaceView::domain() const {
    auto& bbox = sv_.bbox();
    return {bbox.north(), bbox.west(), bbox.south(), bbox.east()};
}


const util::BoundingBox& SpaceView::boundingBox() const {
    return sv_.bbox();
}


bool SpaceView::getLongestElementDiagonal(double&) const {
    return false;
}


void SpaceView::estimate(api::MIREstimation&) const {
    NOTIMP;
}


SpaceView::space_view_t::space_view_t(const param::MIRParametrisation& param) {
    // References:
    // - LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
    // - MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057, Issue 6, 21.
    // June 2006)
    // - MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057 v7 e-signed.
    // 30 November 2015)


    auto earthIsOblate = get<bool>(param, "earthIsOblate");
    auto a             = get<double>(param, earthIsOblate ? "earthMajorAxis" : "radius");
    auto b             = earthIsOblate ? get<double>(param, "earthMinorAxis") : a;

    auto Nr = get<double>(param, "NrInRadiusOfEarth") * (get<long>(param, "edition") == 1 ? 1e-6 : 1.);
    ASSERT(Nr > 1.);
    auto h = (Nr - 1.) * a;

    auto Lap = get<double>(param, "latitudeOfSubSatellitePointInDegrees");
    auto Lop = get<double>(param, "longitudeOfSubSatellitePointInDegrees");
    ASSERT(eckit::types::is_approximately_equal(Lap, 0.));

    // ASSERT(get<size_t>(param, "orientationOfTheGridInDegrees") == 180);


    // projection
    std::string str;
    str += " +proj=geos";
    str += " +type=crs";
    str += " +sweep=y";
    str += " +h=" + std::to_string(h);
    str += " +a=" + std::to_string(a);
    str += " +b=" + std::to_string(b);
    str += " +lon_0=" + std::to_string(Lop);

    projection_ = {Projection::Spec("type", "proj").set("proj", str)};


    // (x, y) space
    auto xspace = [Nr, h](double p, double d, size_t N, bool scansPositively) -> LinearSpacing {
        auto r       = 2. * std::asin(1. / Nr) / d;  //< [radian]
        double start = -p * r * h;                   // height factor is PROJ-specific
        double stop  = (double(N) - p - 1.) * r * h;
        return {scansPositively ? start : stop, scansPositively ? stop : start, long(N), true};
    };

    x_ = xspace(get<double, long>(param, "XpInGridLengths"), get<double, long>(param, "dx"), get<size_t>(param, "Nx"),
                get<bool>(param, "iScansPositively"));
    y_ = xspace(get<double, long>(param, "YpInGridLengths"), get<double, long>(param, "dy"), get<size_t>(param, "Ny"),
                get<bool>(param, "jScansPositively"));


    // number of (valid) points
    numberOfPoints_ = 0;
    for (size_t ix = 0; ix < x_.size(); ++ix) {
        for (size_t iy = 0; iy < y_.size(); ++iy) {
            auto ll = projection_.lonlat({x_[ix], y_[iy]});
            if (std::isfinite(ll.lon()) && std::isfinite(ll.lat())) {
                ++numberOfPoints_;
            }
        }
    }


    // bounding box
    auto geometric_maximum = [](double x_min, double x_eps, const std::function<double(double)>& f,
                                double f_eps = 1.e-9, size_t it_max = 1000) {
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
    };

    auto eps_xy = 1e-6 * h;
    auto eps_ll = 1e-6;

    auto max_y = geometric_maximum(0., eps_xy, [this](double y) { return projection_.lonlat({0, y}).lat(); });
    auto n     = projection_.lonlat({0, max_y}).lat() + eps_ll;
    auto s     = -n;

    auto max_x = geometric_maximum(0., eps_xy, [this](double x) { return projection_.lonlat({x, 0}).lon(); });
    auto e     = util::normalise_longitude(projection_.lonlat({max_x, 0}).lon(), Lop) + eps_ll;
    auto w     = 2. * Lop - e;

    bbox_ = {n, w, s, e};
}


void SpaceView::space_view_t::hash(eckit::MD5& h) const {
    h << "SpaceView::space_view_t";
    h << projection_.spec();
    h << x_.spec();
    h << y_.spec();
}


void SpaceView::remove_invalid_values(const param::MIRParametrisation& param, MIRValuesVector& values) {
    ASSERT(!values.empty());

    space_view_t sv(param);
    auto proj = sv.projection();

    size_t count = values.size();
    MIRValuesVector newValues;
    newValues.reserve(count);

    auto v = values.cbegin();
    for (auto y : sv.y()) {
        for (auto x : sv.x()) {
            auto ll = proj.lonlat({x, y});
            if (std::isfinite(ll.lon()) && std::isfinite(ll.lat())) {
                ASSERT(v != values.cend());
                newValues.emplace_back(*v);
            }
            ++v;
        }
    }

    size_t newCount = newValues.size();
    ASSERT(0 < newCount && newCount <= count);

    values.swap(newValues);
}


}  // namespace other
}  // namespace repres
}  // namespace mir
