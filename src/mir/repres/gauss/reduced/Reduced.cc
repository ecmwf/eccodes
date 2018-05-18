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


#include "mir/repres/gauss/reduced/Reduced.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <sstream>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Plural.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/types/Fraction.h"
#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Reduced::Reduced(const param::MIRParametrisation& parametrisation) :
    Gaussian(parametrisation),
    k_(0),
    Nj_(0) {

    std::vector<long> pl;
    ASSERT(parametrisation.get("pl", pl));
    pls(pl);

    setNj();
}


Reduced::Reduced(size_t N, const std::vector<long>& pl, const util::BoundingBox& bbox) :
    Gaussian(N, bbox),
    k_(0),
    Nj_(0),
    pl_(pl) {
    setNj();
}


Reduced::Reduced(size_t N, const util::BoundingBox& bbox) :
    Gaussian(N, bbox),
    k_(0),
    Nj_(0) {
}


Reduced::~Reduced() {
}


void Reduced::correctWestEast(Longitude& w, Longitude& e, bool grib1) const {
    using eckit::Fraction;
    ASSERT(w <= e);

    Fraction inc = getSmallestIncrement();
    ASSERT(inc > 0);

    const Longitude we = e - w;
    if (e != w && e.normalise(w) == w) {

        // if periodic West/East, adjust East only
        e = w + Longitude::GLOBE - inc;

    } else if (grib1 ? same_with_grib1_accuracy(we + inc, Longitude::GLOBE) || we + inc > Longitude::GLOBE
                     : we + inc >= Longitude::GLOBE) {

        // if periodic West/East, adjust East only
        e = w + Longitude::GLOBE - inc;

    } else {

        const std::vector<long>& pl = pls();
        const std::vector<double>& lats = latitudes();

        Fraction west = w.fraction();
        Fraction east = e.fraction();

        bool first = true;
        std::set<long> NiTried;

        for (size_t j = 0; j < Nj_; ++j) {
            Latitude ll(lats[k_ + j]);

            // crop longitude-wise, track distinct attempts
            const long Ni(pl[k_ + j]);
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

                ASSERT(Nw <= Ne);
                west = Nw * inc;
                east = Ne * inc;

                if (w > double(west) || first) {
                    w = west;
                }
                if (e < double(east) || first) {
                    e = east;
                }
                first = false;
            }
        }

        ASSERT(!first);
    }
}


bool Reduced::sameAs(const Representation& other) const {
    const Reduced* o = dynamic_cast<const Reduced*>(&other);
    return o && Gaussian::sameAs(other);
}


eckit::Fraction Reduced::getSmallestIncrement() const {
    ASSERT(N_);
    const std::vector<long>& pl = pls();
    const long maxpl = *std::max_element(pl.begin(), pl.end());
    ASSERT(maxpl);

    return Longitude::GLOBE.fraction() / maxpl;
}


Iterator* Reduced::unrotatedIterator() const {

    // Lambda captures a vector reference, ok because the representation
    // holding the vector lives longer than the iterator
    const std::vector<long>& pl = pls();
    auto Ni = [&pl](size_t i) {
        ASSERT(i < pl.size());
        return pl[i];
    };

    return Gaussian::unrotatedIterator(Ni);
}


Iterator* Reduced::rotatedIterator(const util::Rotation& rotation) const {

    // Lambda captures a vector reference, ok because the representation
    // holding the vector lives longer than the iterator
    const std::vector<long>& pl = pls();
    auto Ni = [&pl](size_t i) {
        ASSERT(i < pl.size());
        return pl[i];
    };

    return Gaussian::rotatedIterator(Ni, rotation);
}


const std::vector<long>& Reduced::pls() const {
    ASSERT(pl_.size() == N_ * 2);
    ASSERT(pl_.size() >= k_ + Nj_);

    return pl_;
}


void Reduced::pls(std::vector<long>& pl) {
    ASSERT(*std::min_element(pl.begin() + k_, pl.begin() + k_ + Nj_) >= 2);

    pl_.swap(pl);
    pls();
}


void Reduced::setNj() {
    ASSERT(N_ > 0);

    const std::vector<long>& pl = pls();
    const std::vector<double>& lats = latitudes();


    // position to first latitude and first/last longitude
    // NOTE: latitudes_ span the globe, sorted from North-to-South, k_ positions the North
    // NOTE: pl is global
    k_ = 0;
    Nj_  = N_ * 2;

    if (!includesNorthPole() || !includesSouthPole()) {
        Nj_ = 0;
        for (auto& lat : lats) {
            Latitude ll(lat);
            if (bbox_.north() < ll) {
                ++k_;
            } else if (bbox_.south() <= ll) {
                ++Nj_;
            } else {
                break;
            }
        }
        ASSERT(Nj_ > 1);
        ASSERT(Nj_ + k_ <= pl.size());
    }
}


void Reduced::fill(grib_info& info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    const std::vector<long>& pl = pls();

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj = long(Nj_);
    info.grid.N = long(N_);

    info.grid.pl = &pl[k_];
    info.grid.pl_size = long(Nj_);

    ASSERT(k_ + Nj_ <= pl.size());
    for (size_t i = k_; i < k_ + Nj_; i++) {
        ASSERT(pl[i] > 0);
    }

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


void Reduced::fill(api::MIRJob& job) const  {
    ASSERT(isGlobal());
    job.set("pl", pls());
}


size_t Reduced::frame(std::vector<double>& values, size_t size, double missingValue) const {

    validate(values);

    size_t count = 0;

    // TODO: Check if that logic cannot also be used for other grid, and therefor move it to a higher class

    std::map<size_t, size_t> shape;

    Latitude prev_lat = std::numeric_limits<double>::max();
    Longitude prev_lon = -std::numeric_limits<double>::max();

    size_t rows = 0;
    size_t dummy = 0; // Used to keep static analyser quiet
    size_t *col = &dummy;

    // Collect the 'shape' of the gaussian field
    // This could be done with the latitudes() and pls(), maybe more efficeintly
    // but this code could also be used for all grids
    // and even be cached (md5 of iterators)

    // Iterator is 'unrotated'
    eckit::ScopedPtr<Iterator> it(iterator());
    while (it->next()) {
        const Iterator::point_ll_t& p = it->pointUnrotated();

        if (p.lat != prev_lat ) {
            ASSERT(p.lat < prev_lat); // Assumes scanning mode
            prev_lat = p.lat;
            prev_lon = -std::numeric_limits<double>::max();

            col = &shape[rows++];
            (*col) = 0;
        }

        ASSERT(p.lon > prev_lon); // Assumes scanning mode
        prev_lon = p.lon;
        (*col) ++;
    }

    size_t k = 0;
    for (size_t j = 0; j < rows; j++) {
        size_t cols = shape[j];
        for (size_t i = 0; i < cols; i++) {
            if ( !((i < size) || (j < size) || (i >= cols - size) || (j >= rows - size))) {
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;
}


size_t Reduced::numberOfPoints() const {
    size_t total = 0;

    if (isGlobal()) {
        const std::vector<long>& pl = pls();
        total = size_t(std::accumulate(pl.begin(), pl.end(), 0));
    } else {
        eckit::ScopedPtr<repres::Iterator> iter(iterator());
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

    const std::vector<long>& pl = pls();
    const std::vector<double>& lats = latitudes();

    d = 0.;
    for (size_t j = k_ + 1; j < Nj_; ++j) {

        Latitude l1(lats[j - 1]);
        Latitude l2(lats[j]);

        const eckit::Fraction we = Longitude::GLOBE.fraction() / (std::min(pl[j - 1], pl[j]));
        const Latitude&
                latAwayFromEquator(std::abs(l1.value()) > std::abs(l2.value())? l1 : l2),
                latCloserToEquator(std::abs(l1.value()) > std::abs(l2.value())? l2 : l1);

        d = std::max(d, atlas::util::Earth::distance(
                         atlas::PointLonLat(0., latCloserToEquator.value()),
                         atlas::PointLonLat(we, latAwayFromEquator.value()) ));
    }

    ASSERT(d > 0.);
    return true;
}


util::BoundingBox Reduced::extendedBoundingBox(const util::BoundingBox& bbox) const {





    return bbox;
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

