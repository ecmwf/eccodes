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


#include "mir/repres/other/ClenshawCurtis.h"

#include <algorithm>
#include <cmath>
//#include <limits>
#include <map>
//#include <memory>
#include <mutex>
#include <numeric>
#include <ostream>
#include <string>
//#include <type_traits>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/MD5.h"

//#include "mir/api/Atlas.h"
//#include "mir/util/GridBox.h"
#include "mir/api/MIREstimation.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Angles.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Pretty.h"


namespace mir {
namespace repres {
namespace other {


static RepresentationBuilder<ClenshawCurtis> __representation("reduced_cc");

static pthread_once_t once                        = PTHREAD_ONCE_INIT;
static std::mutex* mtx                            = nullptr;
static std::map<size_t, std::vector<double> >* ml = nullptr;

static void init() {
    mtx = new std::mutex();
    ml  = new std::map<size_t, std::vector<double> >();
}


ClenshawCurtis::ClenshawCurtis(size_t N) : Gridded(util::BoundingBox()), N_(N) {
    ASSERT(domain_.isGlobal() && domain_.west() == Longitude::GREENWICH.fraction());

    ASSERT(N_ > 0);
    pl_.resize(2 * N_);

    long n = 20;
    for (size_t i = 0, j = 2 * N_ - 1; i < N_; ++i, --j) {
        pl_[i] = n;
        pl_[j] = n;
        n += 4;
    }
}


ClenshawCurtis::ClenshawCurtis(const param::MIRParametrisation& parametrisation) : Gridded(parametrisation), N_(0) {
    ASSERT(domain_.isGlobal() && domain_.west() == Longitude::GREENWICH.fraction());

    ASSERT(parametrisation.get("N", N_));
    ASSERT(N_ > 0);

    ASSERT(parametrisation.get("pl", pl_));
    ASSERT(pl_.size() == 2 * N_);

    ASSERT(*std::min_element(pl_.begin(), pl_.end()) > 0);
}


ClenshawCurtis::~ClenshawCurtis() = default;


bool ClenshawCurtis::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ClenshawCurtis*>(&other);
    return (o != nullptr) && (N_ == o->N_) && (pl_ == o->pl_) && (domain() == o->domain());
}


atlas::Grid ClenshawCurtis::atlasGrid() const {
    return atlas::ReducedGaussianGrid(pl_, domain());
}


void ClenshawCurtis::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();

    eckit::Log::debug<LibMir>() << "ClenshawCurtis::validate checked " << Pretty(values.size(), {"value"})
                                << ", within domain: " << Pretty(count) << "." << std::endl;
    ASSERT(values.size() == count);
}


bool ClenshawCurtis::extendBoundingBoxOnIntersect() const {
    return false;
}


// std::vector<double> ClenshawCurtis::calculateUnrotatedGridBoxLatitudeEdges() const {

//    // grid-box edge latitudes are the latitude midpoints
//    size_t Nj = N_ * 2;
//    ASSERT(Nj > 1);

//    auto& w = weights();
//    ASSERT(w.size() == Nj);

//    std::vector<double> edges(Nj + 1);
//    auto f = edges.begin();
//    auto b = edges.rbegin();

//    *(f++) = Latitude::NORTH_POLE.value();
//    *(b++) = Latitude::SOUTH_POLE.value();

//    double wacc = -1.;
//    for (size_t j = 0; j < N_; ++j, ++b, ++f) {
//        wacc += 2. * w[j];
//        double deg = util::radian_to_degree(std::asin(wacc));
//        ASSERT(Latitude::SOUTH_POLE.value() <= deg && deg <= Latitude::NORTH_POLE.value());

//        *b = deg;
//        *f = -(*b);
//    }

//    return edges;
//}


void ClenshawCurtis::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }

    const Latitude& s = bbox_.south();
    if (s <= latitudes().back() || s > Latitude::EQUATOR) {
        params.set("force_include_south_pole", true);
    }

    const Latitude& n = bbox_.north();
    if (n >= latitudes().front() || n < Latitude::EQUATOR) {
        params.set("force_include_north_pole", true);
    }
}


const std::vector<double>& ClenshawCurtis::latitudes(size_t N) {
    pthread_once(&once, init);
    std::lock_guard<std::mutex> lock(*mtx);

    ASSERT(N > 0);

    auto j = ml->find(N);
    if (j == ml->end()) {
        eckit::Timer timer("ClenshawCurtis latitudes " + std::to_string(N), eckit::Log::debug<LibMir>());

        // calculate latitudes and insert in known-N-latitudes map
        auto& lats = (*ml)[N];
        lats.resize(2 * N);

        auto f = std::acos(-1.) / double(2 * N);

        for (size_t i = 0, j = 2 * N - 1; i < N; ++i, --j) {
            double theta = f * double(i + 1);
            double latr  = std::asin(std::cos(theta));
            double lat   = util::radian_to_degree(latr);

            lats[i] = lat;
            lats[j] = -lat;
        }
        lats[N - 1] = lats[N] = 0.;

        j = ml->find(N);
        ASSERT(j != ml->end());
    }


    // these are the assumptions we expect from the ClenshawCurtis latitudes values
    return j->second;
}


// eckit::Fraction ClenshawCurtis::getSmallestIncrement() const {
//    ASSERT(N_);
//    using distance_t = std::make_signed<size_t>::type;

//    const std::vector<long>& pl = pl_;
//    const long maxpl            = *std::max_element(pl.begin(), pl.end());
//    ASSERT(maxpl > 0);

//    return Longitude::GLOBE.fraction() / maxpl;
//}


void ClenshawCurtis::fill(grib_info& info) const {
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj        = long(pl_.size());
    info.grid.N         = long(N_);
    info.grid.pl        = pl_.data();
    info.grid.pl_size   = long(pl_.size());

    bbox_.fill(info);
}


void ClenshawCurtis::estimate(api::MIREstimation& estimation) const {
    ClenshawCurtis::estimate(estimation);
    estimation.pl(pl_.size());
}


// std::vector<util::GridBox> ClenshawCurtis::gridBoxes() const {
//    NOTIMP;
//}


size_t ClenshawCurtis::numberOfPoints() const {
    return size_t(std::accumulate(pl_.begin(), pl_.end(), 0));
}


bool ClenshawCurtis::getLongestElementDiagonal(double& d) const {

    // Look for a majorant of all element diagonals, using the difference of
    // latitudes closest/furthest from equator and longitude furthest from
    // Greenwich

    auto& lats = latitudes();

    d = 0.;
    for (size_t j = 0; j < pl_.size(); ++j) {

        Latitude l1(lats[j - 1]);
        Latitude l2(lats[j]);

        const eckit::Fraction we = Longitude::GLOBE.fraction() / (std::min(pl_[j - 1], pl_[j]));
        auto& latAwayFromEquator(std::abs(l1.value()) > std::abs(l2.value()) ? l1 : l2);
        auto& latCloserToEquator(std::abs(l1.value()) > std::abs(l2.value()) ? l2 : l1);

        d = std::max(d, atlas::util::Earth::distance(atlas::PointLonLat(0., latCloserToEquator.value()),
                                                     atlas::PointLonLat(we, latAwayFromEquator.value())));
    }

    ASSERT(d > 0.);
    return true;
}


// util::BoundingBox ClenshawCurtis::extendBoundingBox(const util::BoundingBox&) const {
//    NOTIMP;
//}


void ClenshawCurtis::print(std::ostream& out) const {
    out << "ClenshawCurtis[N=" << N_ << ",bbox=" << bbox_ << "]";
}


Iterator* ClenshawCurtis::iterator() const {
    struct ClenshawCurtisIterator : public Iterator {

        ClenshawCurtisIterator(const std::vector<long>& pl, const std::vector<double>& latitudes) :
            pl_(pl),
            latitudes_(latitudes),
            i_(0),
            j_(0),
            count_(0) {
            ASSERT(pl.size() == latitudes.size());

            nj_ = pl.size();
            ASSERT(nj_ > 0);

            ni_ = size_t(pl_.front());
            ASSERT(ni_ > 1);

            latitude_      = latitudes.front();
            longitude_     = Longitude::GREENWICH.fraction();
            inc_west_east_ = {static_cast<eckit::Fraction::value_type>(360),
                              static_cast<eckit::Fraction::value_type>(ni_)};
        }

        ClenshawCurtisIterator(const ClenshawCurtisIterator&) = delete;
        void operator=(const ClenshawCurtisIterator&) = delete;

        void print(std::ostream& out) const {
            out << "ClenshawCurtisIterator[";
            Iterator::print(out);
            out << ",ni=" << ni_ << ",nj=" << nj_ << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& lat, Longitude& lon) {
            lat = latitude_;
            lon = longitude_;

            if (ni_ == 0) {
                return false;
            }
            else if (++i_ < ni_) {
                longitude_ += inc_west_east_;
            }
            else if (++j_ < nj_) {
                longitude_ = Longitude::GREENWICH.fraction();
                latitude_  = latitudes_[j_];

                i_  = 0;
                ni_ = size_t(pl_[j_]);
                ASSERT(ni_ > 1);

                inc_west_east_ = {static_cast<eckit::Fraction::value_type>(360),
                                  static_cast<eckit::Fraction::value_type>(ni_)};
            }
            else {
                ni_ = 0;
            }

            count_++;
            return true;
        }

        const std::vector<long>& pl_;
        const std::vector<double>& latitudes_;

        size_t nj_;
        size_t ni_;

        Latitude latitude_;
        eckit::Fraction longitude_;
        eckit::Fraction inc_west_east_;

        size_t i_;
        size_t j_;
        size_t count_;
    };

    return new ClenshawCurtisIterator(pl_, latitudes());
}


// const Gridded* ClenshawCurtis::croppedRepresentation(const util::BoundingBox&) const {
//    NOTIMP;
//}


void ClenshawCurtis::makeName(std::ostream& out) const {
    out << "RCC" << N_ << "-";

    eckit::MD5 md5;
    for (auto j : pl_) {
        md5 << j;
    }

    out << std::string(md5);
    bbox_.makeName(out);
}


util::Domain ClenshawCurtis::domain() const {
    return domain_;
}


}  // namespace other
}  // namespace repres
}  // namespace mir
