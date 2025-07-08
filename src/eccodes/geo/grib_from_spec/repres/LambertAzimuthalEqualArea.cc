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

#include <cmath>

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"


namespace mir::repres::regular {


static const RepresentationBuilder<LambertAzimuthalEqualArea> __builder("lambert_azimuthal_equal_area");


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const param::MIRParametrisation& param) :
    RegularGrid(param, make_projection(param)) {}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Projection& projection, const util::BoundingBox& bbox,
                                                     const LinearSpacing& x, const LinearSpacing& y,
                                                     const util::Shape& shape) :
    RegularGrid(projection, bbox, x, y, shape) {}


RegularGrid::Projection LambertAzimuthalEqualArea::make_projection(const param::MIRParametrisation& param) {
    auto spec = make_proj_spec(param);
    if (!spec.empty()) {
        return spec;
    }

    double standardParallel = 0.;
    double centralLongitude = 0.;
    double radius           = 0.;
    ASSERT(param.get("standardParallelInDegrees", standardParallel));
    ASSERT(param.get("centralLongitudeInDegrees", centralLongitude));
    param.get("radius", radius = util::Earth::radius());

    return Projection::Spec("type", "lambert_azimuthal_equal_area")
        .set("standard_parallel", standardParallel)
        .set("central_longitude", centralLongitude)
        .set("radius", radius);
}


void LambertAzimuthalEqualArea::fillGrib(grib_info& info) const {
    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    Point2 reference = grid().projection().lonlat({0., 0.});
    Point2 firstLL   = grid().projection().lonlat({x().front(), y().front()});

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LON];

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("standardParallelInDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("centralLongitudeInDegrees", reference[LLCOORDS::LON]);

    // some extra keys are edition-specific, so parent call is here
    RegularGrid::fillGrib(info);
}


}  // namespace mir::repres::regular
