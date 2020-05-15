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


#include "mir/repres/regular/Mercator.h"

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace repres {
namespace regular {


static RepresentationBuilder<Mercator> __builder("mercator");


Mercator::Mercator(const param::MIRParametrisation& param) : RegularGrid(param, make_projection(param)) {}


RegularGrid::Projection Mercator::make_projection(const param::MIRParametrisation& param) {

    double LaDInDegrees;
    double LonInDegrees;  // what to call this?
    double radius;
    ASSERT(param.get("LaDInDegrees", LaDInDegrees));
    param.get("LonInDegrees", LonInDegrees = 0.);
    param.get("radius", radius = ::atlas::util::Earth::radius());

    return Projection::Spec("type", "mercator")
        .set("longitude0", LonInDegrees)
        .set("latitude1", LaDInDegrees)
        .set("radius", radius);
}


void Mercator::fill(grib_info& /*info*/) const {
    NOTIMP;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir
