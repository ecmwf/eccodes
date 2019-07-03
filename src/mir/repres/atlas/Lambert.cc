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

#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace repres {
namespace atlas {

namespace {
AtlasRegularGrid::Projection make_projection(double latitude1, double latitude2, double latitudeD, double longitude0,
                                             double radius = ::atlas::util::Earth::radius()) {
    ASSERT(radius > 0.);

    return AtlasRegularGrid::Projection::Spec()
        .set("type", "lambert_conformal")
        .set("latitude1", latitude1)
        .set("latitude2", latitude2)
        .set("latitudeD", latitudeD)
        .set("longitude0", longitude0)
        .set("radius", radius);
}

AtlasRegularGrid::Projection make_projection(const param::MIRParametrisation& param) {
    double LaDInDegrees;
    double LoVInDegrees;
    double Latin1InDegrees;
    double Latin2InDegrees;
    double radius;

    ASSERT(param.get("LaDInDegrees", LaDInDegrees));
    ASSERT(param.get("LoVInDegrees", LoVInDegrees));
    ASSERT(param.get("Latin1InDegrees", Latin1InDegrees));
    ASSERT(param.get("Latin2InDegrees", Latin2InDegrees));
    ASSERT(param.get("radius", radius));

    return make_projection(Latin1InDegrees, Latin2InDegrees, LaDInDegrees, LoVInDegrees, radius);
}

}  // (anonymous namespace)

Lambert::Lambert(const param::MIRParametrisation& param) :
    AtlasRegularGrid(param, make_projection(param)) {
    ASSERT(!param.has("rotation"));
}


Lambert::~Lambert() = default;


void Lambert::fill(grib_info&) const {
    NOTIMP;
}


namespace {
static RepresentationBuilder<Lambert> lambert("lambert"); // Name is what is returned by grib_api
}


}  // namespace atlas
}  // namespace repres
}  // namespace mir

