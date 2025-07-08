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


#include "mir/repres/gauss/regular/Regular.h"

#include "eckit/log/JSON.h"
#include "eckit/types/FloatCompare.h"

#include "mir/api/mir_config.h"
#include "mir/api/MIRJob.h"
#include "mir/api/mir_config.h"
#include "mir/util/Atlas.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"


namespace mir::repres::gauss::regular {

Regular::Regular(const param::MIRParametrisation& parametrisation) : Gaussian(parametrisation), k_(0), Ni_(0), Nj_(0) {
    // adjust latitudes, longitudes and re-set bounding box
    Latitude n = bbox_.north();
    Latitude s = bbox_.south();
    correctSouthNorth(s, n);

    Longitude e = bbox_.east();
    Longitude w = bbox_.west();
    correctWestEast(w, e);

    auto old(bbox_);
    bbox_ = util::BoundingBox(n, w, s, e);

    Log::debug() << "Regular::Regular: BoundingBox:"
                 << "\n\t   " << old << "\n\t > " << bbox_ << std::endl;
    setNiNj();
}

Regular::Regular(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Gaussian(N, bbox, angularPrecision), k_(0), Ni_(0), Nj_(0) {

    // adjust latitudes, longitudes and re-set bounding box
    Latitude n = bbox.north();
    Latitude s = bbox.south();
    correctSouthNorth(s, n);

    Longitude w = bbox.west();
    Longitude e = bbox.east();
    correctWestEast(w, e);

    bbox_ = util::BoundingBox(n, w, s, e);

    setNiNj();
}

void Regular::fillGrib(grib_info& info) const {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_GG;

    info.grid.N                            = long(N_);
    info.grid.iDirectionIncrementInDegrees = getSmallestIncrement();
    info.grid.Ni                           = long(Ni_);
    info.grid.Nj                           = long(Nj_);

    bbox_.fillGrib(info);
}

void Regular::fillJob(api::MIRJob& job) const {
    Gaussian::fillJob(job);
    job.set("grid", "F" + std::to_string(N_));
}

void Regular::makeName(std::ostream& out) const {
    out << "F" << N_;
    bbox_.makeName(out);
}

void Regular::correctWestEast(Longitude& w, Longitude& e) const {
    using eckit::Fraction;
    ASSERT(w <= e);

    Fraction inc = getSmallestIncrement();
    ASSERT(inc > 0);

    if (angleApproximatelyEqual(Longitude::GREENWICH, w) &&
        (angleApproximatelyEqual(Longitude::GLOBE - inc, e - w) || Longitude::GLOBE - inc < e - w ||
         (e != w && e.normalise(w) == w))) {

        w = Longitude::GREENWICH;
        e = Longitude::GLOBE - inc;
    }
    else {

        const Fraction west = w.fraction();
        const Fraction east = e.fraction();

        Fraction::value_type Nw = (west / inc).integralPart();
        if (Nw * inc < west) {
            Nw += 1;
        }

        Fraction::value_type Ne = (east / inc).integralPart();
        if (Ne * inc > east) {
            Ne -= 1;
        }

        ASSERT(Nw <= Ne);
        w = Nw * inc;
        e = Ne * inc;
    }
}

bool Regular::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const Regular*>(&other);
    return (o != nullptr) && (N_ == o->N_) && (bbox_ == o->bbox_);
}

eckit::Fraction Regular::getSmallestIncrement() const {
    ASSERT(N_);
    return {90, eckit::Fraction::value_type(N_)};
}

size_t Regular::numberOfPoints() const {
    ASSERT(Ni_);
    ASSERT(Nj_);
    return Ni_ * Nj_;
}

bool Regular::getLongestElementDiagonal(double& d) const {
    constexpr double TWO = 2.;

    ASSERT(N_ > 0);
    const auto& lats = latitudes();

    auto snHalf = (lats[N_ - 1] - lats[N_]) / TWO;
    ASSERT(!eckit::types::is_approximately_equal(snHalf, 0.));

    auto weHalf = double(getSmallestIncrement()) / TWO;
    ASSERT(!eckit::types::is_approximately_equal(weHalf, 0.));

    d = TWO * util::Earth::distance({0., 0.}, {weHalf, snHalf});
    return true;
}


void Regular::json(eckit::JSON& s) const {
    s.startObject();
    s << "type"
      << "regular_gg";
    Gaussian::json(s);
    s.endObject();
}


util::BoundingBox Regular::extendBoundingBox(const util::BoundingBox& bbox) const {

    // adjust West/East to include bbox's West/East
    Longitude w = bbox.west();
    Longitude e = bbox.east();
    {
        auto inc = getSmallestIncrement();

        auto Nmax = (Longitude::GLOBE.fraction() / inc).integralPart();
        auto Nw   = (w.fraction() / inc).integralPart() - 1;
        auto Ne   = (e.fraction() / inc).integralPart() + 1;

        if (Ne - Nw < Nmax) {
            w = Nw * inc;
            e = Ne * inc;
        }
        else {
            w = 0;
            e = Longitude::GLOBE;
        }
    }

    // adjust South/North to include bbox's South/North ('outwards')
    Latitude s = bbox.south();
    Latitude n = bbox.north();
    correctSouthNorth(s, n, false);

    // set bounding box
    const util::BoundingBox extended(n, w, s, e);
    ASSERT(extended.contains(bbox));

    return extended;
}

bool Regular::isPeriodicWestEast() const {
    eckit::Fraction inc = getSmallestIncrement();
    return bbox_.east() - bbox_.west() + inc >= Longitude::GLOBE;
}

atlas::Grid Regular::atlasGrid() const {
#if mir_HAVE_ATLAS
    return atlas::RegularGaussianGrid("F" + std::to_string(N_), domain());
#else
    NOTIMP;
#endif
}

void Regular::setNiNj() {
    ASSERT(N_);

    const auto inc   = getSmallestIncrement();
    const auto& lats = latitudes();

    const Longitude& west = bbox_.west();
    const Longitude& east = bbox_.east();
    const Latitude& south = bbox_.south();
    const Latitude& north = bbox_.north();

    Ni_ = N_ * 4;

    if (east - west + inc < Longitude::GLOBE) {

        auto w  = west.fraction();
        auto Nw = (w / inc).integralPart();
        if (Nw * inc < w) {
            Nw += 1;
        }

        auto e  = east.fraction();
        auto Ne = (e / inc).integralPart();
        if (Ne * inc > e) {
            Ne -= 1;
        }

        ASSERT(Ne - Nw + 1 > 0);
        Ni_ = size_t(Ne - Nw + 1);

        ASSERT(2 <= Ni_ && Ni_ <= N_ * 4);
    }

    k_  = 0;
    Nj_ = N_ * 2;

    if (north < lats.front() || south > lats.back()) {
        Nj_ = 0;
        for (Latitude lat : lats) {
            if (north < lat && !angleApproximatelyEqual(north, lat)) {
                ++k_;
            }
            else if (south <= lat || angleApproximatelyEqual(south, lat)) {
                ++Nj_;
            }
            else {
                break;
            }
        }
        ASSERT(Nj_ > 0);
    }

    Log::debug() << "Regular::setNiNj: Ni*Nj = " << Ni_ << " * " << Nj_ << " = " << (Ni_ * Nj_) << std::endl;
}

size_t Regular::frame(MIRValuesVector& values, size_t size, double missingValue) const {

    // TODO: Check if that logic cannot also be used for other grid, and therefore move it to a higher class
    validate(values);

    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < Nj_; j++) {
        for (size_t i = 0; i < Ni_; i++) {
            if (!((i < size) || (j < size) || (i >= Ni_ - size) || (j >= Nj_ - size))) {  // Check me, may be buggy
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;
}


}  // namespace mir::repres::gauss::regular
