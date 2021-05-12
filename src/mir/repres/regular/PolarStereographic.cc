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


#include "mir/repres/regular/PolarStereographic.h"

#include "mir/util/Exceptions.h"


namespace mir {
namespace repres {
namespace regular {


static RepresentationBuilder<PolarStereographic> __builder("polar_stereographic");


PolarStereographic::PolarStereographic(const param::MIRParametrisation& param) :
    RegularGrid(param, make_projection(param)) {}


RegularGrid::Projection PolarStereographic::make_projection(const param::MIRParametrisation& param) {

    // only PROJ-based projection is available
    auto spec = make_proj_spec(param);
    ASSERT(!spec.empty());

    return spec;
}


void PolarStereographic::fill(grib_info& /*info*/) const {
    NOTIMP;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir
