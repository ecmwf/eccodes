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


#include "mir/repres/latlon/ReducedLL.h"

#include <algorithm>
#include <cmath>
#include <ostream>
#include <sstream>

#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/MD5.h"

#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/GridBox.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Types.h"


namespace mir::repres::latlon {

static bool checkPl(const std::vector<long>& pl) {
    ASSERT(!pl.empty());
    return *std::min_element(pl.begin(), pl.end()) >= 2;
}

ReducedLL::ReducedLL(const param::MIRParametrisation& parametrisation) : Gridded(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
    checkPl(pl_);

    size_t Nj = 0;
    ASSERT(parametrisation.get("Nj", Nj));
    ASSERT(Nj == pl_.size());
}

ReducedLL::~ReducedLL() = default;

void ReducedLL::print(std::ostream& out) const {
    out << "ReducedLL[bbox=" << bbox_ << "]";
}

void ReducedLL::makeName(std::ostream& out) const {
    out << "RLL" << pl_.size() << "-";

    eckit::MD5 md5;
    for (const auto& j : pl_) {
        md5 << j;
    }

    out << std::string(md5);
    bbox_.makeName(out);
}

size_t ReducedLL::numberOfPoints() const {
    size_t total = 0;
    for (const auto& j : pl_) {
        total += size_t(j);
    }
    return total;
}

bool ReducedLL::getLongestElementDiagonal(double& d) const {

    // Look for a majorant of all element diagonals, using the difference of
    // latitudes closest/furthest from equator and longitude furthest from
    // Greenwich

    const util::Domain dom = domain();
    const bool periodic    = dom.isPeriodicWestEast();

    ASSERT(pl_.size() >= 2);
    const size_t Dj(pl_.size() - 1);
    ASSERT(Dj > 0);
    const eckit::Fraction sn(((dom.north() - dom.south()).fraction()) / Dj);

    d = 0.;
    Latitude lat1(dom.north());
    Latitude lat2(dom.north() - sn);

    for (size_t j = 1; j < pl_.size(); ++j) {

        const long Di(std::min(pl_[j - 1], pl_[j]) - (periodic ? 0 : 1));
        ASSERT(Di > 0);
        const eckit::Fraction we((dom.east() - dom.west()).fraction() / Di);

        auto& latAwayFromEquator(std::abs(lat1.value()) > std::abs(lat2.value()) ? lat1 : lat2);
        auto& latCloserToEquator(std::abs(lat1.value()) > std::abs(lat2.value()) ? lat2 : lat1);

        d = std::max(d, util::Earth::distance(atlas::PointLonLat(0., latCloserToEquator.value()),
                                              atlas::PointLonLat(we, latAwayFromEquator.value())));

        lat1 = lat2;
        lat2 -= sn;
    }

    ASSERT(d > 0.);
    return true;
}

bool ReducedLL::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const ReducedLL*>(&other);
    return (o != nullptr) && (bbox_ == o->bbox_) && (pl_ == o->pl_);
}

void ReducedLL::fillGrib(grib_info& /*unused*/) const {
    NOTIMP;
}

void ReducedLL::fillJob(api::MIRJob& job) const {
    bbox_.fillJob(job);
    job.set("pl", pl_);
    job.set("Nj", pl_.size());
    NOTIMP;
}

atlas::Grid ReducedLL::atlasGrid() const {
#if mir_HAVE_ATLAS
    const util::Domain dom = domain();
    auto N                 = long(pl_.size());

    atlas::StructuredGrid::XSpace xspace({{dom.west().value(), dom.east().value()}}, pl_, !dom.isPeriodicWestEast());
    atlas::StructuredGrid::YSpace yspace(atlas::grid::LinearSpacing({{dom.north().value(), dom.south().value()}}, N));

    return atlas::StructuredGrid(xspace, yspace);
#else
    NOTIMP;
#endif
}

void ReducedLL::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
    if (boundingBox().south() > Latitude::EQUATOR) {
        params.set("force_include_south_pole", true);
    }
    if (boundingBox().north() < Latitude::EQUATOR) {
        params.set("force_include_north_pole", true);
    }
}

bool ReducedLL::isPeriodicWestEast() const {
    ASSERT(!pl_.empty());

    auto maxpl = *std::max_element(pl_.begin(), pl_.end());
    ASSERT(maxpl >= 2);

    // if range West-East is within one increment (or greater than) 360 degree
    const eckit::Fraction inc(360, maxpl);
    return bbox_.east() - bbox_.west() + inc >= Longitude::GLOBE;
}

bool ReducedLL::includesNorthPole() const {
    return bbox_.north() == Latitude::NORTH_POLE;
}

bool ReducedLL::includesSouthPole() const {
    return bbox_.south() == Latitude::SOUTH_POLE;
}

void ReducedLL::validate(const MIRValuesVector& values) const {
    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("ReducedLL", values.size(), numberOfPoints());
}

class ReducedLLIterator : public Iterator {

    const std::vector<long>& pl_;
    const size_t nj_;
    size_t ni_;

    const util::Domain domain_;
    const eckit::Fraction west_;
    const eckit::Fraction ew_;
    eckit::Fraction inc_west_east_;
    const eckit::Fraction inc_north_south_;

    eckit::Fraction latitude_;
    eckit::Fraction longitude_;

    size_t i_;
    size_t j_;
    size_t p_;

    size_t count_;
    bool first_;
    bool periodic_;

    void print(std::ostream& out) const override {
        out << "ReducedLLIterator[";
        Iterator::print(out);
        out << ",domain=" << domain_ << ",ni=" << ni_ << ",nj=" << nj_ << ",i=" << i_ << ",j=" << j_ << ",p=" << p_
            << ",count=" << count_ << "]";
    }

    bool next(Latitude& lat, Longitude& lon) override {
        while (j_ < nj_ && i_ < ni_) {
            lat = latitude_;
            lon = longitude_;

            bool contains = domain_.contains(lat, lon);
            if (contains && !first_) {
                count_++;
            }

            longitude_ += inc_west_east_;

            if (++i_ == ni_) {
                ++j_;
                i_ = 0;
                latitude_ -= inc_north_south_;
                longitude_ = west_;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = size_t(pl_[p_++]);
                    ASSERT(ni_ > 1);
                    inc_west_east_ = ew_ / (ni_ - (periodic_ ? 0 : 1));
                }
            }

            if (contains) {
                first_ = false;
                return true;
            }
        }
        return false;
    }

    size_t index() const override { return count_; }

public:
    ReducedLLIterator(const std::vector<long>& pl, const util::Domain& dom) :
        pl_(pl),
        nj_(pl.size()),
        domain_(dom),
        west_(domain_.west().fraction()),
        ew_((domain_.east() - domain_.west()).fraction()),
        inc_north_south_((domain_.north() - domain_.south()).fraction() / eckit::Fraction(nj_ - 1)),
        latitude_(domain_.north().fraction()),
        longitude_(west_),
        i_(0),
        j_(0),
        p_(0),
        count_(0),
        first_(true),
        periodic_(dom.isPeriodicWestEast()) {

        ASSERT(nj_ > 1);

        ni_ = size_t(pl_[p_++]);
        ASSERT(ni_ > 1);
        inc_west_east_ = ew_ / (ni_ - (periodic_ ? 0 : 1));
    }
};

Iterator* ReducedLL::iterator() const {
    return new ReducedLLIterator(pl_, domain());
}

std::vector<util::GridBox> ReducedLL::gridBoxes() const {
    auto dom      = domain();
    bool periodic = isPeriodicWestEast();

    auto Nj                  = pl_.size();
    const eckit::Fraction sn = (dom.north() - dom.south()).fraction() / eckit::Fraction(Nj - 1);
    eckit::Fraction half(1, 2);


    // grid boxes
    std::vector<util::GridBox> r;
    r.reserve(numberOfPoints());


    // latitude edges
    std::vector<double> latEdges(Nj + 1);

    auto lat0   = bbox_.north();
    latEdges[0] = (lat0 + sn / 2).value();
    for (size_t j = 0; j < Nj; ++j) {
        latEdges[j + 1] = (lat0 - (j + half) * sn).value();
    }

    latEdges.front() = std::min(dom.north().value(), latEdges.front());
    latEdges.back()  = std::max(dom.south().value(), latEdges.back());


    for (size_t j = 0; j < Nj; ++j) {

        // longitude edges
        auto Ni = pl_[j];
        ASSERT(Ni > 1);
        const eckit::Fraction we = (dom.east() - dom.west()).fraction() / (Ni - (periodic ? 0 : 1));

        auto lon0 = bbox_.west() - we / 2;
        auto lon1 = lon0;

        if (periodic) {
            for (long i = 0; i < Ni; ++i) {
                auto w = lon1.value();
                lon1 += we;
                r.emplace_back(latEdges[j], w, latEdges[j + 1], lon1.value());
            }

            ASSERT(lon0 == lon1.normalise(lon0));
        }
        else {
            for (long i = 0; i < Ni; ++i) {
                auto w = std::max(bbox_.west().value(), lon1.value());
                lon1 += we;
                auto e = std::min(bbox_.east().value(), lon1.value());
                r.emplace_back(latEdges[j], w, latEdges[j + 1], e);
            }

            ASSERT(lon0 <= lon1.normalise(lon0));
        }
    }

    ASSERT(r.size() == numberOfPoints());
    return r;
}


const Representation* mir::repres::latlon::ReducedLL::croppedRepresentation(const util::BoundingBox&) const {
    std::ostringstream os;
    os << "ReducedLL::croppedRepresentation() not supported for " << *this;
    throw exception::FunctionalityNotSupported(os.str());
}


static const RepresentationBuilder<ReducedLL> reducedLL("reduced_ll");


}  // namespace mir::repres::latlon
