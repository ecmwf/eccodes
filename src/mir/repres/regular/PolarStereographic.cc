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
    ASSERT(param.get("proj", proj_) && !proj_.empty());

    param.get("LaDInDegrees", LaDInDegrees_);
    param.get("orientationOfTheGridInDegrees", orientationOfTheGridInDegrees_);

    long south = 0;
    param.get("southPoleOnProjectionPlane", south);
    southPoleOnProjectionPlane_ = south != 0;
}


void PolarStereographic::fillGrib(grib_info& info) const {
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;
    info.extra_set("edition", 2L);  // write LaDInDegrees

    Point2 first   = {x().front(), y().front()};
    Point2 firstLL = grid().projection().lonlat(first);

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = util::normalise_longitude(firstLL[LLCOORDS::LON], 0);

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.iScansNegatively = x().front() < x().back() ? 0L : 1L;
    info.grid.jScansPositively = y().front() < y().back() ? 1L : 0L;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("LaDInDegrees", LaDInDegrees_);
    info.extra_set("orientationOfTheGridInDegrees", util::normalise_longitude(orientationOfTheGridInDegrees_, 0));

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
         << "Ni=" << x()->size() << ";"
         << "Nj=" << y()->size() << ";"
         << "grid=" << std::abs(x().step()) << "/" << std::abs(y().step()) << ";"
         << "latitudeOfFirstGridPointInDegrees=" << firstLL[LLCOORDS::LAT] << ";"
         << "longitudeOfFirstGridPointInDegrees=" << firstLL[LLCOORDS::LON] << ";"
         << "iScansNegatively=" << (x().front() < x().back() ? 0 : 1) << ";"
         << "jScansPositively=" << (y().front() < y().back() ? 1 : 0);
    //  << "shapeOfTheEarth="  <<  << ";"
    //  << "radius="           <<  << ";"
    //  << "earthMajorAxis="   <<  << ";"
    //  << "earthMinorAxis="   <<  << ";"

    job.set("grid", grid.str());
}


}  // namespace mir::repres::regular
