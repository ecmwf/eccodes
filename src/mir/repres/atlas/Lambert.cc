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
AtlasRegularGrid::Projection make_projection(double latitude1, double latitude2, double longitude0,
                                             double radius = ::atlas::util::Earth::radius()) {
    ASSERT(radius > 0.);

    return AtlasRegularGrid::Projection::Spec()
        .set("type", "lambert")
        .set("latitude1", latitude1)
        .set("latitude2", latitude2)
        .set("longitude0", longitude0)
        .set("radius", radius);
}

AtlasRegularGrid::Projection make_projection(const param::MIRParametrisation& param) {
    double latitude1;
    double latitude2;
    double longitude0;
    double radius;
    param.get("Latin1InDegrees", latitude1);
    param.get("Latin2InDegrees", latitude2);
    param.get("LoVInDegrees", longitude0);
    param.get("radius", radius);

    return make_projection(latitude1, latitude2, longitude0, radius);
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

