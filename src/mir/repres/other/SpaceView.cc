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
#include <fstream>
#include <functional>
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <sstream>
#include <utility>

#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace atlas {
namespace projection {
namespace detail {


class GeostationarySatelliteViewProjection final : public ProjectionImpl {
public:
    GeostationarySatelliteViewProjection(const Projection::Spec& spec) :
        h_(spec.getDouble("h")), a_(spec.getDouble("a")), b_(spec.getDouble("b")), bb_aa_((b_ * b_) / (a_ * a_)) {
        ATLAS_ASSERT(0. < b_ && b_ <= a_ && a_ < h_);
    }

    static std::string static_type() { return "geostationary_satellite_view"; }

    std::string type() const override { return static_type(); }

    void xy2lonlat(double* p) const override {
        // inverse projection

        auto x = p[atlas::XYZ::XX] / h_;
        auto y = p[atlas::XYZ::YY] / h_;

        p[atlas::LONLAT::LON] = std::numeric_limits<double>::infinity();
        p[atlas::LONLAT::LAT] = std::numeric_limits<double>::infinity();

        if (!std::isfinite(x) || !std::isfinite(y)) {
            return;
        }

        double f1 = (a_ + h_) * std::cos(x) * std::cos(y);
        double f2 = std::cos(y) * std::cos(y) + std::sin(y) * std::sin(y) / bb_aa_;

        // visibility checks for positive determinant
        double det = f1 * f1 - 4. * f2 * 434280464.;
        if (det < 0) {
            return;
        }

        double sd = std::sqrt(det);
        double sn = (f1 - sd) / f2;

        double s1  = (a_ + h_) - sn * std::cos(x) * std::cos(y);
        double s2  = sn * std::sin(x) * std::cos(y);
        double s3  = -sn * std::sin(y);
        double sxy = std::sqrt(s1 * s1 + s2 * s2);

        p[atlas::LONLAT::LON] = 180. / M_PI * std::atan(s2 / s1);
        p[atlas::LONLAT::LAT] = 180. / M_PI * std::atan(s3 / (sxy * bb_aa_));
    }

    void lonlat2xy(double* p) const override {
        // forward projection

        auto lat = p[atlas::LONLAT::LON] * M_PI / 180.;
        auto lon = p[atlas::LONLAT::LAT] * M_PI / 180.;

        p[atlas::XYZ::XX] = std::numeric_limits<double>::infinity();
        p[atlas::XYZ::YY] = std::numeric_limits<double>::infinity();

        double c_lat = std::atan(bb_aa_ * std::tan(lat));  //< geocentric latitude
        double re    = b_ / std::sqrt(1. - (1. - bb_aa_) * std::cos(c_lat) * std::cos(c_lat));

        double rl = re;
        double r1 = (a_ + h_) - rl * std::cos(c_lat) * std::cos(lon);
        double r2 = -rl * std::cos(c_lat) * std::sin(lon);
        double r3 = rl * std::sin(c_lat);
        double rn = std::sqrt(r1 * r1 + r2 * r2 + r3 * r3);

        // visibility checks for positive dot product between (point, satellite) . (point, Earth centre)
        auto dot = r1 * rl * std::cos(c_lat) * std::cos(lon) - r2 * r2 - r3 * r3 / bb_aa_;
        if (dot <= 0) {
            return;
        }

        p[atlas::XYZ::XX] = h_ * std::atan(-r2 / r1);
        p[atlas::XYZ::YY] = h_ * std::asin(-r3 / rn);
    }

    Jacobian jacobian(const PointLonLat&) const override { NOTIMP; }

    bool strictlyRegional() const override { return false; }
    RectangularLonLatDomain lonlatBoundingBox(const Domain&) const override { NOTIMP; }

    Spec spec() const override { NOTIMP; }
    std::string units() const override { NOTIMP; }
    void hash(eckit::Hash&) const override { NOTIMP; }

private:
    const double h_;  //< distance from Earth surface to satellite (height)
    const double a_;  //< distance from Earth centre to equator
    const double b_;  //< distance from Earth centre to pole(s)
    const double bb_aa_;
};


}  // namespace detail
}  // namespace projection
}  // namespace atlas


namespace mir {
namespace repres {
namespace other {


static RepresentationBuilder<SpaceView> __builder("space_view");


namespace {


double bisection_method(double x_min, double x_max, const std::function<double(double)>& f, double f0 = 0.,
                        double f_eps = 1.e-9) {
    static std::random_device rd;
    static const auto entropy = rd();
    std::mt19937 gen(entropy);
    std::uniform_real_distribution<double> dis(x_min, x_max);

    auto xp = dis(gen);
    while (f(xp) < f0) {
        xp = dis(gen);
    }

    auto xm = dis(gen);
    while (f(xm) > f0) {
        xm = dis(gen);
    }

    auto x = (xp + xm) / 2.;
    for (auto fx = f(x) - f0; !eckit::types::is_approximately_equal(fx, 0., f_eps);
         x = (xp + xm) / 2., fx = f(x) - f0) {
        (fx >= 0. ? xp : xm) = x;
    }

    return x;
}


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

    Log::info() << "it=" << it << " f(" << x << ")=" << f(x) << std::endl;
    return x;
}


double maximum_radius(double x_max, double y_max, const atlas::Projection& p, size_t it_max = 1000000) {
    static std::random_device rd;
    static const auto entropy = rd();
    std::mt19937 gen(entropy);

    std::uniform_real_distribution<double> x_dis(-x_max, x_max);
    std::uniform_real_distribution<double> y_dis(-y_max, y_max);

    double r2 = 0;
    for (size_t it = 0; it < it_max; ++it) {
        auto xy = p.xy(p.lonlat({x_dis(gen), y_dis(gen)}));

        if (std::isfinite(xy.x()) && std::isfinite(xy.y())) {
            auto r2_new = xy.x() * xy.x() + xy.y() * xy.y();
            if (r2 < r2_new) {
                r2 = r2_new;
            }
        }
    }

    return std::sqrt(r2);
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
    // June 2006).

    // const double SAT_HEIGHT = 42164.;     //< distance from Earth centre to satellite
    // const double R_EQ       = 6378.169;   //< distance from Earth centre to equator
    // const double R_POL      = 6356.5838;  //< distance from Earth centre to pole(s)
    // const double RR         = (R_EQ * R_EQ) / (R_POL * R_POL);

    auto earthIsOblate = get<bool>(param, "earthIsOblate");
    earthMajorAxis_    = get<double>(param, earthIsOblate ? "earthMajorAxis" : "radius");
    earthMinorAxis_    = earthIsOblate ? get<double>(param, "earthMinorAxis") : earthMajorAxis_;

    auto Nr = get<double>(param, "NrInRadiusOfEarth") * (get<long>(param, "edition") == 1 ? 1e-6 : 1.);
    ASSERT(Nr > 1.);
    auto height = (Nr - 1.) * earthMajorAxis_;

    auto Lap = get<double>(param, "latitudeOfSubSatellitePointInDegrees");
    auto Lop = get<double>(param, "longitudeOfSubSatellitePointInDegrees");
    ASSERT(eckit::types::is_approximately_equal(Lap, 0.));

    // auto orientation = get<size_t>(param, "orientationOfTheGridInDegrees");
    // ASSERT(orientation == 180);

    std::string str;
    str += " +proj=geos";
    str += " +type=crs";
    str += " +sweep=y";
    str += " +h=" + std::to_string(1e-3 * height);
    str += " +a=" + std::to_string(1e-3 * earthMajorAxis_);
    str += " +b=" + std::to_string(1e-3 * earthMinorAxis_);
    str += " +lon_0=" + std::to_string(Lop);

    atlas::Projection proj(atlas::Projection::Spec("type", "proj").set("proj", str));
    Log::info() << "proj = " << proj.spec() << std::endl;

    atlas::Projection::Spec sv_spec("type", "geostationary_satellite_view");
    sv_spec.set("h", 1e-3 * height);
    sv_spec.set("a", 1e-3 * earthMajorAxis_);
    sv_spec.set("b", 1e-3 * earthMinorAxis_);

    atlas::Projection sv(new atlas::projection::detail::GeostationarySatelliteViewProjection(sv_spec));
    // Log::info() << "proj = " << proj.spec() << std::endl;


    Log::info() << "r = " << maximum_radius(1e4, 1e4, proj) << std::endl;
    Log::info() << "r = " << maximum_radius(1e4, 1e4, sv) << std::endl;


    auto xp = get<double, long>(param, "XpInGridLengths");
    auto yp = get<double, long>(param, "YpInGridLengths");
    auto dx = get<double, long>(param, "dx");
    auto dy = get<double, long>(param, "dy");

    auto Nx = get<int>(param, "numberOfPointsAlongXAxis");
    auto Ny = get<int>(param, "numberOfPointsAlongYAxis");
    ASSERT(0 < Nx);
    ASSERT(0 < Ny);


    // scaling coefficients
    auto angularSize = 2. * std::asin(1. / Nr);  //< apparent angular size of the Earth

    double rx = angularSize / dx;  // [radian]
    double ry = angularSize / dx;  // [radian]

    size_t NN = 100.;
    std::vector<double> N(NN + 1);
    std::iota(N.begin(), N.end(), 0);
    std::for_each(N.begin(), N.end(), [Nx, NN](double& v) { v *= Nx / double(NN); });

    std::ofstream f("out.txt");

    for (int ix = 0; ix < Nx; ++ix) {
        for (int iy = 0; iy < Ny; ++iy) {
            Point2 xy = {(ix - xp) * rx, (iy - yp) * ry};
            xy        = xy * 1e-3 * height;

#if 0
            auto ll1 = proj.lonlat(xy);
            auto xy1 = proj.xy(ll1);
            Log::info() << "(x, y) = (" << xy.x() << ", " << xy.y() << ") "
                        << "(lo, la) = (" << ll1.lon() << ", " << ll1.lat() << ")\n"
                        << "(x, y) = (" << xy1.x() << ", " << xy1.y() << ")\n." << std::endl;
#endif

            auto ll2 = sv.lonlat(xy);
            auto xy2 = sv.xy(ll2);
#if 0
            Log::info() << "(x, y) = (" << xy.x() << ", " << xy.y() << ") "
                        << "(lo, la) = (" << ll2.lon() << ", " << ll2.lat() << ") "
                        << "(x, y) = (" << xy2.x() << ", " << xy2.y() << ")" << std::endl;
#endif
            if (std::isfinite(xy2.x()) && std::isfinite(xy2.y()) && std::isfinite(ll2.lon()) &&
                std::isfinite(ll2.lat())) {
                f << ll2.lon() << '\t' << ll2.lat() << '\n';
            }
        }
    }
    f.close();


    if (false) {
        auto xp = get<double, long>(param, "XpInGridLengths") - get<double, long>(param, "Xo");
        if (!get<bool>(param, "iScansPositively")) {
            xp = double(Nx - 1) - xp;
        }
    }


    if (false) {
        auto yp = get<double, long>(param, "YpInGridLengths") - get<double, long>(param, "Yo");
        if (!get<bool>(param, "jScansPositively")) {
            yp = double(Ny - 1) - yp;
        }
    }


    if (true) {
        auto max_x = geometric_maximum(1e-3, 1e-3, [&proj](double x) { return proj.lonlat({x, 0}).lon(); });
        auto max_y = geometric_maximum(1e-3, 1e-3, [&proj](double y) { return proj.lonlat({0, y}).lat(); });

        Log::info() << "max_x (proj) = " << max_x << std::endl;
        Log::info() << "max_y (proj) = " << max_y << std::endl;

        auto eps = 1e-9;
        auto n   = std::max({proj.lonlat({0, -max_y}).lat(), proj.lonlat({0, max_y}).lat()}) + eps;
        auto e   = std::max({proj.lonlat({-max_x, 0}).lon(), proj.lonlat({max_x, 0}).lon()}) + eps;
        auto s   = -n;
        auto w   = -e;

        Log::info() << util::BoundingBox(n, w, s, e) << std::endl;
    }


    if (true) {
        auto max_x = geometric_maximum(1e-3, 1e-3, [&sv](double x) { return sv.lonlat({x, 0}).lon(); });
        auto max_y = geometric_maximum(1e-3, 1e-3, [&sv](double y) { return sv.lonlat({0, y}).lat(); });

        Log::info() << "max_x (home) = " << max_x << std::endl;
        Log::info() << "max_y (home) = " << max_y << std::endl;

        auto eps = 1e-9;
        auto n   = std::max({sv.lonlat({0, -max_y}).lat(), sv.lonlat({0, max_y}).lat()}) + eps;
        auto e   = std::max({sv.lonlat({-max_x, 0}).lon(), sv.lonlat({max_x, 0}).lon()}) + eps;
        auto s   = -n;
        auto w   = -e;

        Log::info() << util::BoundingBox(n, w, s, e) << std::endl;
    }


    Log::info() << "." << std::endl;
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
    return 1856 * 1856;  // FIXME x_.size() * y_.size();
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
