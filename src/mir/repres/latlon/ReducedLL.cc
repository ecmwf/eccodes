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

#include "mir/repres/latlon/ReducedLL.h"

#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/MD5.h"
#include "mir/api/Atlas.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"
#include <algorithm>
#include <iostream>

namespace mir {
namespace repres {
namespace latlon {

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
    for (auto j = pl_.begin(); j != pl_.end(); ++j) {
        md5 << *j;
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
    const bool periodic = dom.isPeriodicWestEast();

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

        const Latitude &latAwayFromEquator(std::abs(lat1.value()) > std::abs(lat2.value()) ? lat1 : lat2),
            latCloserToEquator(std::abs(lat1.value()) > std::abs(lat2.value()) ? lat2 : lat1);

        d = std::max(d, atlas::util::Earth::distance(atlas::PointLonLat(0., latCloserToEquator.value()),
                                                     atlas::PointLonLat(we, latAwayFromEquator.value())));

        lat1 = lat2;
        lat2 -= sn;
    }

    ASSERT(d > 0.);
    return true;
}

bool ReducedLL::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ReducedLL*>(&other);
    return o && (bbox_ == o->bbox_) && (pl_ == o->pl_);
}

void ReducedLL::fill(grib_info& info) const {
    NOTIMP;
}

void ReducedLL::fill(api::MIRJob& job) const {
    bbox_.fill(job);
    job.set("pl", pl_);
    job.set("Nj", pl_.size());
    NOTIMP;
}

atlas::Grid ReducedLL::atlasGrid() const {
    const util::Domain dom = domain();

    atlas::StructuredGrid::XSpace xspace({{dom.west().value(), dom.east().value()}}, pl_, !dom.isPeriodicWestEast());
    atlas::StructuredGrid::YSpace yspace(
        atlas::grid::LinearSpacing({{dom.north().value(), dom.south().value()}}, pl_.size()));

    return atlas::StructuredGrid(xspace, yspace);
}

void ReducedLL::fill(util::MeshGeneratorParameters& params) const {
    params.meshGenerator_ = "structured";

    params.meshGenerator_ = "structured";
    if (boundingBox().south() > Latitude::EQUATOR) {
        params.set("force_include_south_pole", true);
    }
    if (boundingBox().north() < Latitude::EQUATOR) {
        params.set("force_include_north_pole", true);
    }
}

bool ReducedLL::isPeriodicWestEast() const {
    ASSERT(pl_.size());
    const long maxpl = *std::max_element(pl_.begin(), pl_.end());

    auto same_with_grib1_accuracy = [&](const Longitude& a, const Longitude& b) {
        static const double GRIB1EPSILON = 0.001;
        return eckit::types::is_approximately_equal(a.value(), b.value(), GRIB1EPSILON);
    };

    const Longitude we = bbox_.east() - bbox_.west();
    const Longitude inc = Longitude::GLOBE - we;

    return same_with_grib1_accuracy(inc * maxpl, Longitude::GLOBE);
}

bool ReducedLL::includesNorthPole() const {
    return bbox_.north() == Latitude::NORTH_POLE;
}

bool ReducedLL::includesSouthPole() const {
    return bbox_.south() == Latitude::SOUTH_POLE;
}

void ReducedLL::validate(const MIRValuesVector& values) const {
    size_t count = 0;
    for (size_t i = 0; i < pl_.size(); i++) {
        count += pl_[i];
    }
    ASSERT(values.size() == count);
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

    eckit::Fraction lat_;
    eckit::Fraction lon_;

    size_t i_;
    size_t j_;
    size_t p_;

    size_t count_;
    bool periodic_;

    virtual void print(std::ostream& out) const {
        out << "ReducedLLIterator[";
        Iterator::print(out);
        out << ",domain=" << domain_ << ",ni=" << ni_ << ",nj=" << nj_ << ",i=" << i_ << ",j=" << j_ << ",p=" << p_
            << ",count=" << count_ << "]";
    }

    virtual bool next(Latitude& lat, Longitude& lon) {

        while (j_ < nj_ && i_ < ni_) {

            lat = lat_;
            lon = lon_;

            i_++;
            lon_ += inc_west_east_;

            if (i_ == ni_) {

                j_++;
                lat_ -= inc_north_south_;
                lon_ = west_;

                i_ = 0;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = pl_[p_++];
                    ASSERT(ni_ > 1);
                    inc_west_east_ = ew_ / (ni_ - (periodic_ ? 0 : 1));
                }
            }

            if (domain_.contains(lat, lon)) {
                count_++;
                return true;
            }
        }
        return false;
    }

public:
    ReducedLLIterator(const std::vector<long>& pl, const util::Domain& dom)
        : pl_(pl)
        , nj_(pl.size())
        , domain_(dom)
        ,

        west_(domain_.west().fraction())
        ,

        ew_((domain_.east() - domain_.west()).fraction())
        ,

        inc_north_south_((domain_.north() - domain_.south()).fraction() / eckit::Fraction(nj_ - 1))
        ,

        lat_(domain_.north().fraction())
        , lon_(west_)
        , i_(0)
        , j_(0)
        , p_(0)
        , count_(0)
        , periodic_(dom.isPeriodicWestEast()) {

        ASSERT(nj_ > 1);

        ni_ = pl_[p_++];
        ASSERT(ni_ > 1);
        inc_west_east_ = ew_ / (ni_ - (periodic_ ? 0 : 1));

        // eckit::Log::debug<LibMir>() << *this << std::endl;
    }
};

Iterator* ReducedLL::iterator() const {
    return new ReducedLLIterator(pl_, domain());
}

namespace {
static RepresentationBuilder<ReducedLL> reducedLL("reduced_ll"); // Name is what is returned by grib_api
}

} // namespace latlon
} // namespace repres
} // namespace mir
