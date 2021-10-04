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
#include <ostream>

#include "eckit/types/FloatCompare.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Angles.h"
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
    std::string str;
    str += " +proj=geos";
    str += " +type=crs";
    str += " +sweep=y";
    str += " +h=" + std::to_string(h);
    str += " +a=" + std::to_string(a);
    str += " +b=" + std::to_string(b);
    str += " +lon_0=" + std::to_string(Lop);

    projection_ = RegularGrid::Projection::Spec("type", "proj").set("proj", str);


    // (x, y) space (the height factor on (rx, ry) is PROJ-specific
    Nx_ = get<long>(param, "Nx");
    ASSERT(1 < Nx_);

    Ny_ = get<long>(param, "Ny");
    ASSERT(1 < Ny_);

    auto ip = get<bool>(param, "iScansPositively");
    auto xp = get<double, long>(param, "XpInGridLengths");
    auto dx = get<double, long>(param, "dx");
    auto rx = 2. * std::asin(1. / Nr) / dx * h;

    (ip ? xa_ : xb_) = rx * (-xp);
    (ip ? xb_ : xa_) = rx * (-xp + double(Nx_ - 1));

    auto jp = get<bool>(param, "jScansPositively");
    auto yp = get<double, long>(param, "YpInGridLengths");
    auto dy = get<double, long>(param, "dy");
    auto ry = 2. * std::asin(1. / Nr) / dy * h;

    (jp ? ya_ : yb_) = ry * (-yp);
    (jp ? yb_ : ya_) = ry * (-yp + double(Ny_ - 1));


    // bounding box
    auto geometric_maximum = [](double x_min, double x_eps, const std::function<double(double)>& f,
                                double f_eps = 1.e-9, size_t it_max = 1000) {
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
        const Projection projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;

        const size_t Nx_;
        const size_t Ny_;
        size_t ix_;
        size_t iy_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "SpaceViewIterator[";
            Iterator::print(out);
            out << ",ix=" << ix_ << ",iy=" << iy_ << ",count=" << index() << "]";
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

        size_t index() const override { return Nx_ * iy_ + ix_; }

    public:
        SpaceViewIterator(Projection projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(std::move(projection)),
            x_(x),
            y_(y),
            Nx_(x_.size()),
            Ny_(y_.size()),
            ix_(0),
            iy_(0),
            count_(0) {
            for (auto ll = projection_.lonlat({x_[ix_], y_[iy_]}); !std::isfinite(ll.lon()) || !std::isfinite(ll.lat());
                 ll      = projection_.lonlat({x_[ix_], y_[iy_]})) {
                if (++ix_ == Nx_) {
                    ix_ = 0;
                    if (++iy_ == Ny_) {
                        break;
                    }
                }
            }
        }

        SpaceViewIterator(const SpaceViewIterator&) = delete;
        SpaceViewIterator& operator=(const SpaceViewIterator&) = delete;
    };

    return new SpaceViewIterator(grid_->projection(), x_, y_);
}


}  // namespace regular
}  // namespace repres
}  // namespace mir
