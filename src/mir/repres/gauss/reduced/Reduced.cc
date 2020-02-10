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


#include "mir/repres/gauss/reduced/Reduced.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <numeric>
#include <set>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Fraction.h"

#include "mir/api/MIREstimation.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Reduced::Reduced(const param::MIRParametrisation& parametrisation) : Gaussian(parametrisation), k_(0), Nj_(N_ * 2) {

    // adjust latitudes, longitudes and re-set bounding box
    Latitude n = bbox_.north();
    Latitude s = bbox_.south();
    correctSouthNorth(s, n);

    std::vector<long> pl;
    ASSERT(parametrisation.get("pl", pl));

    // if pl isn't global (from file!) insert leading/trailing 0's
    const auto& lats = latitudes();
    if (n < lats.front() || s > lats.back()) {

        size_t k  = 0;
        size_t nj = 0;
        for (Latitude lat : lats) {
            if (n < lat) {
                ++k;
            }
            else if (s <= lat) {
                ASSERT(pl[nj] >= 2);
                ++nj;
            }
            else {
                break;
            }
        }
        ASSERT(k + nj <= N_ * 2);

        if (k > 0) {
            pl.reserve(N_ * 2);
            pl.insert(pl.begin(), k, 0);
        }
        pl.resize(N_ * 2, 0);
    }

    setNj(pl, s, n);

    Longitude w = bbox_.west();
    Longitude e = bbox_.east();
    correctWestEast(w, e);

    bbox_ = util::BoundingBox(n, w, s, e);
}


Reduced::Reduced(size_t N, const std::vector<long>& pl, const util::BoundingBox& bbox, double angularPrecision) :
    Gaussian(N, bbox, angularPrecision),
    k_(0),
    Nj_(N_ * 2) {
    setNj(pl, bbox.south(), bbox.north());
}


Reduced::Reduced(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Gaussian(N, bbox, angularPrecision),
    k_(0),
    Nj_(N * 2) {
    // derived classes must set k_, Nj_ using this constructor
}


Reduced::~Reduced() = default;


void Reduced::correctWestEast(Longitude& w, Longitude& e) const {
    using eckit::Fraction;
    ASSERT(w <= e);

    const Fraction smallestIncrement = getSmallestIncrement();
    ASSERT(smallestIncrement > 0);

    if (angleApproximatelyEqual(Longitude::GREENWICH, w) &&
        (angleApproximatelyEqual(Longitude::GLOBE - smallestIncrement, e - w) ||
         Longitude::GLOBE - smallestIncrement < e - w || (e != w && e.normalise(w) == w))) {

        w = Longitude::GREENWICH;
        e = Longitude::GLOBE - smallestIncrement;
    }
    else {

        const Fraction west = w.fraction();
        const Fraction east = e.fraction();
        Fraction W          = west;
        Fraction E          = east;

        bool first = true;
        std::set<long> NiTried;

        const std::vector<long>& pl = pls();
        for (size_t j = k_; j < k_ + Nj_; ++j) {

            // crop longitude-wise, track distinct attempts
            const long Ni(pl[j]);
            ASSERT(Ni >= 2);
            if (NiTried.insert(Ni).second) {

                Fraction inc = Longitude::GLOBE.fraction() / Ni;

                Fraction::value_type Nw = (west / inc).integralPart();
                if (Nw * inc < west) {
                    Nw += 1;
                }

                Fraction::value_type Ne = (east / inc).integralPart();
                if (Ne * inc > east || Nw + Ne == Ni) {
                    Ne -= 1;
                }

                if (Nw <= Ne) {
                    ASSERT(w <= Longitude(Nw * inc));
                    ASSERT(Longitude(Ne * inc) <= e);

                    if (W > double(Nw * inc) || first) {
                        W = Nw * inc;
                    }
                    if (E < double(Ne * inc) || first) {
                        E = Ne * inc;
                    }
                    first = false;
                }
            }
        }

        ASSERT(!first);
        ASSERT(W <= E);
        w = W;
        e = E;
    }
}


bool Reduced::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const Reduced*>(&other);
    return o && Gaussian::sameAs(other);
}


eckit::Fraction Reduced::getSmallestIncrement() const {
    ASSERT(N_);
    using distance_t = std::make_signed<size_t>::type;

    const std::vector<long>& pl = pls();
    const long maxpl            = *std::max_element(pl.begin() + distance_t(k_), pl.begin() + distance_t(k_ + Nj_));
    ASSERT(maxpl);

    return Longitude::GLOBE.fraction() / maxpl;
}


Iterator* Reduced::unrotatedIterator() const {

    // Lambda captures a vector reference, ok because the representation
    // holding the vector lives longer than the iterator
    const std::vector<long>& pl = pls();
    auto Ni                     = [&pl](size_t i) {
        ASSERT(i < pl.size());
        return pl[i];
    };

    return Gaussian::unrotatedIterator(Ni);
}


Iterator* Reduced::rotatedIterator(const util::Rotation& rotation) const {

    // Lambda captures a vector reference, ok because the representation
    // holding the vector lives longer than the iterator
    const std::vector<long>& pl = pls();
    auto Ni                     = [&pl](size_t i) {
        ASSERT(i < pl.size());
        return pl[i];
    };

    return Gaussian::rotatedIterator(Ni, rotation);
}


const std::vector<long>& Reduced::pls() const {
    ASSERT(pl_.size() == N_ * 2);
    ASSERT(pl_.size() >= k_ + Nj_);
    ASSERT(Nj_ > 0);

    return pl_;
}

template <typename PlVector>
void Reduced::setNj(const PlVector& pl, const Latitude& s, const Latitude& n) {
    ASSERT(N_ > 0);
    ASSERT(N_ * 2 == pl.size());


    // position to first latitude and first/last longitude
    // NOTE: latitudes() spans the globe, sorted from North-to-South, k_ positions the North
    // NOTE: pl spans the globe
    k_  = 0;
    Nj_ = N_ * 2;

    const auto& lats = latitudes();
    if (n < lats.front() || s > lats.back()) {
        Nj_ = 0;
        for (auto& lat : lats) {
            Latitude ll(lat);
            if (n < ll) {
                ++k_;
            }
            else if (s <= ll) {
                ASSERT(pl[k_ + Nj_] >= 2);
                ++Nj_;
            }
            else {
                break;
            }
        }
    }

    pl_ = std::vector<long>(pl.begin(), pl.end());
    pls();  // check internal assumptions
}

// Explicit template instantiations of above implementation for different PlVector types

// PlVector = std::vector<int>
template void Reduced::setNj(const std::vector<int>& pl, const Latitude& s, const Latitude& n);

// PlVector = std::vector<long>
template void Reduced::setNj(const std::vector<long>& pl, const Latitude& s, const Latitude& n);


void Reduced::fill(grib_info& info) const {

    // See copy_spec_from_ksec.c in libemos for info

    const std::vector<long>& pl = pls();

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj        = long(Nj_);
    info.grid.N         = long(N_);

    ASSERT(k_ + Nj_ <= pl.size());
    info.grid.pl      = &pl[k_];
    info.grid.pl_size = long(Nj_);

    for (size_t i = k_; i < k_ + Nj_; i++) {
        ASSERT(pl[i] > 0);
    }

    bbox_.fill(info);
}


void Reduced::estimate(api::MIREstimation& estimation) const {
    Gaussian::estimate(estimation);
    const std::vector<long>& pl = pls();
    estimation.pl(pl.size());
}


std::vector<util::GridBox> Reduced::gridBoxes() const {
    ASSERT(1 < Nj_);


    // latitude edges
    std::vector<double> latEdges = calculateUnrotatedGridBoxLatitudeEdges();


    // grid boxes
    std::vector<util::GridBox> r;
    r.reserve(numberOfPoints());

    bool periodic = isPeriodicWestEast();
    auto& pl      = pls();
    ASSERT(k_ + Nj_ <= pl.size());

    for (size_t j = k_; j < k_ + Nj_; ++j) {
        ASSERT(pl[j] > 0);
        eckit::Fraction inc(360, pl[j]);

        auto Ni = size_t(pl[j]);

        // longitude edges
        auto west = bbox_.west().fraction();
        auto Nw   = (west / inc).integralPart();
        if (Nw * inc < west) {
            Nw += 1;
        }
        Longitude lon0 = (Nw * inc) - (inc / 2);
        Longitude lon1 = lon0;

        for (size_t i = 0; i < Ni; ++i) {
            auto l = lon1;
            lon1 += inc;
            r.emplace_back(util::GridBox(latEdges[j], l.value(), latEdges[j + 1], lon1.value()));
        }

        ASSERT(periodic ? lon0 == lon1.normalise(lon0) : lon0 < lon1.normalise(lon0));
    }

    ASSERT(r.size() == numberOfPoints());
    return r;
}


void Reduced::fill(api::MIRJob& job) const {
    ASSERT(isGlobal());
    job.set("pl", pls());
}


size_t Reduced::frame(MIRValuesVector& values, size_t size, double missingValue, bool estimate) const {

    if (!estimate) {
        validate(values);
    }

    size_t count = 0;

    // TODO: Check if that logic cannot also be used for other grid, and therefor move it to a higher class

    std::map<size_t, size_t> shape;

    Latitude prev_lat  = std::numeric_limits<double>::max();
    Longitude prev_lon = -std::numeric_limits<double>::max();

    size_t rows  = 0;
    size_t dummy = 0;  // Used to keep static analyser quiet
    size_t* col  = &dummy;

    // Collect the 'shape' of the gaussian field
    // This could be done with the latitudes() and pls(), maybe more efficeintly
    // but this code could also be used for all grids
    // and even be cached (md5 of iterators)

    // Iterator is 'unrotated'
    std::unique_ptr<Iterator> it(iterator());
    while (it->next()) {
        const auto& p = it->pointUnrotated();

        if (p.lat() != prev_lat) {
            ASSERT(p.lat() < prev_lat);  // Assumes scanning mode
            prev_lat = p.lat();
            prev_lon = -std::numeric_limits<double>::max();

            col    = &shape[rows++];
            (*col) = 0;
        }

        ASSERT(p.lon() > prev_lon);  // Assumes scanning mode
        prev_lon = p.lon();
        (*col)++;
    }

    size_t k = 0;
    for (size_t j = 0; j < rows; j++) {
        size_t cols = shape[j];
        for (size_t i = 0; i < cols; i++) {
            if (!((i < size) || (j < size) || (i >= cols - size) || (j >= rows - size))) {
                if (!estimate) {
                    values[k] = missingValue;
                }
                count++;
            }
            k++;
        }
    }

    if (!estimate) {
        ASSERT(k == values.size());
    }

    return count;
}


size_t Reduced::numberOfPoints() const {
    size_t total = 0;

    if (isGlobal()) {
        const std::vector<long>& pl = pls();
        total                       = size_t(std::accumulate(pl.begin(), pl.end(), 0));
    }
    else {
        std::unique_ptr<repres::Iterator> iter(iterator());
        while (iter->next()) {
            total++;
        }
    }
    return total;
}


bool Reduced::getLongestElementDiagonal(double& d) const {

    // Look for a majorant of all element diagonals, using the difference of
    // latitudes closest/furthest from equator and longitude furthest from
    // Greenwich

    const std::vector<long>& pl     = pls();
    const std::vector<double>& lats = latitudes();

    d = 0.;
    for (size_t j = k_ + 1; j < k_ + Nj_; ++j) {

        Latitude l1(lats[j - 1]);
        Latitude l2(lats[j]);

        const eckit::Fraction we = Longitude::GLOBE.fraction() / (std::min(pl[j - 1], pl[j]));
        auto& latAwayFromEquator(std::abs(l1.value()) > std::abs(l2.value()) ? l1 : l2);
        auto& latCloserToEquator(std::abs(l1.value()) > std::abs(l2.value()) ? l2 : l1);

        d = std::max(d, atlas::util::Earth::distance(atlas::PointLonLat(0., latCloserToEquator.value()),
                                                     atlas::PointLonLat(we, latAwayFromEquator.value())));
    }

    ASSERT(d > 0.);
    return true;
}


util::BoundingBox Reduced::extendBoundingBox(const util::BoundingBox& bbox) const {

    // adjust West/East to include bbox's West/East
    Longitude w = bbox.west();
    Longitude e = bbox.east();
    {
        const auto west = bbox.west().fraction();
        const auto east = bbox.east().fraction();

        bool first = true;
        std::set<long> NiTried;

        const std::vector<long>& pl = pls();
        for (size_t j = k_; j < k_ + Nj_; ++j) {

            // extend longitude-wise, track distinct attempts
            const long Ni(pl[j]);
            if (NiTried.insert(Ni).second) {

                ASSERT(Ni >= 2);
                auto inc = Longitude::GLOBE.fraction() / Ni;

                auto Nw = (west / inc).integralPart() - 1;
                auto Ne = (east / inc).integralPart() + 1;

                if (w > Nw * inc || first) {
                    w = Nw * inc;
                }
                if (e < Ne * inc || first) {
                    e = Ne * inc;
                }
                first = false;
            }
        }

        ASSERT(!first);

        long NiMax = *std::max_element(NiTried.begin(), NiTried.end());
        ASSERT(NiMax > 0);
        auto inc = Longitude::GLOBE.fraction() / NiMax;

        if (e - w + inc >= Longitude::GLOBE) {
            w = 0;
            e = Longitude::GLOBE;
        }

        ASSERT(w < e);
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


bool Reduced::isPeriodicWestEast() const {
    eckit::Fraction inc = getSmallestIncrement();
    return bbox_.east() - bbox_.west() + inc >= Longitude::GLOBE;
}

std::string Reduced::factory() const {
    return "reduced_gg";
}

}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir
