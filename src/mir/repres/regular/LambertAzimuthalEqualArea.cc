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


const Representation* LambertAzimuthalEqualArea::croppedRepresentation(const util::BoundingBox& bbox) const {
    auto mm = minmax_ij(bbox);
    auto Ni = x().size();

    auto projection = grid().projection();
    ASSERT(projection);

    auto first = [this, projection, Ni](size_t firsti, size_t firstj) -> Point2 {
        for (std::unique_ptr<Iterator> it(iterator()); it->next();) {
            auto i = it->index() % Ni;
            auto j = it->index() / Ni;
            if (i == firsti && j == firstj) {
                const auto& latlon = *(*it);
                return projection.xy(PointLonLat{latlon[1], latlon[0]});
            }
        }
        throw exception::UserError("LambertAzimuthalEqualArea::croppedRepresentation: cannot find first point");
    }(mm.first.i, mm.first.j);

    auto spacex = linspace(first.x(), std::abs(x().step()), static_cast<long>(mm.second.i - mm.first.i + 1), xPlus());
    auto spacey = linspace(first.y(), std::abs(y().step()), static_cast<long>(mm.second.j - mm.first.j + 1), yPlus());

    return new LambertAzimuthalEqualArea(projection, bbox, spacex, spacey, shape());
}


}  // namespace mir::repres::regular
