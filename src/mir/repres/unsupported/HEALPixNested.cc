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


#include "mir/repres/unsupported/HEALPixNested.h"

#include <ostream>

#include "eckit/log/JSON.h"

#include "mir/iterator/UnstructuredIterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"


namespace mir::repres::unsupported {


std::string HEALPixNested::name() const {
    return "H" + std::to_string(ring_.Nside()) + "_nested";
}


void HEALPixNested::makeName(std::ostream& out) const {
    out << name();
}


void HEALPixNested::fillGrib(grib_info& info) const {
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;
    info.grid.N         = static_cast<long>(ring_.Nside());

    info.grid.longitudeOfFirstGridPointInDegrees = 45.;  // Not sure what this should be

    info.extra_set("orderingConvention", "nested");
}


void HEALPixNested::json(eckit::JSON& j) const {
    j << "grid" << name();
}


void HEALPixNested::print(std::ostream& out) const {
    out << "HEALPixNested[name=" << name() << "]";
}


std::vector<util::GridBox> HEALPixNested::gridBoxes() const {
    const proxy::HEALPix::Reorder reorder(static_cast<int>(ring_.Nside()));
    const auto N = numberOfPoints();

    std::vector<util::GridBox> boxes(N);

    int i = 0;
    for (const auto& box : ring().gridBoxes()) {
        auto j      = reorder.ring_to_nest(i++);
        boxes.at(j) = box;
    }
    ASSERT(i == N);

    return boxes;
}


::atlas::Grid HEALPixNested::atlasGrid() const {
    // NOTE: delete class altogether once we can build HEALPix nested-ordering atlas::Grid
    NOTIMP;
}


Iterator* HEALPixNested::iterator() const {
    if (longitudes_.empty()) {
        const proxy::HEALPix::Reorder reorder(static_cast<int>(ring_.Nside()));
        const auto N = numberOfPoints();

        longitudes_.resize(N);
        latitudes_.resize(N);

        int i = 0;
        for (const auto& point : ring().atlasGrid().lonlat()) {
            auto j            = reorder.ring_to_nest(i++);
            longitudes_.at(j) = point.lon();
            latitudes_.at(j)  = point.lat();
        }
        ASSERT(i == N);
    }

    return new iterator::UnstructuredIterator(latitudes_, longitudes_);
}


}  // namespace mir::repres::unsupported
