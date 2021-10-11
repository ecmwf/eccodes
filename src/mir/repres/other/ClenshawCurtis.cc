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
#include <map>
#include <numeric>
#include <ostream>
#include <vector>

#include "eckit/types/Fraction.h"
#include "eckit/utils/MD5.h"

// temporary
#if defined(mir_HAVE_ATLAS)
#include "atlas/grid/detail/spacing/CustomSpacing.h"
#endif

#include "mir/api/MIREstimation.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Mutex.h"
#include "mir/util/Trace.h"
#include "mir/util/Types.h"


namespace mir {
namespace repres {
namespace other {


static RepresentationBuilder<ClenshawCurtis> __representation("reduced_cc");

static util::once_flag once;
static util::recursive_mutex* mtx                 = nullptr;
static std::map<size_t, std::vector<double> >* ml = nullptr;

static void init() {
    mtx = new util::recursive_mutex();
    ml  = new std::map<size_t, std::vector<double> >();
}


eckit::Fraction increment(long ni) {
    using t = eckit::Fraction::value_type;
    return {t(360), t(ni)};
}


util::BoundingBox correctBoundingBox(const std::vector<long>& pl, const std::vector<double>& latitudes) {
    auto mm = std::minmax_element(pl.begin(), pl.end());
    ASSERT(*(mm.first) > 0);

    Longitude w = Longitude::GREENWICH;
    Longitude e = Longitude::GLOBE - increment(*(mm.second));
    Latitude n  = latitudes.front();
    Latitude s  = latitudes.back();

    return {n, w, s, e};
}


ClenshawCurtis::ClenshawCurtis(size_t N) : Gridded(util::BoundingBox()), N_(N) {
    ASSERT(domain_.isGlobal() && domain_.west() == Longitude::GREENWICH);

    // pl as octahedral (temporary)
    ASSERT(N_ > 0);
    pl_.resize(2 * N_);  // temporary

    long ni = 20;
    for (size_t i = 0, j = 2 * N_ - 1; i < N_; ++i, --j) {
        pl_[i] = ni;
        pl_[j] = ni;
        ni += 4;
    }

    // Reset bounding box
    bbox_ = correctBoundingBox(pl_, latitudes());
}


ClenshawCurtis::ClenshawCurtis(const param::MIRParametrisation& parametrisation) : Gridded(parametrisation), N_(0) {
    ASSERT(domain_.isGlobal() && domain_.west() == Longitude::GREENWICH);

    ASSERT(parametrisation.get("N", N_));
    ASSERT(N_ > 0);

    ASSERT(parametrisation.get("pl", pl_));
    ASSERT(pl_.size() == 2 * N_);  // temporary

    for (auto ni : pl_) {
        ASSERT(ni > 0);
    }

    // Reset bounding box
    bbox_ = correctBoundingBox(pl_, latitudes());
}


ClenshawCurtis::~ClenshawCurtis() = default;


bool ClenshawCurtis::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ClenshawCurtis*>(&other);
    return (o != nullptr) && (N_ == o->N_) && (pl_ == o->pl_) && (domain() == o->domain());
}


atlas::Grid ClenshawCurtis::atlasGrid() const {
#if defined(mir_HAVE_ATLAS)
    using grid_t = atlas::StructuredGrid;

    auto& lats = latitudes();
    ASSERT(!lats.empty());

    grid_t::XSpace x({0., 360.}, pl_, false);
    grid_t::YSpace y = new atlas::grid::spacing::CustomSpacing(long(lats.size()), lats.data());

    return grid_t(new grid_t::grid_t(x, y, atlas::Projection(), domain_));
#else
    NOTIMP;
#endif
}


void ClenshawCurtis::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();

    Log::debug() << "ClenshawCurtis::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << " (" << domain() << ")." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("ClenshawCurtis", values.size(), count);
}


bool ClenshawCurtis::extendBoundingBoxOnIntersect() const {
    return false;
}


std::vector<double> ClenshawCurtis::calculateUnrotatedGridBoxLatitudeEdges() const {

    // grid-box edge latitudes are the latitude midpoints
    auto& lats = latitudes();
    ASSERT(!lats.empty());

    std::vector<double> edges;
    edges.reserve(lats.size() + 1);

    edges.push_back(Latitude::NORTH_POLE.value());
    for (auto b = lats.begin(), a = b++; b != lats.end(); a = b++) {
        edges.push_back((*b + *a) / 2.);
    }
    edges.push_back(Latitude::SOUTH_POLE.value());

    return edges;
}


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
    util::call_once(once, init);
    util::lock_guard<util::recursive_mutex> lock(*mtx);

    ASSERT(N > 0);

    auto k = ml->find(N);
    if (k == ml->end()) {
        trace::Timer timer("ClenshawCurtis latitudes " + std::to_string(N), Log::debug());

        // calculate latitudes and save in map
        auto& lats = (*ml)[N];
        lats.resize(2 * N);  // temporary

        for (size_t i = 0, j = 2 * N - 1; i < N; ++i, --j) {
            double lat = 90. - 90. * double(i + 1) / double(N);
            lats[i]    = lat;
            lats[j]    = -lat;
        }
        lats[N - 1] = lats[N] = 0.;

        k = ml->find(N);
        ASSERT(k != ml->end());
    }

    return k->second;
}


void ClenshawCurtis::fill(grib_info& info) const {
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REDUCED_GG;

    info.grid.Nj      = long(pl_.size());
    info.grid.N       = long(N_);
    info.grid.pl      = pl_.data();
    info.grid.pl_size = long(pl_.size());

    bbox_.fill(info);
}


void ClenshawCurtis::estimate(api::MIREstimation& estimation) const {
    Gridded::estimate(estimation);
    estimation.pl(pl_.size());
}


std::vector<util::GridBox> ClenshawCurtis::gridBoxes() const {

    // latitude edges
    std::vector<double> latEdges = calculateUnrotatedGridBoxLatitudeEdges();
    ASSERT(!latEdges.empty());


    // grid boxes
    std::vector<util::GridBox> r;
    r.reserve(numberOfPoints());


    ASSERT(!pl_.empty());
    for (size_t j = 0; j < pl_.size(); ++j) {
        ASSERT(pl_[j] > 0);

        auto inc = increment(pl_[j]);
        auto ni  = size_t(pl_[j]);

        // longitude edges
        auto west = bbox_.west().fraction();
        auto Nw   = (west / inc).integralPart();
        if (Nw * inc < west) {
            Nw += 1;
        }
        Longitude lon0 = (Nw * inc) - (inc / 2);
        Longitude lon1 = lon0;

        for (size_t i = 0; i < ni; ++i) {
            auto l = lon1;
            lon1 += inc;
            r.emplace_back(util::GridBox(latEdges[j], l.value(), latEdges[j + 1], lon1.value()));
        }

        ASSERT(lon0 == lon1.normalise(lon0));
    }

    ASSERT(r.size() == numberOfPoints());
    return r;
}


size_t ClenshawCurtis::numberOfPoints() const {
    return size_t(std::accumulate(pl_.begin(), pl_.end(), 0));
}


bool ClenshawCurtis::getLongestElementDiagonal(double& d) const {

    // Look for a majorant of all element diagonals, using the difference of
    // latitudes closest/furthest from equator and longitude furthest from
    // Greenwich

    auto& lats = latitudes();

    d = 0.;
    for (size_t j = 1; j < pl_.size(); ++j) {

        Latitude l1(lats[j - 1]);
        Latitude l2(lats[j]);

        auto inc = increment(std::min(pl_[j - 1], pl_[j]));
        auto& latAwayFromEquator(std::abs(l1.value()) > std::abs(l2.value()) ? l1 : l2);
        auto& latCloserToEquator(std::abs(l1.value()) > std::abs(l2.value()) ? l2 : l1);

        d = std::max(d, util::Earth::distance(atlas::PointLonLat(0., latCloserToEquator.value()),
                                              atlas::PointLonLat(inc, latAwayFromEquator.value())));
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
            pl_(pl), latitudes_(latitudes), i_(0), j_(0), count_(0), first_(true) {
            ASSERT(pl.size() == latitudes.size());

            nj_ = pl.size();
            ASSERT(nj_ > 0);

            ni_ = size_t(pl_.front());
            ASSERT(ni_ > 0);

            lat_ = latitudes_.front();
            lon_ = Longitude::GREENWICH;
            inc_ = increment(long(ni_));
        }

        ClenshawCurtisIterator(const ClenshawCurtisIterator&) = delete;
        void operator=(const ClenshawCurtisIterator&) = delete;

        void print(std::ostream& out) const override {
            out << "ClenshawCurtisIterator[";
            Iterator::print(out);
            out << ",ni=" << ni_ << ",nj=" << nj_ << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& lat, Longitude& lon) override {
            if (j_ >= nj_) {
                return false;
            }

            lat = lat_;
            lon = lon_;

            if (first_) {
                first_ = false;
            }
            else {
                count_++;
            }

            if (++i_ < ni_) {
                lon_ += inc_;
            }
            else if (++j_ < nj_) {
                i_  = 0;
                ni_ = size_t(pl_[j_]);
                ASSERT(ni_ > 1);

                lat_ = latitudes_[j_];
                lon_ = Longitude::GREENWICH;
                inc_ = increment(long(ni_));
            }

            return true;
        }

        size_t index() const override { return count_; }

        const std::vector<long>& pl_;
        const std::vector<double>& latitudes_;

        size_t ni_;
        size_t nj_;
        size_t i_;
        size_t j_;
        size_t count_;
        bool first_;

        eckit::Fraction inc_;
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
