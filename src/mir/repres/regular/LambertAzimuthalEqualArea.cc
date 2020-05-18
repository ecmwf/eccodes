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


#include "mir/repres/regular/LambertAzimuthalEqualArea.h"

#include <string>

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace regular {

static RepresentationBuilder<LambertAzimuthalEqualArea> __builder("lambert_azimuthal_equal_area");

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const param::MIRParametrisation& param) :
    RegularGrid(param, make_projection(param)) {}

RegularGrid::Projection LambertAzimuthalEqualArea::make_projection(const param::MIRParametrisation& param) {
    std::string proj;
    if (param.get("proj", proj)) {
        return RegularGrid::make_projection_via_proj(param, proj);
    }

    double standardParallel;
    double centralLongitude;
    double radius;
    ASSERT(param.get("standardParallelInDegrees", standardParallel));
    ASSERT(param.get("centralLongitudeInDegrees", centralLongitude));
    param.get("radius", radius = ::atlas::util::Earth::radius());

    return Projection::Spec("type", "lambert_azimuthal_equal_area")
        .set("standard_parallel", standardParallel)
        .set("central_longitude", centralLongitude)
        .set("radius", radius);
}

void LambertAzimuthalEqualArea::fill(grib_info& info) const {

    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    ASSERT(x_.size() > 1);
    ASSERT(y_.size() > 1);

    auto Dx = (x_.max() - x_.min()) / (x_.size() - 1.);
    auto Dy = (y_.max() - y_.min()) / (y_.size() - 1.);
    ASSERT(Dx > 0.);
    ASSERT(Dy > 0.);

    Point2 reference = grid_.projection().lonlat({0., 0.});
    Point2 firstLL   = grid_.projection().lonlat({x_.front(), y_.front()});

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LON];
    info.grid.Ni                                 = long(x_.size());
    info.grid.Nj                                 = long(y_.size());

    GribExtraSetting::set(info, "xDirectionGridLengthInMetres", Dx);
    GribExtraSetting::set(info, "yDirectionGridLengthInMetres", Dy);
    GribExtraSetting::set(info, "standardParallelInDegrees", reference[LLCOORDS::LAT]);
    GribExtraSetting::set(info, "centralLongitudeInDegrees", reference[LLCOORDS::LON]);

    // some extra keys are edition-specific, so parent call is here
    RegularGrid::fill(info);
}

}  // namespace regular
}  // namespace repres
}  // namespace mir
