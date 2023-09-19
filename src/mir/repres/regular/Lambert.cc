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

    double LaDInDegrees;
    double LoVInDegrees;
    double Latin1InDegrees;
    double Latin2InDegrees;
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

    ASSERT(x_.size() > 1);
    ASSERT(y_.size() > 1);
    auto Dx = (x_.max() - x_.min()) / double(x_.size() - 1);
    auto Dy = (y_.max() - y_.min()) / double(y_.size() - 1);

    Point2 first     = {firstPointBottomLeft_ ? x_.min() : x_.front(), firstPointBottomLeft_ ? y_.min() : y_.front()};
    Point2 firstLL   = grid_.projection().lonlat(first);
    Point2 reference = grid_.projection().lonlat({0., 0.});

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL[LLCOORDS::LON], 0) : firstLL[LLCOORDS::LON];

    info.grid.Ni = long(x_.size());
    info.grid.Nj = long(y_.size());

    info.grid.latitudeOfSouthernPoleInDegrees  = latitudeOfSouthernPoleInDegrees_;
    info.grid.longitudeOfSouthernPoleInDegrees = longitudeOfSouthernPoleInDegrees_;
    info.grid.uvRelativeToGrid                 = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", Dx);
    info.extra_set("DyInMetres", Dy);
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
