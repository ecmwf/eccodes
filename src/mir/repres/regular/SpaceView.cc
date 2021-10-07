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


#include "mir/repres/regular/SpaceView.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

#include "eckit/types/FloatCompare.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace repres {
namespace regular {


static RepresentationBuilder<SpaceView> __builder("space_view");


namespace detail {


template <typename EXTERNAL_T, typename INTERNAL_T = EXTERNAL_T>
EXTERNAL_T get(const param::MIRParametrisation& param, const std::string& key) {
    INTERNAL_T value;
    ASSERT(param.get(key, value));
    return static_cast<EXTERNAL_T>(value);
}


SpaceViewInternal::SpaceViewInternal(const param::MIRParametrisation& param) {
    auto earthIsOblate = get<bool>(param, "earthIsOblate");
    auto a             = get<double>(param, earthIsOblate ? "earthMajorAxis" : "radius");
    auto b             = earthIsOblate ? get<double>(param, "earthMinorAxis") : a;

    auto Nr = get<double>(param, "NrInRadiusOfEarth") * (get<long>(param, "edition") == 1 ? 1e-6 : 1.);
    ASSERT(Nr > 1.);

    auto h = (Nr - 1.) * a;

    auto Lop = get<double>(param, "longitudeOfSubSatellitePointInDegrees");
    auto Lap = get<double>(param, "latitudeOfSubSatellitePointInDegrees");
    ASSERT(eckit::types::is_approximately_equal(Lap, 0.));

    // ASSERT(get<size_t>(param, "orientationOfTheGridInDegrees") == 180);


    // projection
    auto proj = [](double h, double a, double b, double lon_0) {
        auto _h = " +h=" + std::to_string(h);
        auto _l = eckit::types::is_approximately_equal(lon_0, 0.) ? "" : " +lon_0=" + std::to_string(lon_0);
        auto _e = eckit::types::is_approximately_equal(a, b) ? " +R=" + std::to_string(a)
                                                             : " +a=" + std::to_string(a) + " +b=" + std::to_string(b);
        return "+proj=geos +type=crs +sweep=y" + _h + _l + _e;
    };

    projection_ = RegularGrid::Projection::Spec("type", "proj").set("proj", proj(h, a, b, Lop));


    // (x, y) space (the height factor on (rx, ry) is PROJ-specific
    Nx_ = get<long>(param, "Nx");
    ASSERT(1 < Nx_);

    auto ip = get<bool>(param, "iScansPositively");
    auto xp = get<double, long>(param, "XpInGridLengths");
    auto dx = get<double, long>(param, "dx");
    ASSERT(dx > 0);

    auto rx = 2. * std::asin(1. / Nr) / dx * h;

    (ip ? xa_ : xb_) = rx * (-xp);
    (ip ? xb_ : xa_) = rx * (-xp + double(Nx_ - 1));

    Ny_ = get<long>(param, "Ny");
    ASSERT(1 < Ny_);

    auto jp = get<bool>(param, "jScansPositively");
    auto yp = get<double, long>(param, "YpInGridLengths");
    auto dy = get<double, long>(param, "dy");
    ASSERT(dy > 0);

    auto ry = 2. * std::asin(1. / Nr) / dy * h;

    (jp ? ya_ : yb_) = ry * (-yp);
    (jp ? yb_ : ya_) = ry * (-yp + double(Ny_ - 1));


    // longest element diagonal, a multiple of a reference central element diagonal (avoiding distortion)
    LongestElementDiagonal_ =
        20. * util::Earth::distance(projection_.lonlat({-rx / 2, ry / 2}), projection_.lonlat({rx / 2, -ry / 2}));
    ASSERT(0. < LongestElementDiagonal_);


    // pre-calculate (lon, lat) coordinates
    // (projection without lon_0 so range is Greenwich-centred)
    RegularGrid::Projection projection = RegularGrid::Projection::Spec("type", "proj").set("proj", proj(h, a, b, 0));

    lonlat_.resize(Nx_ * Ny_);
    size_t index = 0;
    for (auto& _y : y()) {
        for (auto& _x : x()) {
            auto& ll = lonlat_[index++];
            ll       = projection.lonlat({_x, _y});
            if (std::isfinite(ll.lon()) && std::isfinite(ll.lat())) {
                ASSERT(-90. < ll.lon() && ll.lon() < 90.);
                ASSERT(-90. < ll.lat() && ll.lat() < 90.);

                ll.lon() += Lop;
            }
            else {
                ll = {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};
            }
        }
    }


    // bounding box
    auto geometric_maximum = [](double x_min, double x_eps, const std::function<double(double)>& f,
                                double f_eps = 1.e-9, size_t it_max = 1000) {
        size_t it = 0;
        auto x    = x_min;
        auto fx   = f(x);

        for (auto dx = x_eps; f_eps < dx && it < it_max; ++it) {
            auto fx_new = f(x + dx);
            if (!std::isfinite(fx_new) || fx_new < fx) {
                dx /= 2.;
            }
            else {
                x += dx;
                fx = fx_new;
                dx *= 2.;
            }
        }

        ASSERT(0. < fx && fx < 90.);
        return fx;
    };

    auto eps_xy = 1e-6 * h;
    auto eps_ll = 1e-6;

    auto max_lon = geometric_maximum(0., eps_xy, [&](double x) { return projection.lonlat({x, 0}).lon(); });
    auto w       = Lop - max_lon - eps_ll;
    auto e       = Lop + max_lon + eps_ll;

    auto max_lat = geometric_maximum(0., eps_xy, [&](double y) { return projection.lonlat({0, y}).lat(); });
    auto n       = max_lat + eps_ll;
    auto s       = -n;

    bbox_ = {n, w, s, e};
}


}  // namespace detail


SpaceView::SpaceView(const param::MIRParametrisation& param) :
    detail::SpaceViewInternal(param),
    RegularGrid(SpaceViewInternal::projection_, SpaceViewInternal::bbox_, SpaceViewInternal::x(),
                SpaceViewInternal::y()) {}


void SpaceView::fill(grib_info& /*info*/) const {
    NOTIMP;
}


Iterator* SpaceView::iterator() const {
    class SpaceViewIterator : public Iterator {
        const std::vector<PointLonLat>& lonlat_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "SpaceViewIterator[";
            Iterator::print(out);
            out << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            while (count_ < lonlat_.size()) {
                // only one of (lon, lat) needs to be checked
                auto& ll = lonlat_[count_++];
                if (std::isfinite(ll.lon())) {
                    _lat = lat(ll.lat());
                    _lon = lon(ll.lon());
                    return true;
                }
            }

            return false;
        }

        size_t index() const override { return count_; }

    public:
        SpaceViewIterator(const std::vector<PointLonLat>& lonlat) : lonlat_(lonlat), count_(0) {}

        SpaceViewIterator(const SpaceViewIterator&) = delete;
        SpaceViewIterator& operator=(const SpaceViewIterator&) = delete;
    };

    return new SpaceViewIterator(lonlat_);
}


bool SpaceView::getLongestElementDiagonal(double& d) const {
    d = LongestElementDiagonal_;
    return true;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir
