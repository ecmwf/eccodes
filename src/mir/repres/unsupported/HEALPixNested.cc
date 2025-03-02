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

#include "mir/api/MIRJob.h"
#include "mir/iterator/UnstructuredIterator.h"
#include "mir/reorder/HEALPix.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"
#include "mir/util/Log.h"


namespace mir::repres::unsupported {


void HEALPixNested::makeName(std::ostream& out) const {
    out << "H" << std::to_string(ring_.Nside()) << "_nested";
}


void HEALPixNested::fillGrib(grib_info& info) const {
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;
    info.grid.N         = static_cast<long>(ring_.Nside());

    info.grid.longitudeOfFirstGridPointInDegrees = 45.;  // Not sure what this should be

    info.extra_set("orderingConvention", "nested");
}

void HEALPixNested::fillJob(api::MIRJob& job) const {
    job.set("grid", ring_.uniqueName() + "_nested");
}


void HEALPixNested::json(eckit::JSON& j) const {
    j.startObject();
    j << "grid" << ring_.uniqueName();
    j << "type"
      << "healpix";
    j << "ordering"
      << "nested";
    j.endObject();
}


void HEALPixNested::print(std::ostream& out) const {
    out << "HEALPixNested[name=" << ring_.uniqueName() << "]";
}


std::vector<util::GridBox> HEALPixNested::gridBoxes() const {
    const auto N       = numberOfPoints();
    const auto reorder = reorder::HEALPixRingToNested().reorder(N);
    ASSERT(reorder.size() == N);

    std::vector<util::GridBox> boxes(N);

    int i = 0;
    for (const auto& box : ring().gridBoxes()) {
        auto j      = reorder.at(i++);
        boxes.at(j) = box;
    }
    ASSERT(i == N);

    return boxes;
}


::atlas::Grid HEALPixNested::atlasGrid() const {
    Log::warning() << "HEALPixNested::atlasGrid() unsupported, returning {type=healpix, ordering=ring}" << std::endl;
    return ring_.atlasGrid();
}


Iterator* HEALPixNested::iterator() const {
    if (longitudes_.empty()) {
        const auto N       = numberOfPoints();
        const auto reorder = reorder::HEALPixRingToNested().reorder(N);
        ASSERT(reorder.size() == N);

        longitudes_.resize(N);
        latitudes_.resize(N);

        int i = 0;
        for (const auto& point : ring().atlasGrid().lonlat()) {
            auto j            = reorder.at(i++);
            longitudes_.at(j) = point.lon();
            latitudes_.at(j)  = point.lat();
        }
        ASSERT(i == N);
    }

    return new iterator::UnstructuredIterator(latitudes_, longitudes_);
}


}  // namespace mir::repres::unsupported
