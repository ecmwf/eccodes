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


Reduced::Reduced(const param::MIRParametrisation& parametrisation):
    Gaussian(parametrisation) {
}


Reduced::Reduced(size_t N, const util::BoundingBox& bbox):
    Gaussian(N, bbox) {
}


Reduced::~Reduced() {
}


void Reduced::cropToBoundingBox(size_t N, const std::vector<double>& latitudes, util::BoundingBox& bbox, std::vector<long>& pl) {
    ASSERT(N > 0);
    ASSERT(pl.size() == N * 2);
    ASSERT(pl.size() == latitudes.size());


    // adjust bounding box North/South and clip pl
    std::vector<long> newpl;
    newpl.reserve(pl.size());

    Latitude n = bbox.north();
    Latitude s = bbox.south();

    double latMin = Latitude::NORTH_POLE.value();
    double latMax = Latitude::SOUTH_POLE.value();

    for (size_t i = 0; i < latitudes.size(); i++) {
        Latitude ll(latitudes[i]);
        if ((ll >= s) && (ll <= n)) {
            newpl.push_back(pl[i]);
            if (latMin > latitudes[i]) {
                latMin = latitudes[i];
            }
            if (latMax < latitudes[i]) {
                latMax = latitudes[i];
            }
        }
    }

    ASSERT(latMin <= latMax);
    n = latMax;
    s = latMin;


    // adjust bounding box West/East (actually, only East if periodic)
    Longitude w = bbox.west();
    Longitude e = bbox.east();


    const long maxpl = *std::max_element(pl.begin(), pl.end());
    ASSERT(maxpl);

    eckit::Fraction inc = Longitude::GLOBE.fraction() / maxpl;
    ASSERT(inc > 0);

    if (e > w + Longitude::GLOBE - inc) {
        e = w + Longitude::GLOBE - inc;
    }

    // ensure 0 <= East - West < 360
    bool same(e == w);
    if (!same) {
        e = e.normalise(w);
        if (e == w) {
            e = w + Longitude::GLOBE - inc;
        }
    }


    // set bounding box and inform
    util::BoundingBox newbbox = util::BoundingBox(n, w, s, e);

    if (newbbox != bbox) {
        eckit::Channel& log = eckit::Log::debug<LibMir>();
        std::streamsize old = log.precision(12);
        log << "Reduced::cropToBoundingBox: "
            << "\n   " << bbox
            << "\n > " << newbbox
            << "\n   #pl=" << pl.size()
            << "\n > #pl=" << newpl.size()
            << std::endl;
        log.precision(old);

        bbox = newbbox;
        pl.swap(newpl);
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


void Reduced::fill(grib_info& info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    const std::vector<long> &pl = pls();

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj = pl.size();
    info.grid.N = N_;

    info.grid.pl = &pl[0];
    info.grid.pl_size = pl.size();


    for (size_t i = 0; i < size_t(info.grid.pl_size); i++) {
        ASSERT(info.grid.pl[i] > 0);
    }

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    // for GRIB, a global field is also aligned with Greenwich
    bool westAtGreenwich = bbox_.west() == Longitude::GREENWICH;;

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


const Reduced *Reduced::croppedRepresentation(const util::BoundingBox& bbox) const  {
    return croppedRepresentation(bbox, pls());
}


util::BoundingBox Reduced::croppedBoundingBox(const util::BoundingBox& bbox) const {
    const std::vector<long>& pl = pls();
    const std::vector<double>& lats = latitudes();
    ASSERT(lats.size() == pl.size());

    Latitude n;
    Latitude s;
    eckit::Fraction e;
    eckit::Fraction w;

    const eckit::Fraction west = bbox.west().fraction();
    const eckit::Fraction east = bbox.east().fraction();

    bool first = true;
    std::set<long> NiTried;

    for (size_t i = 0; i < lats.size(); i++) {
        Latitude ll(lats[i]);
        if ((ll >= bbox.south()) && (ll <= bbox.north())) {

            // crop latitude-wise, ensuring exact Gaussian latitudes
            n = std::max(Latitude(lats[i]), n);
            s = std::min(Latitude(lats[i]), s);

            // crop longitude-wise, track distinct attempts
            const long Ni(pl[i]);
            ASSERT(Ni >= 2);
            if (NiTried.insert(Ni).second) {

                eckit::Fraction inc = Longitude::GLOBE.fraction() / Ni;

                eckit::Fraction::value_type Nw = (west / inc).integralPart();
                if (Nw * inc < west) {
                    Nw += 1;
                }

                eckit::Fraction::value_type Ne = (east / inc).integralPart();
                if (Ne * inc > east || Nw + Ne == Ni) {
                    Ne -= 1;
                }

                ASSERT(Nw <= Ne);
                if (w > Nw * inc || first) {
                    w = Nw * inc;
                }
                if (e < Ne * inc || first) {
                    e = Ne * inc;
                }
                first = false;
            }
        }
    }

    ASSERT(!first);


    // set bounding box and inform
    util::BoundingBox cropped(n, w, s, e);

    if (cropped != bbox) {
        eckit::Channel& log = eckit::Log::debug<LibMir>();
        std::streamsize old = log.precision(12);
        log << "Reduced::croppedBoundingBox: "
            << "\n   " << bbox
            << "\n > " << cropped
            << std::endl;
        log.precision(old);
    }

    return cropped;
}


const Reduced *Reduced::croppedRepresentation(const util::BoundingBox&, const std::vector<long>&) const {
    std::ostringstream os;
    os << "Reduced::croppedRepresentation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
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

    const std::vector<double>& lats = latitudes();
    const std::vector<long>& pl = pls();
    ASSERT(pl.size() == lats.size());
    ASSERT(pl.size() == N_ * 2);

    d = 0.;
    Latitude l1(Latitude::NORTH_POLE);
    Latitude l2(lats[0]);

    for (size_t j = 1; j < lats.size(); ++j) {

        const eckit::Fraction we = Longitude::GLOBE.fraction() / (std::min(pl[j - 1], pl[j]));
        const Latitude&
                latAwayFromEquator(std::abs(l1.value()) > std::abs(l2.value())? l1 : l2),
                latCloserToEquator(std::abs(l1.value()) > std::abs(l2.value())? l2 : l1);

        d = std::max(d, atlas::util::Earth::distance(
                         atlas::PointLonLat(0., latCloserToEquator.value()),
                         atlas::PointLonLat(we, latAwayFromEquator.value()) ));

        l1 = l2;
        l2 = lats[j];
    }

    ASSERT(d > 0.);
    return true;
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

