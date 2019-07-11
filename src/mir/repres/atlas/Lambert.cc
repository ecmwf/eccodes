/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/atlas/Lambert.h"

#include "eckit/exception/Exceptions.h"

#include "atlas/util/Earth.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace repres {
namespace atlas {

static RepresentationBuilder<Lambert> __builder("lambert");

Lambert::Lambert(const param::MIRParametrisation& param) : AtlasRegularGrid(param, make_projection(param)) {}

AtlasRegularGrid::Projection Lambert::make_projection(const param::MIRParametrisation& param) {
    double LaDInDegrees;
    double LoVInDegrees;
    double Latin1InDegrees;
    double Latin2InDegrees;
    double radius;
    ASSERT(param.get("LaDInDegrees", LaDInDegrees));
    ASSERT(param.get("LoVInDegrees", LoVInDegrees));
    ASSERT(param.get("Latin1InDegrees", Latin1InDegrees));
    ASSERT(param.get("Latin2InDegrees", Latin2InDegrees));
    param.get("radius", radius = ::atlas::util::Earth::radius());

    ASSERT(radius > 0.);

    return AtlasRegularGrid::Projection::Spec("type", "lambert_conformal")
        .set("latitude1", Latin1InDegrees)
        .set("latitude2", Latin2InDegrees)
        .set("latitudeD", LaDInDegrees)
        .set("longitude0", LoVInDegrees)
        .set("radius", radius);
}

void Lambert::fill(grib_info&) const {
    NOTIMP;
}

}  // namespace atlas
}  // namespace repres
}  // namespace mir
