/*
 * (C) Copyright 1996-2016 ECMWF.
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


#include "mir/repres/gauss/regular/Regular.h"

#include <cmath>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Plural.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/utils/MD5.h"

#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/GreatCircle.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace regular {


Regular::Regular(const param::MIRParametrisation& parametrisation):
    Gaussian(parametrisation) {
    adjustBoundingBoxEastWest(bbox_);
    setNiNj();
}


Regular::Regular(size_t N):
    Gaussian(N) {
    adjustBoundingBoxEastWest(bbox_);
    setNiNj();
}


Regular::Regular(size_t N, const util::BoundingBox& bbox):
    Gaussian(N, bbox) {
    adjustBoundingBoxEastWest(bbox_);
    setNiNj();
}


Regular::~Regular() {
}


void Regular::fill(grib_info& info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;

    info.grid.N = N_;
    info.grid.iDirectionIncrementInDegrees = 90.0 / N_;
    info.grid.Ni = Ni_;
    info.grid.Nj = Nj_;

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    // for GRIB, a global field is also aligned with Greenwich
    bool westAtGreenwich = bbox_.west() == Longitude::GREENWICH;

    long j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = isGlobal() && westAtGreenwich ? 1 : 0;
}


void Regular::fill(api::MIRJob& job) const  {
    std::stringstream os;
    os << "F" << N_;
    job.set("gridname", os.str());
}


void Regular::makeName(std::ostream& out) const {
    out << "F" << N_;
    bbox_.makeName(out);
}


bool Regular::sameAs(const Representation& other) const {
    const Regular* o = dynamic_cast<const Regular*>(&other);
    return o && (N_ == o->N_) && (bbox_ == o->bbox_);
}


eckit::Fraction Regular::getSmallestIncrement() const {
    ASSERT(N_);
    return eckit::Fraction(90, N_);
}


void Regular::adjustBoundingBoxEastWest(util::BoundingBox& bbox) {
    Longitude e = bbox.east();
    Longitude w = bbox.west();

    bool adjustedEast = false;
    bool adjustedWest = false;

    eckit::Fraction inc = getSmallestIncrement();
    if (e - w > Longitude::GLOBE - inc) {
        adjustedEast = true;
        e = w + Longitude::GLOBE - inc;
    }

    const long range = 4 * long(N_);
    for (long i = -range; i <= range; ++i) {
        const Longitude l = w - i * inc;
        if (!adjustedEast && bbox.east().sameWithGrib1Accuracy(l)) {
            adjustedEast = true;
            e = l;
        }
        if (!adjustedWest && bbox.west().sameWithGrib1Accuracy(l)) {
            adjustedWest = true;
            w = l;
        }
        if (adjustedEast && adjustedWest) {
            break;
        }
    }

    bbox = util::BoundingBox(bbox.north(), w, bbox.south(), e);
}


bool Regular::isPeriodicWestEast() const {
    const Longitude inc = Longitude(eckit::Fraction(90, N_));
    return (bbox_.east() - bbox_.west() + inc).sameWithGrib1Accuracy(360.0);
}


size_t Regular::numberOfPoints() const {
    if (isGlobal()) {
        ASSERT(Nj_ == N_ * 2);
        ASSERT(Ni_ == N_ * 4);
        return Ni_ * Nj_;
    }
    else {
        size_t total = 0;
        eckit::ScopedPtr<repres::Iterator> iter(iterator());
        while (iter->next()) {
            total++;
        }
        return total;
    }
}


double Regular::longestElementDiagonal() const {

    // Look for a majorant of all element diagonals, using the difference of
    // latitudes closest/furthest from equator and longitude furthest from
    // Greenwich

    const std::vector<double>& lats = latitudes();
    ASSERT(N_ * 2 == lats.size());
    ASSERT(N_);

    double d = 0.;
    Latitude l1(Latitude::NORTH_POLE);
    Latitude l2(lats[0]);

    for (size_t j = 1; j < lats.size(); ++j, l1 = l2, l2 = lats[j]) {

        const eckit::Fraction we = Longitude::GLOBE.fraction() / (N_ * 4);
        const Latitude&
                latAwayFromEquator(std::abs(l1.value()) > std::abs(l2.value())? l1 : l2),
                latCloserToEquator(std::abs(l1.value()) > std::abs(l2.value())? l2 : l1);

        d = std::max(d, util::GreatCircle::distanceInMeters(
                         Iterator::point_ll_t(latCloserToEquator, 0),
                         Iterator::point_ll_t(latAwayFromEquator, we) ));
    }

    ASSERT(d > 0.);
    return d;

}


atlas::Grid Regular::atlasGrid() const {
    return atlas::grid::RegularGaussianGrid("F" + std::to_string(N_), domain());
}


void Regular::validate(const std::vector<double>& values) const {
    const size_t count = numberOfPoints();

    eckit::Log::debug<LibMir>() << "Regular::validate checked " << eckit::Plural(values.size(), "value") << ", within domain: " << eckit::BigNum(count) << "." << std::endl;
    ASSERT(values.size() == size_t(count));
}


void Regular::setNiNj() {
    const util::Domain dom = domain();

    Ni_ = N_ * 4;
    if (!dom.isPeriodicEastWest()) {
        const Latitude lat_middle = (dom.north() + dom.south()) / 2.;
        const eckit::Fraction inc(90, N_);

        Ni_ = 0;
        const eckit::Fraction west = dom.west().fraction();
        for (size_t i = 0; i < N_ * 4; ++i) {
            const eckit::Fraction lon = west + i * inc;
            if (dom.contains(lat_middle, lon)) {
                ++Ni_;
            }
        }
    }
    ASSERT(2 <= Ni_ && Ni_ <= N_ * 4);

    Nj_ = N_ * 2;
    if (!dom.includesPoleNorth() || !dom.includesPoleSouth()) {
        const Longitude lon_middle = (dom.west() + dom.east()) / 2.;

        Nj_ = 0;
        for (const double& lat : latitudes()) {
            if (dom.contains(lat, lon_middle)) {
                ++Nj_;
            }
        }
    }
    ASSERT(2 <= Nj_ && Nj_ <= N_ * 2);
}


size_t Regular::frame(std::vector<double>& values, size_t size, double missingValue) const {

    // TODO: Check if that logic cannot also be used for other grid, and therefore move it to a higher class
    validate(values);

    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < Nj_; j++) {
        for (size_t i = 0; i < Ni_; i++) {
            if (!((i < size) || (j < size) || (i >= Ni_ - size) || (j >= Nj_ - size))) { // Check me, may be buggy
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;
}


void Regular::shape(size_t& ni, size_t& nj) const {
    ni = Ni_;
    nj = Nj_;
}


Regular::RegularIterator::RegularIterator(const std::vector<double>& latitudes, size_t N, size_t Ni, size_t Nj, const util::Domain& dom) :
    latitudes_(latitudes),
    west_(dom.west().fraction()),
    N_(N),
    Ni_(Ni),
    Nj_(Nj),
    lon_(west_),
    inc_(90, static_cast<long long>(N_)),
    i_(0),
    j_(0),
    k_(0),
    count_(0) {

    // latitudes_ covers the whole globe, but (Ni_,Nj_) cover only the domain
    ASSERT(latitudes_.size() == N * 2);
    ASSERT(2 <= Ni_ && Ni_ <= N * 4);
    ASSERT(2 <= Nj_ && Nj_ <= N * 2);

    // Position to first latitude
    while (k_ < latitudes_.size() && dom.north() < latitudes_[k_]) {
        k_++;
    }

}


Regular::RegularIterator::~RegularIterator() {
    ASSERT(count_ == Ni_ * Nj_);
}


void Regular::RegularIterator::print(std::ostream& out) const {
    out << "RegularIterator["
        <<  "west="  << west_
         << ",N="     << N_
         << ",Ni="    << Ni_
         << ",Nj="    << Nj_
         << ",i="     << i_
         << ",j="     << j_
         << ",k="     << k_
         << ",count=" << count_
         << "]";
}


bool Regular::RegularIterator::next(Latitude& lat, Longitude& lon) {
    while (j_ < Nj_ && i_ < Ni_) {

        ASSERT(j_ + k_ < latitudes_.size());
        lat = latitudes_[j_ + k_];
        lon = lon_;

        i_++;
        lon_ += inc_;

        if (i_ == Ni_) {
            j_++;
            i_ = 0;
            lon_ = west_;
        }
        count_++;
        return true;
    }
    return false;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir

