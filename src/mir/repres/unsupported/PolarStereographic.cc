/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"

#include "mir/repres/unsupported/PolarStereographic.h"

namespace mir {
namespace repres {


PolarStereographic::PolarStereographic(const param::MIRParametrisation &parametrisation) {

    ASSERT(parametrisation.get("Nx", Nx_));
    ASSERT(parametrisation.get("Ny", Ny_));

    ASSERT(parametrisation.get("DxInMetres", Dx_));
    ASSERT(parametrisation.get("DyInMetres", Dy_));

    ASSERT(parametrisation.get("latitudeOfFirstGridPointInDegrees", latitudeOfFirstGridPoint_));
    ASSERT(parametrisation.get("longitudeOfFirstGridPointInDegrees", longitudeOfFirstGridPoint_));
    ASSERT(parametrisation.get("orientationOfTheGridInDegrees", orientationOfTheGrid_));
    ASSERT(parametrisation.get("southPoleOnProjectionPlane", southPoleOnProjectionPlane_));
    ASSERT(parametrisation.get("radiusOfTheEarth", radiusOfTheEarth_));

    bool earthIsOblate;
    ASSERT(parametrisation.get("earthIsOblate", earthIsOblate));
    ASSERT(!earthIsOblate);

}

PolarStereographic::PolarStereographic() {
}


PolarStereographic::~PolarStereographic() {
}


void PolarStereographic::print(std::ostream &out) const {

    out << "PolarStereographic["
        << "Nx=" << Nx_
        << ",Ny=" << Ny_
        << ",Dx=" << Dx_
        << ",Dy=" << Dy_
        << ",longitudeOfFirstGridPoint=" << longitudeOfFirstGridPoint_
        << ",latitudeOfFirstGridPoint=" << latitudeOfFirstGridPoint_
        << ",orientationOfTheGrid=" << orientationOfTheGrid_
        << ",southPoleOnProjectionPlane=" << southPoleOnProjectionPlane_
        << ",radiusOfTheEarth=" << radiusOfTheEarth_

        << "]";
}

void PolarStereographic::fill(grib_info &info) const  {
    NOTIMP;
}

void PolarStereographic::validate(const std::vector<double> & values) const {
    ASSERT(values.size() == Nx_ * Ny_);
}


namespace {
static RepresentationBuilder<PolarStereographic> polarStereographic("polar_stereographic"); // Name is what is returned by grib_api
}

}  // namespace repres
}  // namespace mir

