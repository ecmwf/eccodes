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

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace repres {
namespace regular {


static RepresentationBuilder<Lambert> __builder("lambert");


Lambert::Lambert(const param::MIRParametrisation& param) : RegularGrid(param, make_projection(param)) {}


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


void Lambert::fill(grib_info&) const {
    NOTIMP;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir
