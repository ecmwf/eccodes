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


#include "mir/repres/regular/PolarStereographic.h"

#include <cmath>
#include <sstream>

#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Angles.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"


namespace mir::repres::regular {


static const RepresentationBuilder<PolarStereographic> __builder("polar_stereographic");


PolarStereographic::PolarStereographic(const param::MIRParametrisation& param) :
    RegularGrid(param, make_proj_spec(param)),
    LaDInDegrees_(0),
    orientationOfTheGridInDegrees_(0),
    southPoleOnProjectionPlane_(false) {
    long edition = 0;
    param.get("edition", edition);

    // GRIB1 cannot write LaD
    writeLaDInDegrees_ = edition == 2;
    param.get("writeLaDInDegrees", writeLaDInDegrees_);

    // GRIB2 cannot write negative longitude values
    writeLonPositive_ = edition == 2;
    param.get("writeLonPositive", writeLonPositive_);

    // Details
    ASSERT(param.get("proj", proj_) && !proj_.empty());

    param.get("LaDInDegrees", LaDInDegrees_);
    param.get("orientationOfTheGridInDegrees", orientationOfTheGridInDegrees_);

    long south = 0;
    param.get("southPoleOnProjectionPlane", south);
    southPoleOnProjectionPlane_ = south != 0;

    long uvRelativeToGrid = 0;
    uvRelativeToGrid_     = param.get("uvRelativeToGrid", uvRelativeToGrid) && uvRelativeToGrid != 0;
}


void PolarStereographic::fillGrib(grib_info& info) const {
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;

    Point2 first   = {x().front(), y().front()};
    Point2 firstLL = grid().projection().lonlat(first);

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL[LLCOORDS::LON], 0) : firstLL[LLCOORDS::LON];

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.uvRelativeToGrid = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("orientationOfTheGridInDegrees", util::normalise_longitude(orientationOfTheGridInDegrees_, 0));

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", LaDInDegrees_);
    }

    // some extra keys are edition-specific, so parent call is here
    RegularGrid::fillGrib(info);
}


void PolarStereographic::fillJob(api::MIRJob& job) const {
    Point2 first   = {x().front(), y().front()};
    Point2 firstLL = grid().projection().lonlat(first);

    std::ostringstream grid;
    grid << "gridType=polar_stereographic;"
         << "proj=" << proj_ << ";"
         << "LaDInDegrees=" << LaDInDegrees_ << ";"
         << "orientationOfTheGridInDegrees=" << orientationOfTheGridInDegrees_ << ";"
         << "southPoleOnProjectionPlane=" << (southPoleOnProjectionPlane_ ? "1" : "0") << ";"
         << "Ni=" << x().size() << ";"
         << "Nj=" << y().size() << ";"
         << "grid=" << std::abs(x().step()) << "/" << std::abs(y().step()) << ";"
         << "latitudeOfFirstGridPointInDegrees=" << firstLL[LLCOORDS::LAT] << ";"
         << "longitudeOfFirstGridPointInDegrees=" << firstLL[LLCOORDS::LON];

    job.set("grid", grid.str());

    // some extra keys are edition-specific, so parent call is here
    RegularGrid::fillJob(job);
}


}  // namespace mir::repres::regular
