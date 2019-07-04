/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <cmath>
#include <iostream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/Plural.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "atlas/util/Earth.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/atlas/LambertAzimuthalEqualArea.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace atlas {

namespace {
AtlasRegularGrid::Projection make_projection(double standardParallel, double centralLongitude,
                                             double radius = ::atlas::util::Earth::radius()) {
    ASSERT(radius > 0.);

    return AtlasRegularGrid::Projection::Spec()
        .set("type", "lambert_azimuthal_equal_area")
        .set("standard_parallel", standardParallel)
        .set("central_longitude", centralLongitude)
        .set("radius", radius);
}

AtlasRegularGrid::Projection make_projection(const param::MIRParametrisation& param) {
    double standardParallel;
    double centralLongitude;
    double radius;
    param.get("standardParallelInDegrees", standardParallel);
    param.get("centralLongitudeInDegrees", centralLongitude);
    param.get("radius", radius);

    return make_projection(standardParallel, centralLongitude, radius);
}

}  // namespace

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const param::MIRParametrisation& param) :
    AtlasRegularGrid(param, make_projection(param)) {}

LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(double x0, double x1, size_t nx, double y0, double y1, size_t ny,
                                                     double standardParallel, double centralLongitude) :
    AtlasRegularGrid({x0, x1, long(nx)}, {y0, y1, long(ny)}, make_projection(standardParallel, centralLongitude)) {}

LambertAzimuthalEqualArea::~LambertAzimuthalEqualArea() = default;

void LambertAzimuthalEqualArea::fill(grib_info& info) const {

    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    ASSERT(x_.size() > 1);
    ASSERT(y_.size() > 1);

    auto Dx = (x_.max() - x_.min()) / (x_.size() - 1);
    auto Dy = (y_.max() - y_.min()) / (y_.size() - 1);
    ASSERT(Dx > 0);
    ASSERT(Dy > 0);

    Point2 firstXY{x_.front(), y_.front()};
    Point2 reference = grid_.projection().lonlat({0., 0.});
    Point2 firstLL   = grid_.projection().lonlat(firstXY);

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[1];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[0];

    GribExtraSetting::set(info, "xDirectionGridLengthInMillimetres", std::lround(Dx * 1.e3));
    GribExtraSetting::set(info, "yDirectionGridLengthInMillimetres", std::lround(-Dy * 1.e3));
    GribExtraSetting::set(info, "numberOfPointsAlongXAxis", long(x_.size()));
    GribExtraSetting::set(info, "numberOfPointsAlongYAxis", long(y_.size()));
    GribExtraSetting::set(info, "standardParallelInMicrodegrees", std::lround(reference[1] * 1.e6));
    GribExtraSetting::set(info, "centralLongitudeInMicrodegrees", std::lround(reference[0] * 1.e6));
}

namespace {
static RepresentationBuilder<LambertAzimuthalEqualArea> lambertAzimuthalEqualArea(
    "lambert_azimuthal_equal_area");  // Name is what is returned by grib_api
}

}  // namespace atlas
}  // namespace repres
}  // namespace mir
