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


#include "mir/repres/regular/Lambert.h"

#include <cmath>

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Angles.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"


namespace mir::repres::regular {


static const RepresentationBuilder<Lambert> __builder("lambert");


Lambert::Lambert(const param::MIRParametrisation& param) : RegularGrid(param, make_projection(param)) {
    long edition = 0;
    param.get("edition", edition);

    // GRIB1 cannot write LaD
    writeLaDInDegrees_ = edition == 2;
    param.get("writeLaDInDegrees", writeLaDInDegrees_);

    // GRIB2 cannot write negative longitude values
    writeLonPositive_ = edition == 2;
    param.get("writeLonPositive", writeLonPositive_);

    // Details
    param.get("latitudeOfSouthernPoleInDegrees", latitudeOfSouthernPoleInDegrees_ = -90.);
    param.get("longitudeOfSouthernPoleInDegrees", longitudeOfSouthernPoleInDegrees_ = 0.);

    long uvRelativeToGrid = 0;
    uvRelativeToGrid_     = param.get("uvRelativeToGrid", uvRelativeToGrid) && uvRelativeToGrid != 0;
}


RegularGrid::Projection Lambert::make_projection(const param::MIRParametrisation& param) {
    auto spec = make_proj_spec(param);
    if (!spec.empty()) {
        return spec;
    }

    double LaDInDegrees    = 0.;
    double LoVInDegrees    = 0.;
    double Latin1InDegrees = 0.;
    double Latin2InDegrees = 0.;
    ASSERT(param.get("LaDInDegrees", LaDInDegrees));
    ASSERT(param.get("LoVInDegrees", LoVInDegrees));
    param.get("Latin1InDegrees", Latin1InDegrees = LaDInDegrees);
    param.get("Latin2InDegrees", Latin2InDegrees = LaDInDegrees);

    return Projection::Spec("type", "lambert_conformal_conic")
        .set("latitude1", Latin1InDegrees)
        .set("latitude2", Latin2InDegrees)
        .set("latitude0", LaDInDegrees)
        .set("longitude0", LoVInDegrees);
}


void Lambert::fillGrib(grib_info& info) const {
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;

    Point2 first = {firstPointBottomLeft() ? x().min() : x().front(), firstPointBottomLeft() ? y().min() : y().front()};
    Point2 firstLL   = grid().projection().lonlat(first);
    Point2 reference = grid().projection().lonlat({0., 0.});

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL[LLCOORDS::LON], 0) : firstLL[LLCOORDS::LON];

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.latitudeOfSouthernPoleInDegrees  = latitudeOfSouthernPoleInDegrees_;
    info.grid.longitudeOfSouthernPoleInDegrees = longitudeOfSouthernPoleInDegrees_;
    info.grid.uvRelativeToGrid                 = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("Latin1InDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("Latin2InDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("LoVInDegrees", writeLonPositive_ ? util::normalise_longitude(reference[LLCOORDS::LON], 0)
                                                     : reference[LLCOORDS::LON]);

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", reference[LLCOORDS::LAT]);
    }

    // some extra keys are edition-specific, so parent call is here
    RegularGrid::fillGrib(info);
}


}  // namespace mir::repres::regular
