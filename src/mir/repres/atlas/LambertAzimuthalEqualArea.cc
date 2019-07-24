/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/atlas/LambertAzimuthalEqualArea.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Point2.h"

#include "atlas/util/Earth.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace atlas {

static RepresentationBuilder<LambertAzimuthalEqualArea> __builder("lambert_azimuthal_equal_area");

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const param::MIRParametrisation& param) :
    AtlasRegularGrid(param, make_projection(param)) {}

AtlasRegularGrid::Projection LambertAzimuthalEqualArea::make_projection(const param::MIRParametrisation& param) {
    double standardParallel;
    double centralLongitude;
    double radius;
    ASSERT(param.get("standardParallelInDegrees", standardParallel));
    ASSERT(param.get("centralLongitudeInDegrees", centralLongitude));
    param.get("radius", radius = ::atlas::util::Earth::radius());

    ASSERT(radius > 0.);

    return Projection::Spec("type", "lambert_azimuthal_equal_area")
        .set("standard_parallel", standardParallel)
        .set("central_longitude", centralLongitude)
        .set("radius", radius);
}

void LambertAzimuthalEqualArea::fill(grib_info& info) const {
    using namespace eckit::geometry;

    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    ASSERT(x_.size() > 1);
    ASSERT(y_.size() > 1);

    auto Dx = (x_.max() - x_.min()) / (x_.size() - 1);
    auto Dy = (y_.max() - y_.min()) / (y_.size() - 1);
    ASSERT(Dx > 0);
    ASSERT(Dy > 0);

    Point2 reference = grid_.projection().lonlat({0., 0.});
    Point2 firstLL   = grid_.projection().lonlat({x_.front(), y_.front()});

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[LON];
    info.grid.Ni                                 = long(x_.size());
    info.grid.Nj                                 = long(y_.size());

    GribExtraSetting::set(info, "xDirectionGridLengthInMillimetres", std::lround(Dx * 1.e3));
    GribExtraSetting::set(info, "yDirectionGridLengthInMillimetres", std::lround(Dy * 1.e3));
    GribExtraSetting::set(info, "standardParallelInMicrodegrees", std::lround(reference[LAT] * 1.e6));
    GribExtraSetting::set(info, "centralLongitudeInMicrodegrees", std::lround(reference[LON] * 1.e6));
}

}  // namespace atlas
}  // namespace repres
}  // namespace mir
